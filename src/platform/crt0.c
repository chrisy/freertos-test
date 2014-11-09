/** MPU bootstrap routine
 * \file src/platform/crt0.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <version.h>
#include "nvic.h"
#include "fsmc.h"
#include "main.h"

// Peripheral headers
#include <stm32/dma.h>
#include <stm32/serial.h>
#include <stm32/i2c.h>
#include <stm32/spi.h>

// Our handlers
void _crt0_init(void) __attribute__ ((naked, noreturn));
void _crt0_nmi_handler(void) __attribute__ ((naked, noreturn));
void _crt0_hardfault_handler(void) __attribute__ ((naked, noreturn));

// Handlers in FreeRTOS (from portable/GCC/ARM_CM3/port.c)
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

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

    .DMA2_Channel1_IRQHandler   = DMA2_Channel1_IRQHandler,
    .DMA2_Channel2_IRQHandler   = DMA2_Channel2_IRQHandler,
    .DMA2_Channel3_IRQHandler   = DMA2_Channel3_IRQHandler,
#ifdef STM32F10X_CL
    .DMA2_Channel4_IRQHandler   = DMA2_Channel4_IRQHandler,
    .DMA2_Channel5_IRQHandler   = DMA2_Channel5_IRQHandler,
#else
    .DMA2_Channel4_5_IRQHandler = DMA2_Channel4_5_IRQHandler,
#endif

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
#if USE_SERIAL_UART4
    .UART4_IRQHandler           = UART4_IRQHandler,
#endif
#if USE_SERIAL_UART5
    .UART5_IRQHandler           = UART5_IRQHandler,
#endif

    // STM32 I2C handlers
#if USE_I2C1
    .I2C1_EV_IRQHandler         = I2C1_EV_IRQHandler,
    .I2C1_ER_IRQHandler         = I2C1_ER_IRQHandler,
#endif
#if USE_I2C2
    .I2C2_EV_IRQHandler         = I2C2_EV_IRQHandler,
    .I2C2_ER_IRQHandler         = I2C2_ER_IRQHandler,
#endif

    // STM32 SPI handlers
#if USE_SPI1
    .SPI1_IRQHandler            = SPI1_IRQHandler,
#endif
#if USE_SPI2
    .SPI2_IRQHandler            = SPI2_IRQHandler,
#endif
#if USE_SPI3
    .SPI3_IRQHandler            = SPI3_IRQHandler,
#endif
};

/* Some static text info for the binary image */
static char _crt0_info[] SECTION_INFO =
    PROJECT_NAME " v" PROJECT_VERSION " " PROJECT_COPYRIGHT;


void _crt0_default_handler(void)
{
    return;
}

void _crt0_nmi_handler(void)
{
    dbg("\r\nNMI!\r\n\r\n");
    for (;; ) ;
}

void __attribute__ ((noreturn)) _crt0_hardfault_print(uint32_t *faultStack)
{
    volatile uint32_t r0;
    volatile uint32_t r1;
    volatile uint32_t r2;
    volatile uint32_t r3;
    volatile uint32_t r12;
    volatile uint32_t lr;   /* Link register. */
    volatile uint32_t pc;   /* Program counter. */
    volatile uint32_t psr;  /* Program status register. */

    r0 = faultStack[0];
    r1 = faultStack[1];
    r2 = faultStack[2];
    r3 = faultStack[3];

    r12 = faultStack[4];
    lr = faultStack[5];
    pc = faultStack[6];
    psr = faultStack[7];

    dbg("\r\nHardfault!\r\n");
    dbgf("r0=%08x r1=%08x r2=%08x r3=%08x r12=%08x\r\n", r0, r1, r2, r3, r12);
    dbgf("lr=%08x pc=%08x psr=%08x\r\n\r\n", lr, pc, psr);

    for (;; ) ;
}

void _crt0_hardfault_handler(void)
{
    __asm volatile
    (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word _crt0_hardfault_print       \n"
    );

    for (;; ) ;  // this is just for the compiler to feel we satisfied noreturn
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
    uint32_t *data = &_mm_data_start;
    uint32_t *data_end = &_mm_data_end;
    uint32_t *datai = &_mm_datai_start;

    while (data < data_end) {
        *data = *datai;
        data++;
        datai++;
    }

    // Setup clocks and the like
    SystemInit();

    // Debugging output
    dbg_init();
    dbgf("\r\n%s\r\n", _crt0_info);

    // Launch!
    main();

    // Shouldn't get here!
    HALT();
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
