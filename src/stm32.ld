MEMORY
{
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
  stack(rwx): ORIGIN = 0x20004FFC, LENGTH = 0K

  rom (rx)  : ORIGIN = 0x00000000, LENGTH = 128K
}

SECTIONS
{
    .nvic_vector : 
    {
        *(vectors)    /* Vector table */
    } >rom

    .text : 
    {
        *(.text)      /* Program code */
        *(.text.*)
        *(.rodata)    /* Read only data */
        *(.rodata.*)        
        _DATAI_BEGIN = .;
    } >rom

    .data : 
    {
        _DATA_BEGIN = .;
        *(.data)      /* Data memory */
        *(.data.*)        
        _DATA_END = .;
    } >ram AT >rom

    .data_init : 
    {
        _DATAI_END = .;
    } >rom

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
