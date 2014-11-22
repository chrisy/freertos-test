/** Linker script for STM32F103ZG
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 * \file src/platform/STM32F1032G.ld
 *
 * \author Chris Luke <chrisy@flirble.org>
 * \copyright Copyright (c) Chris Luke <chrisy@flirble.org>
 *
 * \copyright This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

MEMORY
{
    flash (rx)        : ORIGIN = 0x08000000, LENGTH = 1M
    ram (rwx)         : ORIGIN = 0x20000000, LENGTH = 96K
    stack (rw)        : ORIGIN = 0x20010000, LENGTH = 0K
    fsmc_bank1 (rw)   : ORIGIN = 0x60000000, LENGTH = 256M /* 256MB max */
    fsmc_bank1_1 (rw) : ORIGIN = 0x60000000, LENGTH = 64M  /* 64MB max */
    fsmc_bank1_2 (rw) : ORIGIN = 0x64000000, LENGTH = 64M  /* 64MB max */
    fsmc_bank1_3 (rw) : ORIGIN = 0x68000000, LENGTH = 1M   /* 64MB max */
    fsmc_bank1_4 (rw) : ORIGIN = 0x6c000000, LENGTH = 64M  /* 64MB max */
    fsmc_bank2 (rw)   : ORIGIN = 0x70000000, LENGTH = 256M /* 256MB max */
    fsmc_bank3 (rw)   : ORIGIN = 0x80000000, LENGTH = 256M /* 256MB max */
    fsmc_bank4 (rw)   : ORIGIN = 0x90000000, LENGTH = 256M /* 256MB max */
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
    } >flash

    .data :
    {
        . = ALIGN(4);
        _mm_data_start = .;
        *(.data)    /* Data memory */
        *(.data.*)
        . = ALIGN(4);
        _mm_data_end = .;
    } >ram AT >flash
    _mm_datai_start = LOADADDR(.data);

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

    .fsmc_bank1 :
    {
        _fsmc_bank1_start = .;
        *(.fsmc_bank1)
        *(.fsmc_bank1.*)
        _fsmc_bank1_end = .;
    } >fsmc_bank1

    .fsmc_bank1_1 :
    {
        _fsmc_bank1_1_start = .;
        *(.fsmc_bank1_1)
        *(.fsmc_bank1_1.*)
        _fsmc_bank1_1_end = .;
    } >fsmc_bank1_1

    .fsmc_bank1_2 :
    {
        _fsmc_bank1_2_start = .;
        *(.fsmc_bank1_2)
        *(.fsmc_bank1_2.*)
        _fsmc_bank1_2_end = .;
    } >fsmc_bank1_2

    .fsmc_bank1_3 :
    {
        _fsmc_bank1_3_start = .;
        *(.fsmc_bank1_3)
        *(.fsmc_bank1_3.*)
        _fsmc_bank1_3_end = .;
    } >fsmc_bank1_3

    .fsmc_bank1_4 :
    {
        _fsmc_bank1_4_start = .;
        *(.fsmc_bank1_4)
        *(.fsmc_bank1_4.*)
        _fsmc_bank1_4_end = .;
    } >fsmc_bank1_4

    .fsmc_bank2 :
    {
        _fsmc_bank2_start = .;
        *(.fsmc_bank2)
        *(.fsmc_bank2.*)
        _fsmc_bank2_end = .;
    } >fsmc_bank2

    .fsmc_bank3 :
    {
        _fsmc_bank3_start = .;
        *(.fsmc_bank3)
        *(.fsmc_bank3.*)
        _fsmc_bank3_end = .;
    } >fsmc_bank3

    .fsmc_bank4 :
    {
        _fsmc_bank4_start = .;
        *(.fsmc_bank4)
        *(.fsmc_bank4.*)
        _fsmc_bank4_end = .;
    } >fsmc_bank4


    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }

}

