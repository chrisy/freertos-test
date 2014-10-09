#include <stdint.h>

#include "main.h"

void low_level_init(void);
void nmi_handler(void);
void hardfault_handler(void);

extern unsigned int _STACKTOP;

#define FNPTR_TO_UINT(fn) ((union {void (*from)(void); unsigned int *to;}) fn).to

// Define the vector table
//unsigned int * the_nvic_vector[4] 
struct {
	unsigned int *stack_top;
	void (*init)(void);
	void (*nmi)(void);
	void (*hardfault)(void);
} the_nvic_vector 
__attribute__ ((section(".nvic_vector")))= {
    (unsigned int *)	&_STACKTOP,        // stack pointer
    low_level_init,    // code entry point
    nmi_handler,       // NMI handler (not really)
    hardfault_handler  // hard fault handler (let's hope not)
};


extern unsigned int _BSS_BEGIN;
extern unsigned int _BSS_END;

extern unsigned int _DATA_BEGIN;
extern unsigned int _DATA_END;
extern unsigned int _DATAI_BEGIN;
extern unsigned int _DATAI_END;

void low_level_init(void)
{
    unsigned int* bss_begin = &_BSS_BEGIN;
    unsigned int* bss_end   = &_BSS_END;
    while(bss_begin < bss_end)
    {
        *bss_begin = 0;
        bss_begin++;
    }

    unsigned int* data_begin  = &_DATA_BEGIN;
    unsigned int* data_end    = &_DATA_END;
    unsigned int* datai_begin = &_DATAI_BEGIN;
    unsigned int* datai_end   = &_DATAI_END;

    unsigned int data_size  = data_end  - data_begin;
    unsigned int datai_size = datai_end - datai_begin;

    if(data_size != datai_size) {
        //Linker script is not correct.
        while(1);
    }

    while(data_begin < data_end)
    {
        *data_begin = *datai_begin;
        data_begin++;
        datai_begin++;
    }


    main();
}


void nmi_handler(void)
{
    return;
}

void hardfault_handler(void)
{
    return;
}
