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

#include "main.h"

int main(void)
{
    // setup clocks
    // add main thread
    // start it up
    vTaskStartScheduler();
}
