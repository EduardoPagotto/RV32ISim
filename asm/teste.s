//.equ stack_top, 0x7ff

.section .text # ro code area
.globl _start

_start:
    andi sp, sp ,0
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
    la sp, _estack
    la s1, seque4b #li s1, seque4b
    lw t1, 0(s1) # 
    la s2, value1
    sw zero, 0(s2)
    la s3 , value2
    sw zero, 0(s3)
    ecall 
    jal ra, vvv
halt: j halt
    ;
vvv: nop
    nop
    nop
    nop
    jalr zero, ra


.section .rodata // ro variaveis 
msg_erro: .ascii "erro fatal"
msg_erroc: .string "erro fatal\n"

.section .data // rw variaveis estaticas e globais inicializadas com valor
# array1: .byte 0x11, 0x12, 0x13, 0x14
# seque1b: .2byte 0xccff
seque4b: .4byte 0x00112233
# seque8b: .8byte 0x0011223344556677
helloworld: .ascii "Ola Mundo"

# seqHalf: .half 0xff00
# teste1: .word 0x00112233
# teste2: .dword 0x0011223344556677

buffer: .zero 64 # reserva 256 butes zerados 

.bss // rw variaveis nao inicializadas (zeradas) [.bss symbol, lenght, align]
value1: .4byte 
value2: .8byte
# value2: 32, 4 
# value3: 32, 4

# .comm value4, 8
# .common value_global1, 8 # merge com outros arquivos de codigo ??

_end:



    