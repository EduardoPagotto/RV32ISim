.section .text # Code area
.globl _start # entry point 

_start:
    la sp,_stack_top

    # teste de carga
    la t0, data01
    lb t1, 0(t0)
    lh t2, 0(t0)
    lbu t3, 0(t0)
    lhu t4, 0(t0)
    lw t5, 0(t0)
    li t5, 0x01234567
    sw t5, 0(t0)
    ;
    la t0, data02
    lh t1, 0(t0)
    li t1, 0xabcd
    sh t1, 0(t0)
    ;
    la t0, data03
    lb t1, 0(t0)
    li t1, 0x34
    sb t1, 0(t0)    
    #
    # Salva registro no stack
    #
    li t0, 1024   # valor de teste
    addi sp, sp, -4     # Reserva espaco na pilha
    sw t0, 0(sp)        # Salva registro na pilha
    #
    # Teste chamada de subrotina para frente
    #
    jal teste1
    addi t0, zero, 0
    #
    # Recupera registro do Stack
    #
    lw t0, 0(sp)    # Recupera valor
    addi sp, sp, 4  # Desaloca espaco na pilha
    j salto
    #
    # entrada de salto para traz
    #
salto2:
    li   t0, -200
    j salto3
    #
    # subrotina 2 com ra salva automaticamente
    #
sub2:
    lw t3, -4(sp)
    nop
    ret
salto:
    li   t0, 1
    li   t5, -100;
    j salto2
salto3:
     li   t4, 3
    addi t6, t0, -1
    wfi
    #
    # Sub Rotina de loop
    # "for(t0=_free_ram; t0 < (_free_ram + 5); t0++)" 
    #   t2 = mem[t0] ; t2 = 0; mem[t0] = 0;
    # t0 endereco indice base
    # t1 meta
    # t2 dado
teste1: 
    la t0,_free_ram  # carrega inicio da memoria livre
    addi t1, t0, 5  # tamanho do loop
loop1:
    lb t2, 0(t0)     # carga do byte armazenado na memoria 
    mv t2, zero      # zera dado
    sb t2, 0(t0)     # armazena o zero na memoria
    addi t0, t0, 1   # incrementa proximo endereco   
    blt t0, t1, loop1# continua loop se nao terminou
    #
    # Salva endereco de retorno na pilha ante do proximo salto
    #
    addi sp, sp, -4
    sw ra, 0(sp)
    # Chama subrotina que reecreve ra
    jal sub2
    # carrega ra anterior do stack
    lw ra, 0(sp)
    addi sp, sp, 4
    # retora ao chamador
    ret              


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
