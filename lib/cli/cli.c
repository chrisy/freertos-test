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
#include <posixio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"

static void cli_print(void *opaque, const char *str)
{
    struct cli *cli = (struct cli *)opaque;

    if (cli->out != NULL) {
        fputs(str, cli->out);
        fflush(cli->out);
    }
}

static int cli_exec(void *opaque, int argc, const char *const *argv)
{
    struct cli *cli = (struct cli *)opaque;

    fprintf(cli->out, "Execute:");
    while (argc) {
        fprintf(cli->out, " %s", *argv);
        argc--; argv++;
    }
    fputs("\r\n", cli->out);

    return 0;
}

static void cli_sigint(void *opaque)
{
    struct cli *cli = (struct cli *)opaque;

    (void)cli;
}

static void __attribute__ ((noreturn)) cli_task(void *param)
{
    struct cli *cli = (struct cli *)param;

    fprintf(stdout, "CLI task %s started.\r\n", cli->name);
    if(cli->out != stdout) {
    	fprintf(cli->out, "CLI task %s started.\r\n", cli->name);
    }

    microrl_init(&cli->rl, cli, cli_print);
    microrl_set_execute_callback(&cli->rl, cli_exec);
    microrl_set_sigint_callback(&cli->rl, cli_sigint);

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
    memset(cli, '\0', sizeof(struct cli));

    cli->name = malloc(strlen(name)+5);
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
