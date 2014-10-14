/**
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file include/config.h
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
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

// The heap size check in sbrk will offset its ceiling by this much
#define SYSTEM_STACK_SIZE       512

#define STM32F10X_HD

#define USE_I2C1                1
#define USE_I2C2                1
#define USE_SERIAL_USART1       1
#define USE_SERIAL_USART2       0
#define USE_SERIAL_USART3       0
#define USE_SERIAL_UART4        0
#define USE_SERIAL_UART5        0
#define USE_SPI1                1
#define USE_SPI2                1
#define USE_SPI3                1

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


#include <stm32f10x.h>
#include <system_stm32f10x.h>
#include <core_cm3.h>
#include <FreeRTOS.h>

#endif /* CONFIG_H */
