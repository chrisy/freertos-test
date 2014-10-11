/**
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file include/config.h
 * Config
 */


#ifndef CONFIG_H
#define CONFIG_H

#ifdef _DEBUG
# define DEBUG 1
#else
# define DEBUG 0
#endif

#ifdef HAVE_CONFIG_H
#include "ac_config.h"
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#include "cc_config.h"

#define CPU_FREQ 72000000
#define STM32F10X_HD

#define USE_I2C1                1
#define USE_I2C2                0
#define USE_SERIAL_USART1       1
#define USE_SERIAL_USART2       0
#define USE_SERIAL_USART2       0
#define USE_SERIAL_UART4        1
#define USE_SERIAL_UART5        1
#define USE_SPI1                0
#define USE_SPI3                0

/* Highest priority (highest number) */
#define THREAD_PRIO_VTIMER      4
#define THREAD_PRIO_MAIN        3
//#define THREAD_PRIO_TCPIP       2
//#define THREAD_PRIO_NTPCLIENT   1
/* Lowest priority (lowest number) */

/* Highest priority (lowest number) */
//#define IRQ_PRIO_ETH            4
#define IRQ_PRIO_SYSTICK        8
#define IRQ_PRIO_I2C            12
#define IRQ_PRIO_SPI            12
#define IRQ_PRIO_USART          12
/* Lowest priority (highest number) */

#define MSP_STACK_SIZE          512
#define MAIN_STACK_SIZE         512
//#define NTPCLIENT_STACK_SIZE    512
//#define TCPIP_STACK_SIZE        512
#define VTIMER_STACK_SIZE       512



#define DISABLE_IRQ         portENTER_CRITICAL
#define ENABLE_IRQ          portEXIT_CRITICAL

//#define GPIO_ON(pfx)        _PASTE2(pfx, _PAD)->BSRR = _PASTE2(pfx, _PIN);
//#define GPIO_OFF(pfx)       _PASTE2(pfx, _PAD)->BRR  = _PASTE2(pfx, _PIN);
//#define MS2ST(ms)           (((ms) * configTICK_RATE_HZ) / 1000)
//#define S2ST(ms)            ((ms) * configTICK_RATE_HZ)
#define DELAY_MS(ms)        vTaskDelay(MS2ST(ms))
#define DELAY_S(ms)         vTaskDelay(S2ST(ms))

#include <stm32f10x.h>
#include <system_stm32f10x.h>
#include <core_cm3.h>
#include <FreeRTOS.h>

#endif /* CONFIG_H */
