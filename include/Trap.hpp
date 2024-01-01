#pragma once
#include <stdint.h>

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

class Trap {
  private:
    uint32_t mepc{0}, mtval{0}, pcToSet{0};
    TrapState trapState{TrapState::Idle};
    MCause mcause{MCause::IllegalInstruction};

  public:
    Trap(const uint32_t& mepc, MCause cause, uint32_t mtval)
        : trapState(TrapState::SetCSRJump), mepc(mepc), mcause(cause), mtval(mtval), pcToSet(0) {}

    ~Trap() = default;

    Trap() = delete;
    Trap(const Trap& t) = delete;

    void setPC(const uint32_t& mtvec) {

        const uint32_t mc = static_cast<uint32_t>(mcause);

        const uint32_t index = mc & 0x7fffffff;

        const bool isInterrupt = mc & 0x80000000;

        const uint32_t offset = isInterrupt ? 0 : 48;

        if ((mtvec & 01) == 01) {
            pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        } else {
            pcToSet = (mtvec & 0xfffffffc);
        }

        // pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        trapState = TrapState::SetPc;
    }
};