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

#include <dma.h>
#include <serial.h>

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
	// Initialize USARTs
	qs_serial = xQueueCreateSet(21);
	serial_start(&Serial1, 9600, qs_serial);
	serial_start(&Serial2, 9600, qs_serial);
	serial_start(&Serial3, 9600, qs_serial);
}
