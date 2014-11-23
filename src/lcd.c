/** LCD stuff
 * \file src/lcd.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lcd.h"

uint8_t SECTION_FSMC_BANK1_3 lcd_framebuffer[LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT * 2];

void lcd_init(void)
{
    ASSERT(lcd_framebuffer != NULL);

    STM3210E_LCD_Init();

    LCD_Clear(LCD_COLOR_RED);
    LCD_Clear(LCD_COLOR_GREEN);
    LCD_Clear(LCD_COLOR_BLUE);
    LCD_Clear(LCD_COLOR_BLACK);
}

void lcd_refresh(void)
{
    LCD_Bitblt(lcd_framebuffer);
}

int32_t lcd_parsecolor(char *str)
{
    if (!strcasecmp(str, "red")) return LCD_COLOR_RED;
    else if (!strcasecmp(str, "green")) return LCD_COLOR_GREEN;
    else if (!strcasecmp(str, "blue")) return LCD_COLOR_BLUE;
    else if (!strcasecmp(str, "cyan")) return LCD_COLOR_CYAN;
    else if (!strcasecmp(str, "yellow")) return LCD_COLOR_YELLOW;
    else if (!strcasecmp(str, "magenta")) return LCD_COLOR_MAGENTA;
    else if (!strcasecmp(str, "white")) return LCD_COLOR_WHITE;
    else if (!strcasecmp(str, "black")) return LCD_COLOR_BLACK;

	return -1;
}
