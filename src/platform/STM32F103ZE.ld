/** Linker script for STM32F103ZE
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/platform/STM32F1032E.ld
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

MEMORY
{
    flash (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
    stack (rw)  : ORIGIN = 0x20010000, LENGTH = 0K
}

ENTRY(_crt0_init)

SECTIONS
{
    . = 0;

    .nvic_vector :
    {
        *(vectors)  /* Vector table */
    } >flash

    .info :
    {
        *(info)     /* Text info */
    } >flash

    .text :
    {
        *(.text)    /* Program code */
        *(.text.*)
        *(.rodata)  /* Read only data */
        *(.rodata.*)
        _mm_datai_start = .;
    } >flash

    .data :
    {
        . = ALIGN(4);
        _mm_data_start = .;
        *(.data)    /* Data memory */
        *(.data.*)
        _mm_data_end = .;
    } >ram AT >flash

    .data_init :
    {
        . = ALIGN(4);
        _mm_datai_end = .;
    } >flash

    .bss :
    {
        . = ALIGN(4);
        _mm_bss_start = .;
        *(.bss)     /* BSS is zero-filled at runtime */
        *(.bss.*)
        *(COMMON)
        _mm_bss_end = .;
    } >ram

    .heap :
    {
        . = ALIGN(4);
        _mm_heap_start = .;
    } >ram

    .stack :
    {
        _mm_heap_end = .;
        _mm_stack_top = .;
    } >stack
}

