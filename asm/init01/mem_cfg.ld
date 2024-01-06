OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv")

ENTRY( _start )

__start_memory_rom = 0x000;
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
    .text : { 
        . = ALIGN(4);
        *(.text) 
        . = ALIGN(4);
    } > ROM

    .rodata : { 
        . = ALIGN(4);
        *(.rodata)
        . = ALIGN(4);
    } > ROM

    .data (NOLOAD): { 
        . = ALIGN(4);
        *(.data) 
        . = ALIGN(4);
    } > RAM

    .bss (NOLOAD): { 
        . = ALIGN(4);
        *(.bss)
        *(.comment)
        . = ALIGN(4);
    } > RAM 

    _free_ram = .;

   .stack (NOLOAD):
   {
     /* _edata =  ABSOLUTE(.) ; */
      . = ALIGN(4);
      . = . + (__size_memory_ram - SIZEOF(.data) - SIZEOF(.bss));
      . = ALIGN(4);
       _stack_top = .;
   } > RAM
}
