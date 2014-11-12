/** FreeRTOS wrappers and helpers.
 * \file src/platform/freertos.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <FreeRTOS.h>
#include <task.h>

#if configUSE_TICK_HOOK
static uint64_t milliseconds;
#endif

/**
 * Used by the RTOS when no task can be scheduled.
 */
void vApplicationIdleHook(void)
{
    __WFI();
}

#if configUSE_TICK_HOOK
/**
 * Called by the RTOS after every tick interrupt.
 * This is in the context of an interrupt, so any code here
 * must execute quickly.
 * This implementation maintains a crude millisecond counter.
 */
void vApplicationTickHook(void)
{
    milliseconds += 1000 / configTICK_RATE_HZ;
}
#endif

/**
 * Called if the RTOS detects a stack overflow in a task.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    HALT();
}

#if configUSE_TICK_HOOK
/**
 * Fetch the current millisecond counter.
 * This counter is quite crude having only the granulatity of 'ticks'.
 *
 * @returns Total number of millisconds the RTOS scheduler has been
 * running for.
 */
uint64_t milliseconds_get(void)
{
    uint64_t ret;

    taskENTER_CRITICAL();
    ret = milliseconds;
    taskEXIT_CRITICAL();
    return ret;
}
#endif

#if configGENERATE_RUN_TIME_STATS
/**
 * Used by the RTOS to initialize a timer to track task CPU time.
 */
void platform_timer_init(void)
{
    TIM_TypeDef *tim = TIM14;

    taskENTER_CRITICAL();
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    taskEXIT_CRITICAL();

    tim->PSC = 60000; // prescaler - gives us 600Hz
    tim->CR1 = TIM_CR1_CEN;
    tim->EGR |= TIM_EGR_UG;
}
#endif

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
