.section .text # ro code area
.globl _start

_start:
    la sp, _estack
    la t0, irqVec
    csrw mtvec, t0
    nop
    nop
    ecall
    andi t0, t0, 10
    andi t1, t1, 10
    andi t2, t2, 10
loop:
    j loop

irqVec:
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode
    j EnvironmentCallFromMMode




EnvironmentCallFromMMode:
    andi t0, t0, 0
    andi t1, t1, 0
    andi t2, t2, 0
    andi t3, t3, 0
    andi t4, t4, 0
    andi t5, t5, 0
    andi t6, t6, 0
    andi s1, s1, 0
    andi s2, s2, 0
    andi s3, s3, 0
    mret

.section .rodata // ro variaveis