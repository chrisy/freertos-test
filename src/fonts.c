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
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <fonts/fontem.h>
#include <fonts/font_all.h>

#include "fonts.h"
#include "lcd.h"

/** The font currently selected from the command line. */
const struct font *current_font = &font_UbuntuMonoR_18;

/** Command to list currently available fonts. */
static int cmd_fontlist(struct cli *cli, int argc, const char *const *argv)
{
    font_print_all(cli->out);

    return 0;
}

/** Command to select a font. */
static int cmd_fontsel(struct cli *cli, int argc, const char *const *argv)
{
    int c;
    int size = 10;
    char *type = NULL;

    optind = 0;
    opterr = 0;
    while ((c = getopt(argc, (char *const *)argv, "s:t:")) != EOF) {
        switch (c) {
        case 's':     // set font size
            size = atoi(optarg);
            break;

        case 't':     // set font weight/style/type
            type = optarg;
            break;

        case ':':
            fprintf(cli->out, "Option \"%s\" requires a parameter." EOL,
                    argv[optind - 1]);
            return 1;

        default:
            fprintf(cli->out, "Unknown option \"%s\"." EOL, argv[optind - 1]);
            return 1;
        }
    }

    if (argv[optind] != NULL) {
        const struct font *f = font_find(argv[optind], type, size);

        if (f == NULL) {
            fprintf(cli->out, "Unable to find a font named \"%s\" of type " \
                              "\"%s\" and size %d." EOL,
                    argv[optind], type ? type : "<any>", size);
            return 1;
        }

        current_font = f;
    }

    fprintf(cli->out, "%s, %s, %d." EOL,
            current_font->name,
            current_font->style,
            current_font->size);

    return 0;
}

/** Command to print a string to the LCD using the current font. */
static int cmd_fontprint(struct cli *cli, int argc, const char *const *argv)
{
    int c;
    int x = 0, y = 0;
    uint16_t color = LCD_COLOR_WHITE;
    int32_t tcolor;

    optind = 0;
    opterr = 0;
    while ((c = getopt(argc, (char *const *)argv, "c:x:y:")) != EOF) {
        switch (c) {
        case 'c':     // color
            tcolor = lcd_parsecolor(optarg);
            if (tcolor == -1) {
                fprintf(cli->out, "Unknown color: \"%s\"." EOL, optarg);
                return 1;
            }
            color = tcolor;
            break;

        case 'x':     // x coord
            x = atoi(optarg);
            break;

        case 'y':     // y coord
            y = atoi(optarg);
            break;

        case ':':
            fprintf(cli->out, "Option \"%s\" requires a parameter." EOL,
                    argv[optind - 1]);
            return 1;

        default:
            fprintf(cli->out, "Unknown option \"%s\"." EOL, argv[optind - 1]);
            return 1;
        }
    }

    int len = 0;
    for (int i = optind; i < argc; i++)
        len += strlen(argv[i]) + 1;

    if (!len) return 0;  // optimization

    char *str = malloc(len);
    if (str == NULL) {
        fprintf(cli->out, "Unable to allocate %d bytes." EOL, len);
        return 1;
    }
    *str = '\0';

    for (int i = optind; i < argc; i++) {
        if (i != optind) strcat(str, " ");
        strcat(str, argv[i]);
    }

    font_draw_string_RGB16(current_font,
                           x, y, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT,
                           lcd_framebuffer, str, 0, color);
    lcd_refresh_dma();

    free(str);

    return 0;
}

/** Command to clear the LCD. */
static int cmd_fontclear(struct cli *cli, int argc, const char *const *argv)
{
    int c;
    uint16_t color = LCD_COLOR_BLACK;
    int32_t tcolor;

    optind = 0;
    opterr = 0;
    while ((c = getopt(argc, (char *const *)argv, "c:")) != EOF) {
        switch (c) {
        case 'c':     // color
            tcolor = lcd_parsecolor(optarg);
            if (tcolor == -1) {
                fprintf(cli->out, "Unknown color: %s." EOL, optarg);
                return 1;
            }
            color = tcolor;
            break;

        case ':':
            fprintf(cli->out, "Option \"%s\" requires a parameter." EOL,
                    argv[optind - 1]);
            return 1;

        default:
            fprintf(cli->out, "Unknown option \"%s\"." EOL, argv[optind - 1]);
            return 1;
        }
    }

    uint16_t *p = (uint16_t *)lcd_framebuffer;
    for (int i = 0; i < LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH; i++)
        *p++ = color;
    lcd_refresh_dma();

    return 0;
}

/** Initialize the font system */
void font_init(void)
{
    printf("Initializing font subsystem." EOL);

    struct cli_command fontlist = {
        .cmd    = "fontlist",
        .brief  = "List available fonts",
        .help   = "Lists all available fonts and some basic details on each.",
        .fn     = cmd_fontlist,
    };
    cli_addcmd(&fontlist);

    struct cli_command fontsel = {
        .cmd    = "fontsel",
        .brief  = "Select current font",
        .help   = "Selects a font for future 'fontprint' actions." EOL EOL \
                  "Options:" EOL \
                  "  -s <size>     Specifies the desired font size." EOL \
                  "  -t <type>     Specifies the desired font type, or style.",
        .fn     = cmd_fontsel,
    };
    cli_addcmd(&fontsel);

    struct cli_command fontprint = {
        .cmd    = "fontprint",
        .brief  = "Print a string on the LCD",
        .help   = "Prints a string on the LCD in the current font." EOL EOL \
                  "Options: " EOL \
                  "  -c <color>    Specifies the desired text color." EOL \
                  "  -x <coord>    Specifies the x coordinate." EOL \
                  "  -y <coord>    Specifies the y coordinate.",
        .fn     = cmd_fontprint,
    };
    cli_addcmd(&fontprint);

    struct cli_command fontclear = {
        .cmd    = "fontclear",
        .brief  = "Clear the LCD",
        .help   = "Clears the LCD to a selectable color." EOL EOL \
                  "Options: " EOL \
                  "  -c <color>    Specifies the desired background color." EOL,
        .fn     = cmd_fontclear,
    };
    cli_addcmd(&fontclear);
}
