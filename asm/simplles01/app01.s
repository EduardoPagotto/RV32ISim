.section .text # Code area
.globl _start # entry point 

_start:
    nop
    addi t0, zero, 0
    addi t1, zero, 1
    addi t2, zero, 2
    addi t3, zero, 3
    li   t4, 4
    li   t5, -100;
    wfi

// Global Vals
.section .rodata // Constants
.section .data   // rw inicializadas com valor
.section .bss    // rw (zeradas) [.bss symbol, lenght, align]

_end: