/** Simple CLI
 * \file lib/cli/cli.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <microrl.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <posixio/posixio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"

/** The maximum time we'll wait for our semaphore. */
#define SEM_DELAY (5000 / portTICK_PERIOD_MS)

#if configUSE_TRACE_FACILITY
#define CLI_TASKCMDS
#endif


/** A list of all the CLI commands available. */
static struct cli_command *cli_commands = NULL;
/** Count of the number of commands in the cli_commands list. */
static int cli_command_num = 0;
/** A semaphore to protect some critical sections of the CLI. */
static SemaphoreHandle_t cmd_sem = NULL;
/** A flag indicating whether the current list of commands has been sorted or not. */
static char cli_commands_sorted = 0;


static int cli_exec(void *opaque, int argc, const char *const *argv);
#ifdef _USE_COMPLETE
static char **cli_autocomplete(void *opaque, int argc, const char *const *argv);
#endif
static int cmd_help(struct cli *cli, int argc, const char *const *argv);
static int cmd_echo(struct cli *cli, int argc, const char *const *argv);
#ifdef CLI_TASKCMDS
static int cmd_tasks(struct cli *cli, int argc, const char *const *argv);
#endif
#ifdef _USE_HISTORY
static int cmd_history(struct cli *cli, int argc, const char *const *argv);
#endif


/**
 * Initialize the CLI. Called once at platform startup this creates a
 * set of basic commands that the CLI will respond to.
 */
void cli_init(void)
{
    cmd_sem = xSemaphoreCreateMutex();

    struct cli_command help = {
        .cmd    = "help",
        .brief  = "Displays brief help for each command available",
        .fn     = cmd_help,
    };
    cli_addcmd(&help);

    struct cli_command echo = {
        .cmd    = "echo",
        .brief  = "Prints its parameters back to the CLI",
        .help   = "A simple command to print any provided parameters on the CLI.",
        .fn     = cmd_echo,
    };
    cli_addcmd(&echo);

#ifdef CLI_TASKCMDS
    struct cli_command tasks = {
        .cmd    = "tasks",
        .brief  = "Prints a list of tasks that the RTOS is managing",
        .help   = "Outputs a formatted list of the tasks currently " \
                  "managed by the RTOS.",
        .fn     = cmd_tasks,
    };
    cli_addcmd(&tasks);
#endif

#ifdef _USE_HISTORY
    struct cli_command history = {
        .cmd    = "history",
        .brief  = "Prints the history of commands entered",
        .help   = "Output the command history. This is a finite-sized list.",
        .fn     = cmd_history,
    };
    cli_addcmd(&history);
#endif
}


/**
 * A helper routine for the readline library that will send a string
 * to the output stream for a given CLI instance.
 */
static void cli_print(void *opaque, const char *str)
{
    struct cli *cli = (struct cli *)opaque;

    if (cli->out != NULL) {
        fputs(str, cli->out);
        fflush(cli->out);
    }
}


/**
 * Helper routine called by the readline library when control-C is
 * seen in the input stream.
 */
static void cli_sigint(void *opaque)
{
    struct cli *cli = (struct cli *)opaque;

    (void)cli;
}


/**
 * The RTOS task that implements the CLI.
 *
 * @param param An opaque reference to the instance of the CLI this task
 * is servicing.
 */
static void __attribute__ ((noreturn)) cli_task(void *param)
{
    struct cli *cli = (struct cli *)param;

    if (cli->in == NULL)
        cli->in = stdin;
    if (cli->out == NULL)
        cli->out = stdout;
    else
        fprintf(cli->out, "CLI task %s started.\r\n", cli->name);

    fprintf(stdout, "CLI task %s started.\r\n", cli->name);

    microrl_init(&cli->rl, cli, cli_print);
    microrl_set_execute_callback(&cli->rl, cli_exec);
    microrl_set_sigint_callback(&cli->rl, cli_sigint);
#ifdef _USE_COMPLETE
    microrl_set_complete_callback(&cli->rl, cli_autocomplete);
#endif

    for (;; ) {
        int ch = fgetc(cli->in);
        if (ch != EOF)
            microrl_insert_char(&cli->rl, (char)(ch & 0xff));
    }
}


/**
 * Creates an instance of the CLI attached to the I/O streams supplied.
 *
 * @param name The name of the CLI task. This string will be prepended with
 *      "cli_" when the task is created.
 * @param in The FILE stream from which input characters are to be read.
 * @param out The FILE stream to which output strings are to be sent.
 */
void cli_start(char *name, FILE *in, FILE *out)
{
    struct cli *cli;

    cli = malloc(sizeof(struct cli));
    ASSERT(cli != NULL);
    memset(cli, '\0', sizeof(struct cli));

    cli->name = malloc(strlen(name) + 5);
    ASSERT(cli->name != NULL);
    strcpy(cli->name, "cli_");
    strcat(cli->name, name);
    cli->in = in;
    cli->out = out;

    xTaskCreate(cli_task, cli->name,
                STACK_SIZE_CLI, cli,
                THREAD_PRIO_CLI, &cli->task);
}


