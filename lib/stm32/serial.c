/*
 * Copyright (c) Michael Tharp <gxti@partiallystapled.com>
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */


#include <config.h>
#include <task.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include "serial.h"

#if USE_SERIAL_USART1
serial_t Serial1;
#endif
#if USE_SERIAL_USART2
serial_t Serial2;
#endif
#if USE_SERIAL_USART3
serial_t Serial3;
#endif
#if USE_SERIAL_UART4
serial_t Serial4;
#endif
#if USE_SERIAL_UART5
serial_t Serial5;
#endif

#define _serial_putc(serial, val_ptr, timeout) \
    do { xQueueSend(serial->tx_q, val_ptr, timeout); \
         serial->usart->CR1 |= USART_CR1_TXEIE; } while (0)


static void usart_tcie(void *param, uint32_t flags);


void
serial_start(serial_t *serial, int speed
#if configUSE_QUEUE_SETS
             , QueueSetHandle_t queue_set
#endif
             )
{
    IRQn_Type irqn = 0;

    ASSERT((serial->rx_q = xQueueCreate(SERIAL_RX_SIZE, 1)));
    ASSERT((serial->mutex = xSemaphoreCreateMutex()));
#if configUSE_QUEUE_SETS
    if (queue_set)
        /* Must be added to set while it's still empty */
        xQueueAddToSet(serial->rx_q, queue_set);

#endif

    serial->speed = speed;
    serial->tx_dma = NULL;

    // we alter GPIO settings which *might* be worked on
    // elsewhere, so disable interrupts
    taskENTER_CRITICAL();

    uint32_t gpioa_crl = GPIOA->CRL;
    uint32_t gpioa_crh = GPIOA->CRH;
    uint32_t gpiob_crh = GPIOB->CRH;
    uint32_t gpioc_crh = GPIOC->CRH;
    uint32_t gpiod_crl = GPIOD->CRL;

#if USE_SERIAL_USART1
    if (serial == &Serial1) {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        gpioa_crh &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
        gpioa_crh |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;
        gpioa_crh &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        gpioa_crh |= GPIO_CRH_CNF10_0;
        irqn = USART1_IRQn;
        serial->usart = USART1;
        serial->tx_dma = &dma_streams[3];
    } else
#endif
#if USE_SERIAL_USART2
    if (serial == &Serial2) {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        gpioa_crl &= ~(GPIO_CRH_MODE2 | GPIO_CRH_CNF2);
        gpioa_crl |= GPIO_CRH_MODE2_0 | GPIO_CRH_MODE2_1 | GPIO_CRH_CNF2_1;
        gpioa_crl &= ~(GPIO_CRH_MODE3 | GPIO_CRH_CNF3);
        gpioa_crl |= GPIO_CRH_CNF3_0;
        irqn = USART2_IRQn;
        serial->usart = USART2;
        serial->tx_dma = &dma_streams[6];
    } else
#endif
#if USE_SERIAL_USART3
    if (serial == &Serial3) {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        gpiob_crh &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        gpiob_crh |= GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1;
        gpiob_crh &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
        gpiob_crh |= GPIO_CRH_CNF11_0;
        irqn = USART3_IRQn;
        serial->usart = USART3;
        serial->tx_dma = &dma_streams[1];
    } else
#endif
#if USE_SERIAL_UART4
    if (serial == &Serial4) {
        RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
        gpioc_crh &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
        gpioc_crh |= GPIO_CRH_MODE10_0 | GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1;
        gpioc_crh &= ~(GPIO_CRH_MODE11 | GPIO_CRH_CNF11);
        gpioc_crh |= GPIO_CRH_CNF11_0;
        irqn = UART4_IRQn;
        serial->usart = UART4;
        serial->tx_dma = &dma_streams[11];
    } else
#endif
#if USE_SERIAL_UART5
    if (serial == &Serial5) {
        RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
        gpioc_crh &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12);
        gpioc_crh |= GPIO_CRH_MODE12_0 | GPIO_CRH_MODE12_1 | GPIO_CRH_CNF12_1;
        gpiod_crl &= ~(GPIO_CRH_MODE2 | GPIO_CRH_CNF2);
        gpiod_crl |= GPIO_CRH_CNF2_0;
        irqn = UART5_IRQn;
        serial->usart = UART5;
    } else
#endif
    {
        taskEXIT_CRITICAL();
        HALT_WITH_MSG("invalid serial port");
    }

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
#if defined (STM32F10X_HD) || defined  (STM32F10X_CL) || defined  (STM32F10X_HD_VL)
    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
