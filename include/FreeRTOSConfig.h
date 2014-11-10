/**
 * \file include/FreeRTOSConfig.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <config.h>

#define configUSE_IDLE_HOOK             1
#define configUSE_QUEUE_SETS            0
#define configUSE_TICK_HOOK             1
#define configCHECK_FOR_STACK_OVERFLOW  2

#define configUSE_16_BIT_TICKS          0
#define configUSE_ALTERNATIVE_API       0
#define configUSE_CO_ROUTINES           0
#define configUSE_RECURSIVE_MUTEXES     1
#define configUSE_TIME_SLICING          1
#define configUSE_TIMERS                1
#define configUSE_TRACE_FACILITY        1
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

#define configUSE_COUNTING_SEMAPHORES   1
#define configUSE_MUTEXES               1
#define configUSE_PREEMPTION            1
#define configUSE_NEWLIB_REENTRANT      1

#define configASSERT(x)                 ASSERT(x)
#define configCPU_CLOCK_HZ              (SystemCoreClock)
#define configTICK_RATE_HZ              ((TickType_t)100)
#define configMAX_PRIORITIES            5
#define configMINIMAL_STACK_SIZE        128
#define configMAX_TASK_NAME_LEN         16
#define configIDLE_SHOULD_YIELD         1
#define configQUEUE_REGISTRY_SIZE       0
#define configGENERATE_RUN_TIME_STATS   1
#define configTIMER_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
#define configTIMER_QUEUE_LENGTH        2
#define configTIMER_TASK_STACK_DEPTH    128
#define configMAX_CO_ROUTINE_PRIORITIES 1
#define configTASK_RETURN_ADDRESS       NULL


// Needed if we're using heap_4.c
//#define configTOTAL_HEAP_SIZE           32768

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_xTimerGetTimerDaemonTaskHandle  0
#define INCLUDE_pcTaskGetTaskName               0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_vTaskCleanUpResources           1

/* Use lowest possible interrupt priority for the scheduler */
#define configKERNEL_INTERRUPT_PRIORITY 0xff

/* Interrupts with higher priority than this (lower number) will preempt the
 * scheduler, but cannot use any RTOS functions. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 0x10

/* This is the value being used as per the ST library which permits 16
 * priority values, 0 to 15.  This must correspond to the
 * configKERNEL_INTERRUPT_PRIORITY setting.  Here 15 corresponds to the lowest
 * NVIC value of 255. */
#define configLIBRARY_KERNEL_INTERRUPT_PRIORITY 15

// timer stuff
#if configGENERATE_RUN_TIME_STATS
void platform_timer_init(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() platform_timer_init()
#define portGET_RUN_TIME_COUNTER_VALUE() (TIM14->CNT)
#endif

// Useful macros
#define DISABLE_IRQ         portENTER_CRITICAL
#define ENABLE_IRQ          portEXIT_CRITICAL

#define MS2ST(ms)           (((ms) * configTICK_RATE_HZ) / 1000)
#define S2ST(ms)            ((ms) * configTICK_RATE_HZ)
#define DELAY_MS(ms)        vTaskDelay(MS2ST(ms))
#define DELAY_S(ms)         vTaskDelay(S2ST(ms))

#if configUSE_TICK_HOOK
uint64_t milliseconds_get(void);
#endif

#endif /* FREERTOS_CONFIG_H */

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
