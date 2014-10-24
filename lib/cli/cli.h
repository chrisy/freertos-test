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
	char *name;
	TaskHandle_t task;
    FILE        *in;
    FILE        *out;
    microrl_t   rl;
};


void cli_start(char *name, FILE *in, FILE *out);
void cli_stop(struct cli *cli);

#endif /* _CLI_H */
