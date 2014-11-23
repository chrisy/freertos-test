/** LCD setup
 * \file src/lcd.h
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>
#include <stm3210e_eval_lcd.h>

extern uint8_t lcd_framebuffer[];

void lcd_init(void);
void lcd_refresh(void);
int32_t lcd_parsecolor(char *str);

#endif /* _LCD_H */
