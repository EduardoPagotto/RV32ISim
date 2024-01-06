#pragma once
#include <iostream>
#include <stdint.h>

#define MSTATUS_MIE_BIT 3
#define MSTATUS_MIE_MASK (1 << MSTATUS_MIE_BIT)
#define MSTATUS_MPIE_BIT 7
#define MSTATUS_MPIE_MASK (1 << MSTATUS_MPIE_BIT)

enum class MCause : uint32_t {
    // Interrupts
    UserSoftwareInterrupt = 0x80000000,
    SupervisorSoftwareInterrupt,
    Reserved0,
    MachineSoftwareInterrupt,
    UserTimerInterrupt,
    SupervisorTimerInterrupt,
    Reserved1,
    MachineTimerInterrupt,
    UserExternalInterrupt,
    SupervisorExternalInterrupt,
    Reserved2,
    MachineExternalInterrupt,
    // Exceptions
    InstructionAddressMisaligned = 0x00000000,
    InstructionAccessFault,
    IllegalInstruction,
    Breakpoint,
    LoadAddressMisaligned,
    LoadAccessFault,
    StoreAMOAddressMisaligned,
    StoreAMOAccessFault,
    EnvironmentCallFromUMode,
    EnvironmentCallFromSMode,
    Reserved3,
    EnvironmentCallFromMMode,
    InstructionPageFault,
    LoadPageFault,
    Reserved4,
    StoreAMOPageFault,
};

enum class TrapState {
    Idle,
    SetCSRJump,
    ReturnFromTrap,
    SetPc,
};

struct Trap {

    uint32_t mepc{0}, mtval{0}, pcToSet{0};
    TrapState trapState{TrapState::Idle};
    MCause mcause{MCause::IllegalInstruction};

    Trap() = default;
    Trap(const Trap& t) = default;
    Trap(const uint32_t& mepc, MCause cause, uint32_t mtval)
        : trapState(TrapState::SetCSRJump), mepc(mepc), mcause(cause), mtval(mtval), pcToSet(0) {
        std::cout << "\t# <<< TRAP >>>!!! ";
    }

    ~Trap() = default;
};