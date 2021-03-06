/** MPU bootstrap routine.
 * \file src/platform/crt0.c
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
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

/** RTOS service handler (from portable/GCC/ARM_CM3/port.c) */
extern void vPortSVCHandler(void);
/** RTOS pending-service handler (from portable/GCC/ARM_CM3/port.c) */
extern void xPortPendSVHandler(void);
/** RTOS system tick handler (from portable/GCC/ARM_CM3/port.c) */
extern void xPortSysTickHandler(void);

/** Linker hint at the start of the data section in RAM */
extern uint32_t _mm_data_start;
/** Linker hint at the end of the data section in RAM */
extern uint32_t _mm_data_end;
/** Linker hint at the start of the initialized data section in ROM
 * from which to initialize the RAM data section. */

extern uint32_t _mm_datai_start;
/** Linker hint at the end of the initialized data section in ROM
 * from which to initialize the RAM data section. */
extern uint32_t _mm_datai_end;

/** Linker hint at the start of the BSS. */
extern uint32_t _mm_bss_start;
/** Linker hint at the end of the BSS. */
extern uint32_t _mm_bss_end;

/** Linker hint at the start of the system heap. */
extern uint32_t _mm_heap_start;
/** Linker hint at the end of the system heap. */
extern uint32_t _mm_heap_end;

/** Linker hint at the top of the system stack. */
extern uint32_t _mm_stack_top;

/** The ARM vector table. */
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

/**
 * Static text info for the binary image, including
 * project name, version and copyright.
 */
static char _crt0_info[] SECTION_INFO("_crt0_info") =
    PROJECT_NAME " v" PROJECT_VERSION " " PROJECT_COPYRIGHT;

/** A no-operation 'default' handler. */
void _crt0_default_handler(void)
{
    return;
}

/** NMI handler. */
void _crt0_nmi_handler(void)
{
    dbg(EOL "NMI!" EOL EOL);

    NVIC_SystemReset();
}

/**
 * Used by the hardfault handler, this function prints register
 * details at the time of a fault to the debugging console.
 */
void NORETURN _crt0_hardfault_print(uint32_t *faultStack)
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

    dbg(EOL "Hardfault!" EOL);
    dbgf("r0=%08x r1=%08x r2=%08x r3=%08x r12=%08x" EOL,
         (unsigned int)r0, (unsigned int)r1,
         (unsigned int)r2, (unsigned int)r3,
         (unsigned int)r12);
    dbgf("lr=%08x pc=%08x psr=%08x" EOL EOL,
         (unsigned int)lr, (unsigned int)pc,
         (unsigned int)psr);

#ifdef DEBUG
    HALT();
#else
    NVIC_SystemReset();
#endif
}

/**
 * Hardfault handler. Invoked by the processor whenever a fault has no
 * other handler defined. Here we place current register values on the
 * stack and call _crt0_hardfault_print() to dump them to the debugging
 * console.
 */
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

    __builtin_unreachable();
}

/**
 * System entry point and bootstrap routing.
 * Responsible for initializing the BSS, copying initial values
 * from flash to RAM, basic initial hardware setup and then calling
 * main().
 */
void _crt0_init(void)
{
    /* Initialize the BSS */
    uint32_t *bss_start = (uint32_t *)&_mm_bss_start;

    while (bss_start < (uint32_t *)&_mm_bss_end) {
        *bss_start = 0;
        bss_start++;
    }

    /* Copy the initialized data section to RAM */
    uint32_t *data = (uint32_t *)&_mm_data_start;
    uint32_t *datai = (uint32_t *)&_mm_datai_start;

    while (data < (uint32_t *)&_mm_data_end) {
        *data = *datai;
        data++;
        datai++;
    }

    // Memory barrier is not strictly necessary here, but safe
    __DMB();

    // Setup clocks and the like
    SystemInit();

    // SystemInit should have enabled our FSMC SRAM, clear that
    data = (uint32_t *)&_fsmc_bank1_3_start;
    while (data < (uint32_t *)&_fsmc_bank1_3_end) {
        *data = 0;
        data++;
    }

    // Not strictly necessary
    __DMB();

    // Debugging output
    dbg_init();
    dbgf(EOL "%s" EOL, _crt0_info);

    // Launch!
    main();

    // Shouldn't get here!
    NVIC_SystemReset();
}

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
