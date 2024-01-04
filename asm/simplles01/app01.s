.section .text # Code area
.globl _start # entry point 

_start:
    la sp,_stack_top
    ;
    // Limpa memoria
    // t0 endereco indice base
    // t1 meta
    // t2 dado
    la t0,_free_ram
    addi t1, t0, 10
loop1:
    lb t2, 0(t0)
    mv t2, zero
    sb t2, 0(t0)
    addi t0, t0, 1
    blt t0, t1, loop1
    wfi
    ;
    nop
    nop
    addi t0, zero, 0
    addi t1, zero, 1
    addi t2, zero, 2
    addi t3, zero, 3
    li   t4, 4
    li   t5, -100;
    addi t6, t0, -1
    wfi

// Global Vals
.section .rodata // Constants
msg_erroc: .string "erro fatal\n"
msg_err2c: .string "Falha de pagina\n"

.section .data   // rw inicializadas com valor
data01: .4byte 0x00112233
data02: .2byte 0xfcde
data03: .byte 0x73
data04: .byte 0x73
data05: .byte 0x71

.section .bss
value1: .word 0x0
value2: .word 0x0
//.bss value1, 8, 4
# .section .bss // rw nao inicializadas [.bss symbol, lenght, align]
# value1, 4, 4//.8byte 00,00,00,00,00,00,00,00
# value2, 4, 4//.8byte 00,00,00,00,00,00,00,00
