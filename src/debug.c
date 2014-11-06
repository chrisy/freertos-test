/** Debugging helpers
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/debug.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <FreeRTOS.h>
#include <posixio.h>
#include <stdio.h>
#include <stdarg.h>

#include "stdio_init.h"

#ifdef DEBUG

void dbg_init(void)
{
    if (stdio_started) return;

    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;

    // PA9 = output
    GPIOA->CRH = 0x000004B0;

    USART_TypeDef *usart = USART1;
    usart->CR1 = USART_CR1_UE;
    usart->BRR = 72000000 / 9600;
    usart->CR3 = 0;
    usart->CR2 = 0;
    usart->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void dbg(const char *msg)
{
    const char *p = msg;

    if (stdio_started) {
        fputs(msg, stdout);
        return;
    }

    while (*p) {
        // Send byte
        USART1->DR = ((uint16_t)*(p++)) & 0x00ff;
        // Wait for it to have been sent
        while (!(USART1->SR & USART_SR_TXE)) ;
    }
}

void dbgf(const char *fmt, ...)
{
    static char buf[128];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(buf, 128, fmt, ap);
    va_end(ap);
    dbg(buf);
}

#else /* !DEBUG */

void dbg_init(void)
{
}

void dbg(const char *msg)
{
}

void dbgf(const char *fmt, ...)
{
}

#endif /* DEBUG */
