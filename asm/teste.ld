OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv")

ENTRY( _start )

__start_memory_rom = 0x0;
__size_memory_rom = 0x200;
__start_memory_ram = 0x200;
__size_memory_ram = 0x200;
 
MEMORY
{
  ROM (rx)  : ORIGIN = __start_memory_rom, LENGTH = __size_memory_rom
  RAM (!rx) : ORIGIN = __start_memory_ram, LENGTH = __size_memory_ram
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
        *(.comment)
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
      . = . + (__size_memory_ram - SIZEOF(.data) - SIZEOF(.bss) - 0x32);
      . = ALIGN(4);
       _estack = .;
   } > RAM

    /* .symtab : { *(.symtab)} 
    .shstrtab : { *(.shstrtab)}
    .strtab : { *(.strtab)}
    .symtab : { *(.symtab)} */

    _end = .;
}
