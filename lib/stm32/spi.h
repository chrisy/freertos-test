/*
 * Copyright (c) Michael Tharp <gxti@partiallystapled.com>
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _SPI_H
#define _SPI_H

#include <config.h>
#include <semphr.h>
#include <stm32/dma.h>


typedef struct {
    SPI_TypeDef         *spi;
    const dma_ch_t      *tx_dma;
    const dma_ch_t      *rx_dma;
    uint32_t            tx_dma_mode;
    uint32_t            rx_dma_mode;

    GPIO_TypeDef        *cs_pad;
    uint8_t             cs_pin;

    SemaphoreHandle_t   sem;
} spi_t;

#if USE_SPI1
extern spi_t SPI1_Dev;
#endif
#if USE_SPI2
extern spi_t SPI2_Dev;
#endif
#if USE_SPI3
extern spi_t SPI3_Dev;
#endif

void spi_start(spi_t *spi, uint32_t cr1);
void spi_exchange(spi_t *spi, const uint8_t *tx_buf, uint8_t *rx_buf, uint16_t size);

#if USE_SPI1
void SPI1_IRQHandler(void) __attribute__ ((interrupt));
#endif
#if USE_SPI2
void SPI2_IRQHandler(void) __attribute__ ((interrupt));
#endif
#if USE_SPI3
void SPI3_IRQHandler(void) __attribute__ ((interrupt));
#endif

#define spi_select(spi) { (spi)->cs_pad->BRR = (1 << (spi)->cs_pin); }
#define spi_deselect(spi) { (spi)->cs_pad->BSRR = (1 << (spi)->cs_pin); }

#endif

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
