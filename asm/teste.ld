OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf32-littleriscv")

ENTRY( _start )
SECTIONS
{
    . = 0x0000;                 /* ROM */
    .text : { *(.text) }
    /* .gnu_build_id : { *(.note.gnu_build_id) } */
    .rodata : { * (.rodata) }

    . = 0x0100;                 /* RAM */
    .data : { *(.data) }
    /* .sdata : { *(.sdata) } */
    .bss : { *(.bss) }
    /* .debug : { *(.debug) } */

    . += 0x10;                  /* STACK */
    stack_top = .;

    _end = .;
}
