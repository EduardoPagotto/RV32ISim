


.section .text # Code area
.globl _start # entry point 

#  Default values        
.equ CSR_MSTATUS, 0x00000008
.equ CSR_MISA, 0x40000100
.equ CSR_MIE, 0x00000888
//.equ CSR_MTVEC, init_interrupt_vect//0x10000004 | 1

_start:
    # initialize sp
    la sp,_stack_top
    # load default vals into CSR
    li t0, CSR_MSTATUS
    csrrw zero, mstatus, t0
    li t0, CSR_MISA
    csrrw zero, misa, t0
    li t0, CSR_MIE
    csrrw zero, mie, t0
    la t0, init_interrupt_vect
    csrrw zero, mtvec, t0
    nop
    addi t0, zero, 0
    ecall
    nop
    wfi

init_interrupt_vect:
    j InstructionAddressMisaligned
    j InstructionAccessFault
    j IllegalInstruction
    j Breakpoint
    j LoadAddressMisaligned
    j LoadAccessFault
    j StoreAMOAddressMisaligned
    j StoreAMOAccessFault
    j EnvironmentCallFromUMode
    j EnvironmentCallFromSMode
    j Reserved3
    j EnvironmentCallFromMMode
    j InstructionPageFault
    j LoadPageFault
    j Reserved4
    j StoreAMOPageFault

InstructionAddressMisaligned:
    mret
InstructionAccessFault:
    mret
IllegalInstruction:
    mret
Breakpoint:
    mret
LoadAddressMisaligned:
    mret
LoadAccessFault:
    mret
StoreAMOAddressMisaligned:
    mret
StoreAMOAccessFault:
    mret
EnvironmentCallFromUMode:
    mret
EnvironmentCallFromSMode:
    mret
Reserved3:
    mret
EnvironmentCallFromMMode:
    mret
InstructionPageFault:
    mret
LoadPageFault:
    mret
Reserved4:
    mret
StoreAMOPageFault:
    mret

// Global Vals
.section .rodata // Constants
msg_erroc: .string "erro fatal\n"

.section .data   // rw inicializadas com valor
data01: .4byte 0x00112233

.section .bss
value1: .word 0x0

