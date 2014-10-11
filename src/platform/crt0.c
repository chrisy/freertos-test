#include <config.h>
#include <FreeRTOS.h>
#include <nvic.h>
#include "main.h"

void _crt0_init(void);
void _crt0_nmi_handler(void);
void _crt0_hardfault_handler(void);

extern unsigned int _STACKTOP;

// Define the vector table
struct nvic _nvic_vector __attribute__ ((section(".nvic_vector"))) = {
	.stack_top		= (unsigned int *)&_STACKTOP,
	.Reset_Handler		= _crt0_init,
	.NMI_Handler		= _crt0_nmi_handler,
	.HardFault_Handler	= _crt0_hardfault_handler
};

/* Some static text info for the binary image */
char _crt0_info[] __attribute__ ((section(".info"))) = "FreeRTOS-test...";

// These are defined by the linker script
extern unsigned int _BSS_BEGIN;
extern unsigned int _BSS_END;
extern unsigned int _DATA_BEGIN;
extern unsigned int _DATA_END;
extern unsigned int _DATAI_BEGIN;
extern unsigned int _DATAI_END;

void _crt0_default_handler(void)
{
	return;
}

void _crt0_nmi_handler(void)
{
	HALT();
}

void _crt0_hardfault_handler(void)
{
	HALT();
}

// Bootstrap routine
void _crt0_init(void)
{
	unsigned int *bss_begin = &_BSS_BEGIN;
	unsigned int *bss_end = &_BSS_END;

	while (bss_begin < bss_end) {
		*bss_begin = 0;
		bss_begin++;
	}

	unsigned int *data_begin = &_DATA_BEGIN;
	unsigned int *data_end = &_DATA_END;
	unsigned int *datai_begin = &_DATAI_BEGIN;
	unsigned int *datai_end = &_DATAI_END;

	unsigned int data_size = data_end - data_begin;
	unsigned int datai_size = datai_end - datai_begin;

	if (data_size != datai_size)
		// This should not happen!
		while (1) ;

	while (data_begin < data_end) {
		*data_begin = *datai_begin;
		data_begin++;
		datai_begin++;
	}

	// Launch!
	SystemInit();
	main();

	// Shouldn't get here!
	HALT();
}
