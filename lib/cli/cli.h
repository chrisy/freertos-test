/** Simple CLI
 * \file lib/cli/cli.h
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _CLI_H
#define _CLI_H

#include <config.h>
#include <task.h>
#include <stdio.h>
#include <microrl.h>

/**
 * An instance of the CLI.
 * We can concievably run more than one copy of the CLI againsts different
 * I/O streams. All CLI state must therefore be stored in an instance of
 * this structure.
 */
struct cli {
    char            *name;          ///< name of the task
    TaskHandle_t    task;           ///< task reference
    FILE            *in;            ///< input stream
    FILE            *out;           ///< output stream
    char            **completions;  ///< for autocomplete
    int             completion_num; ///< number of entries malloc'ed
    microrl_t       rl;             ///< micro readline reference
};

/**
 * An individual CLI command.
 */
struct cli_command {
    char    *cmd;                                                       ///< the command
    char    *brief;                                                     ///< brief description of the command
    char    *help;                                                      ///< more complete help text for the command
    int     (*fn)(struct cli *cli, int argc, const char *const *argv);  ///< function to call for the command
    char    ** (*completion)(int, const char *const *);                 ///< function to call for autocompletion of parameters
};

void cli_init(void);
void cli_start(char *name, FILE *in, FILE *out);
void cli_stop(struct cli *cli);
void cli_addcmd(struct cli_command *cmd);
void cli_sortcmds(void);

#endif /* _CLI_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
