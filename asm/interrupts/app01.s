.section .text # Code area
.globl _start # entry point 

#  Default values        
.equ CSR_MSTATUS, 0x00000008
.equ CSR_MISA, 0x40000100
.equ CSR_MIE, 0x00000888

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
    la t0, exceptions_vector
    csrrw zero, mtvec, t0
    nop
    addi t0, zero, 0
    ecall
    nop
    nop
    ebreak
    nop
    wfi

exceptions_vector:
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
    ;
    j UserSoftwareInterrupt
    j SupervisorSoftwareInterrupt
    j Reserved0
    j MachineSoftwareInterrupt
    j UserTimerInterrupt
    j SupervisorTimerInterrupt
    j Reserved1
    j MachineTimerInterrupt
    j UserExternalInterrupt
    j SupervisorExternalInterrupt
    j Reserved2
    j MachineExternalInterrupt
    #
    # Exceptions
    #
InstructionAddressMisaligned:
    mret
InstructionAccessFault:
    mret
IllegalInstruction:
    mret
Breakpoint:
    nop
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
    nop
    mret
InstructionPageFault:
    mret
LoadPageFault:
    mret
Reserved4:
    mret
StoreAMOPageFault:
    mret
#
# Interruption
#
UserSoftwareInterrupt:
    mret
SupervisorSoftwareInterrupt:
    mret
Reserved0:
    mret
MachineSoftwareInterrupt:
    mret
UserTimerInterrupt:
    mret
SupervisorTimerInterrupt:
    mret
Reserved1:
    mret
MachineTimerInterrupt:
    mret
UserExternalInterrupt:
    mret
SupervisorExternalInterrupt:
    mret
Reserved2:
    mret
MachineExternalInterrupt:
    mret


// Global Vals
.section .rodata // Constants
msg_erroc: .string "erro fatal\n"

.section .data   // rw inicializadas com valor
data01: .4byte 0x00112233

.section .bss
value1: .word 0x0

