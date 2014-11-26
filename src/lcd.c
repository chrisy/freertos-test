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

/** DMA channel number to use for LCD framebuffer transfers. */
#define LCD_DMA_CHANNEL 0
/** Reference to the DMA structure used for the LCD DMA channel. */
const dma_ch_t *lcd_dma = &dma_streams[LCD_DMA_CHANNEL];

/** The size of the framebuffer, in RGB words (16 bits) */
#define FRAMEBUFFER_SIZE (LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT)
/** The amount of framebuffer to refresh in each DMA transfer */
#define LCD_DMA_SIZE (FRAMEBUFFER_SIZE / 2)

/** The framebuffer for the LCD */
uint8_t lcd_framebuffer[FRAMEBUFFER_SIZE * sizeof(uint16_t)] \
    SECTION_FSMC_BANK1_3("lcd_framebuffer") ALIGN(4);

/** The section of the screen most recently refreshed; 0 or 1. */
static volatile uint16_t lcd_bank;

static void lcd_start_dma(uint32_t start, uint16_t length);
static void lcd_refresh_interrupt(void *param, uint32_t flags);

/** Initialize the LCD system */
void lcd_init(void)
{
    printf("Initializing LCD subsystem." EOL);
    ASSERT(lcd_framebuffer != NULL);

    STM3210E_LCD_Init();
    LCD_Clear(LCD_COLOR_BLACK);

    dma_allocate(lcd_dma, 10, lcd_refresh_interrupt, NULL);
}

/** Manually refresh the framebuffer onto the LCD */
void lcd_refresh(void)
{
    LCD_Bitblt(lcd_framebuffer);
}

/**
 * Trigger a DMA action to copy data from the framebuffer
 * to the LCD.
 * @param start The address in memory to start the transfer from
 * @param length The number of words to transfer. For this routine words
 *      are 16-bits long.
 */
void lcd_start_dma(uint32_t start, uint16_t length)
{
    lcd_dma->ch->CMAR = start;
    lcd_dma->ch->CNDTR = length;
    dma_enable(lcd_dma);
}

/**
 * Manually trigger a refresh of the framebuffer to the LCD using
 * DMA transfers. Because of transfer size limitations, the LCD
 * is refreshed in two DMA transfers of half the screen each. This
 * function triggers the first half; the second half is triggered when
 * the first half completes.
 */
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

/**
 * DMA transfer complete event. If only the first half of the
 * framebuffer was copied, trigger a transfer of the second half.
 */
static void lcd_refresh_interrupt(void *param, uint32_t flags)
{
    dma_disable(lcd_dma);
    if (lcd_bank == 0) {
        lcd_bank = 1;
        lcd_start_dma((uint32_t)lcd_framebuffer + LCD_DMA_SIZE, LCD_DMA_SIZE);
    }
}

/** Utility function to parse color 'names' into an RGB16 value. */
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
