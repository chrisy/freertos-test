/** Program start, platform initialization.
 * \file src/main.c
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
#include <posixio/posixio.h>

// Peripheral headers
#include <stm32/dma.h>
#include <stm32/serial.h>
#include <stm32/i2c.h>
#include <stm32/spi.h>


#include "main.h"
#include "stdio_init.h"
#include "led.h"
#include "fonts.h"
#include "lcd.h"


static void main_task(void *param);
static void platform_init(void);

#if configUSE_QUEUE_SETS
/** Queue set for serial drivers */
QueueSetHandle_t qs_serial;
#endif

/**
 * Create the initial tasks and start the RTOS scheduler.
 */
int main(void)
{
    dbg("Platform starting up." EOL);

    // add main thread
    xTaskCreate(main_task, "main",
                STACK_SIZE_MAIN, NULL,
                THREAD_PRIO_MAIN, NULL);

    // start it up
    dbg("Launching RTOS scheduler." EOL);
    vTaskStartScheduler();

    return 0;
}



/**
 * The "main" task responsible for finishing hardware initialization
 * and starting the other tasks of the system.
 *
 * @param param 'param' is unused.
 */
void NORETURN main_task(void *param)
{
    dbg("Scheduler launched." EOL);

    // Bootstrap the CLI
    cli_init();

    // setup the peripherals
    platform_init();

    // Onboard display setup
    font_init();
    lcd_init();

    // announce life!
    printf("This platform is running!" EOL);
    fflush(stdout);

    // Start the console CLI
    cli_start("serial", NULL, NULL);

    for (;; )
        DELAY_MS(500);
}

/**
 * Initialize the platform, including any hardware related to the platform.
 * The platform includes basic I/O hardware and the POSIX-like subsystem.
 */
static void platform_init(void)
{
    // Bootstrap the POSIX IO platform
    posixio_start();

    // Initialize USARTs
#if configUSE_QUEUE_SETS
    qs_serial = xQueueCreateSet(16);
    ASSERT(qs_serial != NULL);
#endif
#if USE_SERIAL_USART1
    serial_start(&Serial1, DEFAULT_USART_BAUD
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
    serial_puts(&Serial1, "STM32 Platform starting up." EOL);
    stdio_start();
    printf("STDIO started on USART 1." EOL);
    fflush(stdout);
#endif

#if USE_SERIAL_USART2
    printf("Starting USART 2." EOL);
    serial_start(&Serial2, DEFAULT_USART_BAUD
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

#if USE_SERIAL_USART3
    printf("Starting USART 3." EOL);
    serial_start(&Serial3, DEFAULT_USART_BAUD
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

#if USE_SERIAL_UART4
    printf("Starting USART 4." EOL);
    serial_start(&Serial4, DEFAULT_USART_BAUD
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

#if USE_SERIAL_UART5
    printf("Starting USART 5." EOL);
    serial_start(&Serial5, DEFAULT_USART_BAUD
#if configUSE_QUEUE_SETS
                 , qs_serial
#endif
                 );
#endif

    // Initialize i2c
#if USE_I2C1
    printf("Starting I2C 1." EOL);
    i2c_start(&I2C1_Dev);
#endif
#if USE_I2C2
    printf("Starting I2C 2." EOL);
    i2c_start(&I2C2_Dev);
#endif

    printf("Starting LED task." EOL);
    led_init();

    // Initialize SPI
#if USE_SPI1
    printf("Starting SPI 1." EOL);
    spi_start(&SPI1_Dev, 0); // todo - workout the SPI mode bits
#endif
#if USE_SPI2
    printf("Starting SPI 2." EOL);
    spi_start(&SPI2_Dev, 0);
#endif
#if USE_SPI3
    printf("Starting SPI 3." EOL);
    spi_start(&SPI3_Dev, 0);
#endif
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