/**
 * Stops a running CLI task.
 *
 * @param cli The reference to a CLI instance to stop.
 */
void cli_stop(struct cli *cli)
{
    vTaskDelete(cli->task);
    free(cli->name);
    free(cli);
}


/**
 * Adds a command to the list of commands the CLI will respond to.
 *
 * @param cmd A command to add. The contents of this structure are copied
 *      into the command list.
 */
void cli_addcmd(struct cli_command *cmd)
{
    xSemaphoreTake(cmd_sem, SEM_DELAY);
    if (cli_commands == NULL)
        cli_commands = malloc(sizeof(struct cli_command));
    else
        cli_commands = realloc(cli_commands, sizeof(struct cli_command) * (cli_command_num + 1));
    ASSERT(cli_commands != NULL);
    memcpy(&cli_commands[cli_command_num], cmd, sizeof(struct cli_command));
    cli_command_num++;
    xSemaphoreGive(cmd_sem);

    cli_commands_sorted = 0;
}


/** qsort helper routine used when sorting the command list. */
static int cli_cmpstringp(const void *p1, const void *p2)
{
    struct cli_command *c1 = (struct cli_command *)p1;
    struct cli_command *c2 = (struct cli_command *)p2;

    return stricmp(c1->cmd, c2->cmd);
}


/**
 * Sorts the command list into ascending alphanumerical command name order.
 */
void cli_sortcmds(void)
{
    xSemaphoreTake(cmd_sem, SEM_DELAY);
    qsort(cli_commands, cli_command_num, sizeof(struct cli_command), cli_cmpstringp);
    xSemaphoreGive(cmd_sem);
    cli_commands_sorted = 1;
}


/**
 * Called by the readline library when an input string should be interpreted
 * and executed.
 *
 * This function searches for the command in the list of commands. The
 * command is the first word in the string.
 *
 * If a command is not found in the list then an error is printed and no
 * further action taken.
 *
 * If a command is found and it defines a function to call then that function
 * is called with the complete command line in the argc and argv parameters.
 *
 * @returns -1 when the command is not found or the value returned by the
 *      commands function.
 */
int cli_exec(void *opaque, int argc, const char *const *argv)
{
    struct cli *cli = (struct cli *)opaque;

    int (*fn)(struct cli *cli, int argc, const char *const *argv) = NULL;
    int ret = 0;

    xSemaphoreTake(cmd_sem, SEM_DELAY);

    for (int i = 0; i < cli_command_num; i++) {
        if (!strcmp(argv[0], cli_commands[i].cmd)) {
            // drop this in a var so we call it outside our mutex
            fn = cli_commands[i].fn;
            break;
        }
    }
    xSemaphoreGive(cmd_sem);

    if (fn != NULL) {
        ret = fn(cli, argc, argv);
    } else {
        fprintf(cli->out, "Command '%s' not found.\r\n", argv[0]);
        ret = -1;
    }
    return ret;
}


#ifdef _USE_COMPLETE
/**
 * Used by the readline library when "tab" is pressed to discover all
 * partially matching commands.
 */
char **cli_autocomplete(void *opaque, int argc, const char *const *argv)
{
    struct cli *cli = (struct cli *)opaque;

    if (!cli_commands_sorted)
        cli_sortcmds();

    if (cli->completion_num != cli_command_num) {
        if (cli->completions != NULL)
            free((void *)cli->completions);
        cli->completions = (void *)malloc(sizeof(char *) * (cli_command_num + 1));
        ASSERT(cli->completions != NULL);
        cli->completion_num = cli_command_num;
    }

    xSemaphoreTake(cmd_sem, SEM_DELAY);
    int c = 0;
    if (argc == 0) {
        // with no tokens, just return the whole list
        for (; c < cli_command_num; c++)
            cli->completions[c] = cli_commands[c].cmd;
        xSemaphoreGive(cmd_sem);
    } else if (argc == 1) {
        // with one token, match against the command list
        int len = strlen(argv[0]);
        for (int i = 0; i < cli_command_num; i++)
            if (!strncmp(argv[0], cli_commands[i].cmd, len))
                cli->completions[c++] = cli_commands[i].cmd;
        xSemaphoreGive(cmd_sem);
    } else {
        // find the first token in the command list
        int i;
        for (i = 0; i < cli_command_num; i++)
            if (!strcmp(argv[0], cli_commands[i].cmd)) break;
        xSemaphoreGive(cmd_sem);
        // then see if that command has an auto complete method
        if (i < cli_command_num && cli_commands[i].completion != NULL)
            cli_commands[i].completion(argc, argv);
    }
    cli->completions[c] = NULL;

    return cli->completions;
}
#endif


