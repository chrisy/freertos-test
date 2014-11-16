/** Font setup
 * \file src/fonts.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */



#include <config.h>
#include <cli/cli.h>
#include <stdio.h>
#include <fonts/fontem.h>
#include <fonts/font_all.h>

#include "fonts.h"

static int cmd_fontlist(struct cli *cli, int argc, const char *const *argv)
{
	font_print_all(cli->out);
	return 0;
}

void font_init(void)
{
	printf("Initializing font subsystem.\r\n");

	struct cli_command fontlist	= {
		.cmd = "fontlist",
		.brief = "List available fonts",
		.help = "Lists all available fonts and some basic details on each.",
		.fn = cmd_fontlist,
	};
	cli_addcmd(&fontlist);
}
