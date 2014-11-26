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

#include <stm32/dma.h>

#define LCD_DMA_CHANNEL 0
const dma_ch_t *lcd_dma = &dma_streams[LCD_DMA_CHANNEL];

#define FRAMEBUFFER_SIZE (LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT)
uint8_t SECTION_FSMC_BANK1_3("lcd_framebuffer") __attribute__((aligned(4)))
    lcd_framebuffer[FRAMEBUFFER_SIZE * sizeof(uint16_t)];

#define LCD_DMA_SIZE (FRAMEBUFFER_SIZE / 2)

static volatile uint16_t lcd_bank;

static void lcd_refresh_interrupt(void *param, uint32_t flags);

void lcd_init(void)
{
    ASSERT(lcd_framebuffer != NULL);

    STM3210E_LCD_Init();
    LCD_Clear(LCD_COLOR_BLACK);

    dma_allocate(lcd_dma, 10, lcd_refresh_interrupt, NULL);
}

void lcd_refresh(void)
{
    LCD_Bitblt(lcd_framebuffer);
}

static void lcd_start_dma(uint32_t start, uint16_t length)
{
    lcd_dma->ch->CMAR = start;
    lcd_dma->ch->CNDTR = length;
    dma_enable(lcd_dma);
}

void lcd_refresh_dma(void)
{
    lcd_bank = 0;

    dma_disable(lcd_dma);   // just in case

    lcd_dma->ch->CCR =
        DMA_CCR1_MEM2MEM |
        DMA_CCR1_MINC |
        DMA_CCR1_PL_1 |
        DMA_CCR1_MSIZE_0 |
        DMA_CCR1_PSIZE_0 |
        DMA_CCR1_TEIE |
        DMA_CCR1_TCIE |
        DMA_CCR1_DIR;

    lcd_dma->ch->CPAR = (uint32_t)LCD_DMA_Prepare();

    lcd_start_dma((uint32_t)lcd_framebuffer, LCD_DMA_SIZE);
}

static void lcd_refresh_interrupt(void *param, uint32_t flags)
{
    dma_disable(lcd_dma);
    if (lcd_bank == 0) {
        lcd_bank = 1;
        lcd_start_dma((uint32_t)lcd_framebuffer + LCD_DMA_SIZE, LCD_DMA_SIZE);
    }
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