/**
 * A simple "help" command that prints the list of commands with their
 * "brief" text. If invoked with the name of a command as a parameter then
 * it prints the more detailed help text, if available.
 */
int cmd_help(struct cli *cli, int argc, const char *const *argv)
{
    if (argc == 1) {
        // display the brief help for every command
        if (!cli_commands_sorted)
            cli_sortcmds();

        xSemaphoreTake(cmd_sem, SEM_DELAY);
        for (int i = 0; i < cli_command_num; i++) {
            fprintf(cli->out, "%-20s %s\r\n",
                    cli_commands[i].cmd,
                    cli_commands[i].brief ? cli_commands[i].brief : "");
        }
        xSemaphoreGive(cmd_sem);
    } else if (argc == 2) {
        // find a specific command and display its verbose help text
        int i;
        xSemaphoreTake(cmd_sem, SEM_DELAY);
        for (i = 0; i < cli_command_num; i++)
            if (!strcmp(argv[1], cli_commands[i].cmd)) break;

        if (i < cli_command_num) {
            fprintf(cli->out, "Help for '%s':\r\n\r\n", argv[1]);
            fprintf(cli->out, "%s\r\n",
                    cli_commands[i].help ? cli_commands[i].help :
                    cli_commands[i].brief ? cli_commands[i].brief :
                    "");
        } else {
            fprintf(cli->out, "Command '%s' not found.\r\n", argv[1]);
        }
        xSemaphoreGive(cmd_sem);
    } else {
        fprintf(cli->out, "Too many parameters given.\r\n");
        return -1;
    }

    return 0;
}


/**
 * Simple "echo" command. Simply prints to the output stream whatever
 * was presented as parameters to the command.
 */
int cmd_echo(struct cli *cli, int argc, const char *const *argv)
{
    argc--; argv++;

    while (argc--)
        fprintf(cli->out, "%s%s", *argv++, argc ? " " : "");

    fprintf(cli->out, "\r\n");

    return 0;
}


#ifdef CLI_TASKCMDS
/** qsort helper to sort tasks by id. */
static int cmd_taskidcmp(const void *p1, const void *p2)
{
    TaskStatus_t *t1 = (TaskStatus_t *)p1;
    TaskStatus_t *t2 = (TaskStatus_t *)p2;

    return t1->xTaskNumber - t2->xTaskNumber;
}


/**
 * A command that prints the list of currently defined tasks the RTOS
 * scheduler is aware of.
 */
int cmd_tasks(struct cli *cli, int argc, const char *const *argv)
{
    int count = uxTaskGetNumberOfTasks();
    TaskStatus_t *tasks;
    char status;

    tasks = malloc(count * sizeof(TaskStatus_t));
    if (tasks == NULL) {
        fprintf(cli->out, "malloc failed\r\n");
        return -1;
    }

    // Get the task state
    count = uxTaskGetSystemState(tasks, count, NULL);

    fprintf(cli->out, "%-5s %-16s %5s %4s %8s"
#if configGENERATE_RUN_TIME_STATS
            " %8s"
#endif
            "\r\n",
            "ID",
            "Task name",
            "State",
            "Prio",
            "Stack"
#if configGENERATE_RUN_TIME_STATS
            ,
            "CPU time"
#endif
            );

    qsort(tasks, count, sizeof(TaskStatus_t), cmd_taskidcmp);

    /* Create a human readable table from the binary data. */
    for (int i = 0; i < count; i++) {
        switch (tasks[i].eCurrentState) {
        case eReady:
            status = 'R';
            break;

        case eBlocked:
            status = 'B';
            break;

        case eSuspended:
            status = 'S';
            break;

        case eDeleted:
            status = 'D';
            break;

        default:
            status = '?';
            break;
        }

        fprintf(cli->out, "%-5u %-16s   %c   %4u %8u"
#if configGENERATE_RUN_TIME_STATS
                " %8lu"
#endif
                "\r\n",
                (unsigned int)tasks[i].xTaskNumber,
                tasks[i].pcTaskName,
                status,
                (unsigned int)tasks[i].uxCurrentPriority,
                (unsigned int)tasks[i].usStackHighWaterMark
#if configGENERATE_RUN_TIME_STATS
                ,
                (((unsigned long)tasks[i].ulRunTimeCounter) * 10UL) / 12UL
#endif
                );
    }

    return 0;
}
#endif


#ifdef _USE_HISTORY
/**
 * Command that prints the current readline history buffer.
 */
int cmd_history(struct cli *cli, int argc, const char *const *argv)
{
    microrl_print_history(&cli->rl);
    return 0;
}
#endif

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
