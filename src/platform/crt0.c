/** MPU bootstrap routine
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/platform/crt0.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <nvic.h>
#include "main.h"

#include <dma.h>
#include <serial.h>

// Our handlers
void _crt0_init(void) __attribute__ ((noreturn));
void _crt0_nmi_handler(void) __attribute__ ((interrupt));
void _crt0_hardfault_handler(void) __attribute__ ((interrupt));

// Handlers in FreeRTOS (from portable/GCC/ARM_CM3/port.c)
void vPortSVCHandler(void) __attribute__ ((naked));
void xPortPendSVHandler(void) __attribute__ ((naked));
void xPortSysTickHandler(void);

// These are defined by the linker script
extern uint32_t _mm_data_start, _mm_data_end;
extern uint32_t _mm_datai_start, _mm_datai_end;
extern uint32_t _mm_bss_start, _mm_bss_end;
extern uint32_t _mm_heap_start, _mm_heap_end;
extern uint32_t _mm_stack_top;

// Define the vector table
struct nvic _nvic_vector __attribute__ ((section(".nvic_vector"))) = {
    .stack_top                  = &_mm_stack_top,

    // Primary handlers
    .Reset_Handler              = _crt0_init,
    .NMI_Handler                = _crt0_nmi_handler,
    .HardFault_Handler          = _crt0_hardfault_handler,

    // FreeRTOS handlers
    .SVC_Handler                = vPortSVCHandler,
    .PendSV_Handler             = xPortPendSVHandler,
    .SysTick_Handler            = xPortSysTickHandler,

    // STM32 DMA handlers
    .DMA1_Channel1_IRQHandler   = DMA1_Channel1_IRQHandler,
    .DMA1_Channel2_IRQHandler   = DMA1_Channel2_IRQHandler,
    .DMA1_Channel3_IRQHandler   = DMA1_Channel3_IRQHandler,
    .DMA1_Channel4_IRQHandler   = DMA1_Channel4_IRQHandler,
    .DMA1_Channel5_IRQHandler   = DMA1_Channel5_IRQHandler,
    .DMA1_Channel6_IRQHandler   = DMA1_Channel6_IRQHandler,
    .DMA1_Channel7_IRQHandler   = DMA1_Channel7_IRQHandler,

    .DMA2_Channel1_IRQHandler   = DMA1_Channel1_IRQHandler,
    .DMA2_Channel2_IRQHandler   = DMA1_Channel1_IRQHandler,
    .DMA2_Channel3_IRQHandler   = DMA1_Channel1_IRQHandler,
    .DMA2_Channel4_5_IRQHandler = DMA1_Channel1_IRQHandler,

    // STM32 Serial peripheral handlers
#if USE_SERIAL_USART1
    .USART1_IRQHandler          = USART1_IRQHandler,
#endif
#if USE_SERIAL_USART2
    .USART2_IRQHandler          = USART2_IRQHandler,
#endif
#if USE_SERIAL_USART3
    .USART3_IRQHandler          = USART3_IRQHandler,
#endif
};

/* Some static text info for the binary image */
char _crt0_info[] __attribute__ ((section(".info"))) = "FreeRTOS-test...";


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
    /* Initialize the BSS */
    uint32_t *bss_start = &_mm_bss_start;
    uint32_t *bss_end = &_mm_bss_end;

    while (bss_start < bss_end) {
        *bss_start = 0;
        bss_start++;
    }

    /* Copy the initialized data section to RAM */
    uint32_t *data_start = &_mm_data_start;
    uint32_t *data_end = &_mm_data_end;
    uint32_t *datai_start = &_mm_datai_start;
    uint32_t *datai_end = &_mm_datai_end;

    uint32_t data_size = data_end - data_start;
    uint32_t datai_size = datai_end - datai_start;

    if (data_size != datai_size)
        // This should not happen!
        while (1) ;

    while (data_start < data_end) {
        *data_start = *datai_start;
        data_start++;
        datai_start++;
    }

    // Launch!
    SystemInit();
    main();

    // Shouldn't get here!
    HALT();
}
