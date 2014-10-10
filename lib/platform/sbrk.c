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
