/** LED driver.
 * \file src/led.c
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
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include "led.h"


/**
 * Simple timer task to cycle the LEDs on the evaluation board.
 */
static void led_advance(TimerHandle_t timer)
{
    static uint8_t i = 0;

    uint16_t b = (1 << i) << 6;

    GPIOF->BSRR = b;
    GPIOF->BRR = (0xf << 6) & ~b;

    i++;
    if (i > 3) i = 0;
}

/** A timer handle for the LED driver. */
static TimerHandle_t led_timer;

/**
 * Initializes the GPIO pins for LED output and starts a task
 * to display patterns on them.
 */
void led_init(void)
{
    taskENTER_CRITICAL();

    RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;

    GPIOF->CRL &= ~(
        GPIO_CRL_MODE6 | GPIO_CRL_CNF6 |
        GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOF->CRH &= ~(
        GPIO_CRH_MODE8 | GPIO_CRH_CNF8 |
        GPIO_CRH_MODE9 | GPIO_CRH_CNF9);

    GPIOF->CRL |= GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1;
    GPIOF->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;
    GPIOF->CRH |= GPIO_CRH_MODE8_0 | GPIO_CRH_MODE8_1;
    GPIOF->CRH |= GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1;

    taskEXIT_CRITICAL();

    GPIOF->BSRR = 0xffff;

    led_timer = xTimerCreate("led", 100 / portTICK_PERIOD_MS,
                             pdTRUE, NULL, led_advance);
    xTimerStart(led_timer, 0);
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
