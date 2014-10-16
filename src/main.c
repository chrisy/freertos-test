/** Program start
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/main.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <FreeRTOS.h>
#include <task.h>
#include <posixio.h>
#include <dma.h>
#include <serial.h>
#include <i2c.h>
#include <spi.h>

#include "main.h"

static void platform_init(void);

QueueSetHandle_t qs_serial = NULL;


int main(void)
{
    // setup the hardware
    platform_init();

    // add main thread

    // start it up
    vTaskStartScheduler();
}

static void platform_init(void)
{
    // Bootstrap the POSIX IO platform
    posixio_start();

    // Initialize USARTs
    qs_serial = xQueueCreateSet(16);
    ASSERT(qs_serial != NULL);
#if USE_SERIAL_USART1
    serial_start(&Serial1, 9600, qs_serial);
#endif
#if USE_SERIAL_USART2
    serial_start(&Serial2, 9600, qs_serial);
#endif
#if USE_SERIAL_USART3
    serial_start(&Serial3, 9600, qs_serial);
#endif
#if USE_SERIAL_UART4
    serial_start(&Serial4, 9600, qs_serial);
#endif

    // Initialize i2c
#if USE_I2C1
    i2c_start(&I2C1_Dev);
#endif
#if USE_I2C2
    i2c_start(&I2C2_Dev);
#endif

    // Initialize SPI
#if USE_SPI1
    spi_start(&SPI1_Dev, 0); // todo - workout the SPI mode bits
#endif
#if USE_SPI2
    spi_start(&SPI2_Dev, 0);
#endif
#if USE_SPI3
    spi_start(&SPI3_Dev, 0);
#endif
}
