OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv")

ENTRY( _start )

MEMORY
{
  ROM (rx)  : ORIGIN = 0x0, LENGTH = 0x100
  RAM (!rx) : ORIGIN = 0x100, LENGTH = 0x100
}


SECTIONS
{
    /* . = 0x0000;                  */
    .text : 
    { 
        *(.text) 
        . = ALIGN(4);
    } > ROM
    /* .gnu_build_id : { *(.note.gnu_build_id) } */
    .rodata : 
    { 
        . = ALIGN(4);
        * (.rodata)
        . = ALIGN(4);
    } > ROM

    /* . = 0x0100;                  */

    .data : 
    { 
        . = ALIGN(4);
        *(.data) 
        . = ALIGN(4);
    } > RAM

    .bss : 
    { 
        . = ALIGN(4);
        *(.bss) 
        . = ALIGN(4);
    } > RAM

    /* stack_top = ADDR(.text) + SIZEOF(RAM) + SIZEOF(ROM) - 4;  */
    /* stack_top = 0x1ff; */
    /* . += 0x40;               */
    /* stack_top = .; */
    /* stack_top = max_mem; */

   .stack (NOLOAD):
   {
      . = ALIGN(4);
      _sstack = .;
      . = . + 0x40;
      . = ALIGN(4);
       _estack = .;
   } > RAM


    _end = .;
}
