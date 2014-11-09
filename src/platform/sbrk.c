/** sbrk allocator with bounds checking
 * \file src/platform/sbrk.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdint.h>
#include <errno.h>

extern uint32_t _mm_heap_start;
extern uint32_t _mm_heap_end;
char *heap_top = (char *)&_mm_heap_start;

/* We assume we're being called in a critical section */
char *_sbrk(intptr_t increment)
{
    char *ret;

    portENTER_CRITICAL();

    if (heap_top + increment + SYSTEM_STACK_SIZE > (char *)&_mm_heap_end) {
        portEXIT_CRITICAL();
        errno = ENOMEM;
        return (char *)-1;
    }
    ret = heap_top;
    heap_top += increment;

    portEXIT_CRITICAL();

    return ret;
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
