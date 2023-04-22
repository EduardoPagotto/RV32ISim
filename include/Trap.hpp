#pragma once
#include "CSR.hpp"

enum class MCause : uint64_t {
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

enum class TrapState { Idle, SetCSRJump, ReturnFromTrap, SetPc };

#define MSTATUS_MIE_BIT 3
#define MSTATUS_MIE_MASK (1 << MSTATUS_MIE_BIT)
#define MSTATUS_MPIE_BIT 7
#define MSTATUS_MPIE_MASK (1 << MSTATUS_MPIE_BIT)

class Trap {

  public:
    Trap(CSR* csr) {
        this->csr = csr;
        this->state = TrapState::Idle;
    }

    virtual ~Trap() = default;

    void trapException(uint32_t mepc, uint32_t mcause, uint32_t mtval) {
        this->mepc = mepc;
        this->mcause = mcause;
        this->mtval = mtval;
        this->state = TrapState::SetCSRJump;
    }

    void trapReturn() { this->state = TrapState::ReturnFromTrap; }

    void step() {

        if (this->csr->beginTrap()) { // FIXME funcao aqui mover para o controller!!!!

            this->state = TrapState::SetCSRJump;
            this->flush = true;

        } else if (this->csr->beginTrapReturn()) { // FIXME funcao aqui mover para o controller !!!!

            this->state = TrapState::ReturnFromTrap;
            this->flush = true;

        } else {
            switch (static_cast<TrapState>(state)) {
                case TrapState::Idle: {
                    this->returnToPipelineMode = false;
                    this->setPc = false;
                    this->flush = false;
                    return;
                }

                case TrapState::SetCSRJump: {
                    csr->mem.mepc = mepc;
                    csr->mem.mcause = mcause;
                    csr->mem.mtval = mtval;

                    const uint32_t mie = (csr->mem.mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
                    // Unset MPIE bit
                    csr->mem.mstatus = (csr->mem.mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
                    // Save MIE to MPIE
                    csr->mem.mstatus = (csr->mem.mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
                    // Unset mie
                    csr->mem.mstatus = (csr->mem.mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

                    const uint32_t index = mcause & 0x7fffffff;
                    const uint32_t isInterrupt = mcause & 0x80000000;
                    const uint32_t offset = isInterrupt ? 0 : 48;

                    this->pcToSet = (csr->mem.mtvec & 0xfffffffc) + offset + (index << 2);

                    this->setPc = true;
                    this->returnToPipelineMode = true;
                    this->flush = false;

                    this->state = TrapState::Idle;

                    return;
                }

                case TrapState::SetPc: {
                    this->setPc = true;
                    this->returnToPipelineMode = true;
                    this->state = TrapState::Idle;
                    this->flush = false;
                    return;
                }

                case TrapState::ReturnFromTrap: {
                    this->pcToSet = csr->mem.mepc;
                    this->state = TrapState::SetPc;
                    this->flush = false;

                    const uint32_t mpie = (csr->mem.mstatus & MSTATUS_MPIE_MASK) >> MSTATUS_MPIE_BIT;
                    // Unset MIE bit
                    csr->mem.mstatus = (csr->mem.mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>
                    // Save MPIE to MIE
                    csr->mem.mstatus = (csr->mem.mstatus | (mpie << MSTATUS_MIE_BIT)) >> 0; // >>>
                    // Unset mpie
                    csr->mem.mstatus = (csr->mem.mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>

                    return;
                }
            }
        }
    }

    void commit() {}

    void reset() {}

  private:
    CSR* csr;
    TrapState state;
    uint32_t mepc, mcause, mtval, pcToSet;
    bool returnToPipelineMode, setPc, flush;
};