#endif
    GPIOA->CRL = gpioa_crl;
    GPIOA->CRH = gpioa_crh;
    GPIOB->CRH = gpiob_crh;
    GPIOC->CRH = gpioc_crh;
    GPIOD->CRL = gpiod_crl;

    // finished with GPIO, we can release this now
    taskEXIT_CRITICAL();

    NVIC_SetPriority(irqn, IRQ_PRIO_USART);
    NVIC_EnableIRQ(irqn);
    serial_set_speed(serial);
    serial->usart->CR3 = 0;

    if (serial->tx_dma) {
        dma_allocate(serial->tx_dma, IRQ_PRIO_USART, usart_tcie, serial);
        serial->tx_dma->ch->CPAR = (uint32_t)&serial->usart->DR;
        serial->usart->CR3 |= USART_CR3_DMAT;
        serial->tx_q = NULL;
        ASSERT((serial->tcie_sem = xSemaphoreCreateBinary()));
    } else {
        ASSERT((serial->tx_q = xQueueCreate(SERIAL_TX_SIZE, 1)));
        serial->tcie_sem = NULL;
    }

    serial->usart->CR1 = 0
                         | USART_CR1_UE
                         | USART_CR1_TE
                         | USART_CR1_RE
                         | USART_CR1_RXNEIE
    ;
}


void
serial_set_speed(serial_t *serial)
{
    USART_TypeDef *u = serial->usart;

    if (u == USART1)
        /* FIXME: assuming PCLK2=HCLK */
        u->BRR = SystemCoreClock / serial->speed;
    else
        /* FIXME: assuming PCLK1=HCLK/2 */
        u->BRR = SystemCoreClock / 2 / serial->speed;
}


static void
_serial_write(serial_t *serial, const char *value, uint16_t size)
{
    serial->usart->SR = ~USART_SR_TC;
    if (serial->tx_dma) {
        dma_disable(serial->tx_dma);
        serial->tx_dma->ch->CCR = 0
                                  | DMA_CCR1_DIR
                                  | DMA_CCR1_MINC
                                  | DMA_CCR1_TEIE
                                  | DMA_CCR1_TCIE
        ;
        serial->tx_dma->ch->CMAR = (uint32_t)value;
        serial->tx_dma->ch->CNDTR = size;
        dma_enable(serial->tx_dma);
        xSemaphoreTake(serial->tcie_sem, portMAX_DELAY);
    } else {
        while (size--) {
            _serial_putc(serial, value, portMAX_DELAY);
            value++;
        }
    }
}


void
serial_puts(serial_t *serial, const char *value)
{
    xSemaphoreTake(serial->mutex, portMAX_DELAY);
    _serial_write(serial, value, strlen(value));
    xSemaphoreGive(serial->mutex);
}


void
serial_write(serial_t *serial, const char *value, uint16_t size)
{
    xSemaphoreTake(serial->mutex, portMAX_DELAY);
    _serial_write(serial, value, size);
    xSemaphoreGive(serial->mutex);
}


#if USE_SERIAL_PRINTF
void
serial_printf(serial_t *serial, const char *fmt, ...)
{
    unsigned len;
    static char fmt_buf[64];
    va_list ap;

    va_start(ap, fmt);
    xSemaphoreTake(serial->mutex, portMAX_DELAY);
    len = vsnprintf(fmt_buf, sizeof(fmt_buf), fmt, ap);
    _serial_write(serial, fmt_buf, MIN(len, sizeof(fmt_buf)));
    va_end(ap);
    xSemaphoreGive(serial->mutex);
}
#endif


void
serial_drain(serial_t *serial)
{
    xSemaphoreTake(serial->mutex, portMAX_DELAY);
    while (!(serial->usart->SR & USART_SR_TC)) {
    }
    xSemaphoreGive(serial->mutex);
}


int16_t
serial_get(serial_t *serial, TickType_t timeout)
{
    uint8_t val;

    if (xQueueReceive(serial->rx_q, &val, timeout))
        return val;
    else
        return -ETIMEDOUT;
}


static inline void
usart_irq(serial_t *serial)
{
    USART_TypeDef *u = serial->usart;
    uint16_t sr, dr;
    uint8_t val;
    BaseType_t wakeup = pdFALSE;

    sr = u->SR;
    dr = u->DR;

    if ((sr & USART_SR_RXNE) && serial->rx_q) {
        val = (uint8_t)dr;
        xQueueSendToBackFromISR(serial->rx_q, &val, &wakeup);
    }

    if (sr & USART_SR_TXE) {
        if (serial->tx_q && xQueueReceiveFromISR(serial->tx_q, &val, &wakeup))
            u->DR = val;
        else
            u->CR1 &= ~USART_CR1_TXEIE;
    }

    portEND_SWITCHING_ISR(wakeup);
}


static inline void
usart_tcie(void *param, uint32_t flags)
{
    serial_t *serial = (serial_t *)param;
    BaseType_t wakeup = pdFALSE;

    dma_disable(serial->tx_dma);
    xSemaphoreGiveFromISR(serial->tcie_sem, &wakeup);
    portEND_SWITCHING_ISR(wakeup);
}


#if USE_SERIAL_USART1
void
USART1_IRQHandler(void)
{
    usart_irq(&Serial1);
}
#endif

#if USE_SERIAL_USART2
void
USART2_IRQHandler(void)
{
    usart_irq(&Serial2);
}
#endif

#if USE_SERIAL_USART3
void
USART3_IRQHandler(void)
{
    usart_irq(&Serial3);
}
#endif

#if USE_SERIAL_UART4
void
UART4_IRQHandler(void)
{
    usart_irq(&Serial4);
}
#endif

#if USE_SERIAL_UART5
void
UART5_IRQHandler(void)
{
    usart_irq(&Serial5);
}
#endif
