/* Linker script for STM32
 * vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 */

MEMORY
{
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K
    stack(rwx)  : ORIGIN = 0x20010000, LENGTH = 0K

    flash (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
}

SECTIONS
{
    .nvic_vector :
    {
        *(vectors)    /* Vector table */
    } >flash

    .info :
    {
        *(info)       /* Text info */
    } >flash

    .text :
    {
        *(.text)      /* Program code */
        *(.text.*)
        *(.rodata)    /* Read only data */
        *(.rodata.*)
        _DATAI_BEGIN = .;
    } >flash

    .data :
    {
        _DATA_BEGIN = .;
        *(.data)      /* Data memory */
        *(.data.*)
        _DATA_END = .;
    } >ram AT >flash

    .data_init :
    {
        _DATAI_END = .;
    } >flash

    .bss :
    {
        _BSS_BEGIN = .;
        *(.bss)       /* Zero-filled run time allocate data memory */
        *(COMMON)
        _BSS_END = .;
    } >ram

    .heap :
    {
        _HEAP = .;
    } >ram

    .stack :
    {
        _STACKTOP = .;
    } >stack
}

