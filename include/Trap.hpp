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

        switch (static_cast<TrapState>(state)) {
            case TrapState::Idle: {
                return;
            }

            case TrapState::SetCSRJump: {

                csr->write(CSR_MEPC, mepc);
                csr->write(CSR_MCAUSE, mcause);
                csr->write(CSR_MTVAL, mtval);

                uint32_t mstatus = csr->read(CSR_MSTATUS);
                const uint32_t mie = (mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
                // Unset MPIE bit
                mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
                // Save MIE to MPIE
                mstatus = (mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
                // Unset mie
                mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

                csr->write(CSR_MSTATUS, mstatus);

                const uint32_t index = mcause & 0x7fffffff;
                const bool isInterrupt = mcause & 0x80000000;
                const uint32_t offset = isInterrupt ? 0 : 48;

                uint32_t mtvec = csr->read(CSR_MTVEC);
                this->pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);

                this->state = TrapState::SetPc;

                csr->beginTrap();

                return;
            }

            case TrapState::SetPc: {
                csr->setBranchAddress(this->pcToSet);
                csr->intoTrap();
                this->state = TrapState::Idle;
                return;
            }

            case TrapState::ReturnFromTrap: {

                // TODO
                this->pcToSet = csr->read(CSR_MEPC); // sr->mem.mepc;
                this->state = TrapState::SetPc;

                uint32_t mstatus = csr->read(CSR_MSTATUS);
                const uint32_t mpie = (mstatus & MSTATUS_MPIE_MASK) >> MSTATUS_MPIE_BIT;
                // Unset MIE bit
                mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>
                // Save MPIE to MIE
                mstatus = (mstatus | (mpie << MSTATUS_MIE_BIT)) >> 0; // >>>
                // Unset mpie
                mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>

                csr->write(CSR_MSTATUS, mstatus);

                return;
            }
        }
    }

    void commit() {}
    void reset() {}

  private:
    CSR* csr;
    TrapState state;
    uint32_t mepc, mcause, mtval, pcToSet;
};
