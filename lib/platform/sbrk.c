/**
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file lib/platform/sbrk.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <errno.h>
#include <stdint.h>

/* sbrk allocator with bounds checking */

extern uint32_t _sheap;
extern uint32_t _eheap;
char *heap_top = (char *)&_sheap;

void *
_sbrk(intptr_t increment)
{
    void *ret;

    if (heap_top + increment > (char *)&_eheap)
        return (char *)-1;
    ret = heap_top;
    heap_top += increment;
    return ret;
}
