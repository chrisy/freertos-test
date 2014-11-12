/** sbrk allocator with bounds checking.
 * \file src/platform/sbrk.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdint.h>
#include <real_errno.h>

/** Linker hint at the start of the system heap. */
extern uint32_t _mm_heap_start;
/** Linker hint at the end of the system heap. */
extern uint32_t _mm_heap_end;

/** The high-water mark of all heap space currently allocated. */
char *heap_top = (char *)&_mm_heap_start;

/**
 * Implementation of the sbrk call to allocate a fresh section of the
 * system heap. This implementation has some simple bounds checking
 * based on hints provided by the linker.
 *
 * @param increment The amount of space on the heap to allocate.
 * @returns Pointer to the start of the new block when successful;
 * -1 otherwise and errno will be set to an error code.
 */
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
