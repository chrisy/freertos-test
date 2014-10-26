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

// Use the POSIX name for this in FreeRTOS
#ifdef _READ_WRITE_RETURN_TYPE
#undef _READ_WRITE_RETURN_TYPE
#endif
#define _READ_WRITE_RETURN_TYPE ssize_t

#ifdef HAVE_CONFIG_H
#include "ac_config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
#endif
#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif

#include "cc_config.h"

// The heap size check in sbrk will offset its ceiling by this much
#define SYSTEM_STACK_SIZE       0

// This is the STM32 family we're using
#define STM32F10X_XL            1
// We have external RAM
#define DATA_IN_ExtSRAM         1

#define USE_I2C1                1
#define USE_I2C2                1
#define USE_SERIAL_USART1       1
#define USE_SERIAL_USART2       0
#define USE_SERIAL_USART3       0
#define USE_SERIAL_UART4        0
#define USE_SERIAL_UART5        0
#define USE_SPI1                0
#define USE_SPI2                0
#define USE_SPI3                0

/* Highest priority (highest number) */
#define THREAD_PRIO_MAIN        3
#define THREAD_PRIO_CLI         3
/* Lowest priority (lowest number) */

/* Highest priority (lowest number) */
#define IRQ_PRIO_SYSTICK        8
#define IRQ_PRIO_I2C            12
#define IRQ_PRIO_SPI            12
#define IRQ_PRIO_USART          12
/* Lowest priority (highest number) */

#define STACK_SIZE_MAIN         2048
#define STACK_SIZE_CLI          2048


#include <stm32f10x.h>
#include <system_stm32f10x.h>
#include <core_cm3.h>
#include <FreeRTOS.h>
#include <debug.h>

#endif /* CONFIG_H */
