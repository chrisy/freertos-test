/** Simple CLI
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file lib/cli/cli.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <microrl.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <posixio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"

#define SEM_DELAY (5000 / portTICK_PERIOD_MS)

static struct cli_command *cli_commands = NULL;
static int cli_command_num = 0;
static SemaphoreHandle_t cmd_sem = NULL;
static char cli_commands_sorted = 0;


static int cli_exec(void *opaque, int argc, const char *const *argv);
#ifdef _USE_COMPLETE
static char **cli_autocomplete(void *opaque, int argc, const char *const *argv);
#endif
static int cmd_help(FILE *in, FILE *out, int argc, const char *const *argv);


void cli_init(void)
{
    cmd_sem = xSemaphoreCreateMutex();

    struct cli_command help = {
        .cmd    = "help",
        .brief  = "Displays brief help for each command available",
        .fn     = cmd_help,
    };
    cli_addcmd(&help);
}

static void cli_print(void *opaque, const char *str)
{
    struct cli *cli = (struct cli *)opaque;

    if (cli->out != NULL) {
        fputs(str, cli->out);
        fflush(cli->out);
    }
}

static void cli_sigint(void *opaque)
{
    struct cli *cli = (struct cli *)opaque;

    (void)cli;
}

static void __attribute__ ((noreturn)) cli_task(void *param)
{
    struct cli *cli = (struct cli *)param;

    if (in == NULL)
        cli->in = stdin;
    if (out == NULL)
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

    xTaskCreate(cli_task, cli->name, 4096, cli, 4, &cli->task);
}

void cli_stop(struct cli *cli)
{
    free(cli->name);
    vTaskDelete(cli->task);
    free(cli);
}

void cli_addcmd(struct cli_command *cmd)
{
    xSemaphoreTake(cmd_sem, SEM_DELAY);
    if (cli_commands == NULL)
        cli_commands = malloc(sizeof(struct cli_command));
    else
        cli_commands = realloc(cli_commands, sizeof(struct cli_command) * cli_command_num + 1);
    ASSERT(cli_commands != NULL);
    memcpy(&cli_commands[cli_command_num], cmd, sizeof(struct cli_command));
    cli_command_num++;
    xSemaphoreGive(cmd_sem);

    cli_commands_sorted = 0;
}

static int cli_cmpstringp(const void *p1, const void *p2)
{
    struct cli_command *c1 = *(struct cli_command **)p1;
    struct cli_command *c2 = *(struct cli_command **)p2;

    return stricmp(c1->cmd, c2->cmd);
}

void cli_sortcmds(void)
{
    xSemaphoreTake(cmd_sem, SEM_DELAY);
    qsort(cli_commands, cli_command_num, sizeof(struct cli_command), cli_cmpstringp);
    xSemaphoreGive(cmd_sem);
    cli_commands_sorted = 1;
}

int cli_exec(void *opaque, int argc, const char *const *argv)
{
    struct cli *cli = (struct cli *)opaque;

    int (*fn)(FILE *in, FILE *out, int argc, const char *const *argv) = NULL;
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
        ret = fn(cli->in, cli->out, argc, argv);
    } else {
        fprintf(cli->out, "Command '%s' not found.\r\n", argv[0]);
        ret = -1;
    }
    return ret;
}


#ifdef _USE_COMPLETE
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
        for (; c < cli_command_num; c++)
            cli->completions[c] = cli_commands[c].cmd;
    } else if (argc == 1) {
        int len = strlen(argv[0]);
        for (int i = 0; i < cli_command_num; i++) {
            if (!strncmp(argv[0], cli_commands[i].cmd, len))
                cli->completions[c++] = cli_commands[i].cmd;
        }
    }
    cli->completions[c] = NULL;
    xSemaphoreGive(cmd_sem);

    return cli->completions;
}
#endif

int cmd_help(FILE *in, FILE *out, int argc, const char *const *argv)
{
    if (!cli_commands_sorted)
        cli_sortcmds();

    for (int i = 0; i < cli_command_num; i++) {
        fprintf(out, "%-20s %s\r\n",
                cli_commands[i].cmd,
                cli_commands[i].brief ? cli_commands[i].brief : "");
    }

    return 0;
}
