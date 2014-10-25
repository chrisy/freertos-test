/** Simple CLI
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file lib/cli/cli.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _CLI_H
#define _CLI_H

#include <config.h>
#include <task.h>
#include <stdio.h>
#include <microrl.h>

struct cli {
    char            *name;          // name of the task
    TaskHandle_t    task;           // task reference
    FILE            *in;            // input stream
    FILE            *out;           // output stream
    char            **completions;  // for autocomplete
    int             completion_num; // number of entries malloc'ed
    microrl_t       rl;             // micro readline reference
};

struct cli_command {
    char    *cmd;
    char    *brief;
    char    *help;
    int     (*fn)(FILE *in, FILE *out, int argc, const char *const *argv);
};

void cli_init(void);
void cli_start(char *name, FILE *in, FILE *out);
void cli_stop(struct cli *cli);
void cli_addcmd(struct cli_command *cmd);
void cli_sortcmds(void);

#endif /* _CLI_H */
