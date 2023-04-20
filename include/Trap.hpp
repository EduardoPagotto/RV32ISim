#pragma once
#include "CSR.hpp"
#include "Controller.hpp"
#include <stdint.h>

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
    Trap(Controller* crt, CSR* csr) {

        this->csr = csr;
        this->crt = crt;
        this->state = static_cast<uint32_t>(TrapState::Idle);
    }

    virtual ~Trap() = default;

    void trapException(uint32_t mepc, uint32_t mcause, uint32_t mtval) {
        this->mepc = mepc;
        this->mcause = mcause;
        this->mtval = mtval;
        this->state = static_cast<uint32_t>(TrapState::SetCSRJump);
    }

    void trapReturn() { this->state = static_cast<uint32_t>(TrapState::ReturnFromTrap); }

    void compute() {
        if (this->crt->beginTrap()) { // FIXME funcao aqui mover para o controller!!!!

            this->state = static_cast<uint32_t>(TrapState::SetCSRJump);
            this->flush = 1;

        } else if (this->crt->beginTrapReturn()) { // FIXME funcao aqui mover para o controller !!!!

            this->state = static_cast<uint32_t>(TrapState::ReturnFromTrap);
            this->flush = 1;

        } else {
            switch (static_cast<TrapState>(state)) {
                case TrapState::Idle: {
                    this->returnToPipelineMode = 0;
                    this->setPc = 0;
                    this->flush = 0;
                    return;
                }

                case TrapState::SetCSRJump: {
                    csr->mepc = mepc;
                    csr->mcause = mcause;
                    csr->mtval = mtval;

                    const uint32_t mie = (csr->mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
                    // Unset MPIE bit
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
                    // Save MIE to MPIE
                    csr->mstatus = (csr->mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
                    // Unset mie
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

                    const uint32_t index = mcause & 0x7fffffff;
                    const uint32_t isInterrupt = mcause & 0x80000000;
                    const uint32_t offset = isInterrupt ? 0 : 48;

                    this->pcToSet = (csr->mtvec & 0xfffffffc) + offset + (index << 2);

                    this->setPc = 1;
                    this->returnToPipelineMode = 1;
                    this->flush = 0;

                    this->state = static_cast<uint32_t>(TrapState::Idle);

                    return;
                }

                case TrapState::SetPc: {
                    this->setPc = 1;
                    this->returnToPipelineMode = 1;
                    this->state = static_cast<uint32_t>(TrapState::Idle);
                    this->flush = 0;
                    return;
                }

                case TrapState::ReturnFromTrap: {
                    this->pcToSet = csr->mepc;
                    this->state = static_cast<uint32_t>(TrapState::SetPc);
                    this->flush = 0;

                    const uint32_t mpie = (csr->mstatus & MSTATUS_MPIE_MASK) >> MSTATUS_MPIE_BIT;
                    // Unset MIE bit
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>
                    // Save MPIE to MIE
                    csr->mstatus = (csr->mstatus | (mpie << MSTATUS_MIE_BIT)) >> 0; // >>>
                    // Unset mpie
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>

                    return;
                }
            }
        }
    }

    void commit() {
        // state.commit();
        // mepc.commit();
        // mcause.commit();
        // mtval.commit();
        // returnToPipelineMode.commit();
        // flush.commit();
        // setPc.commit();
        // pcToSet.commit();
    }
    void reset() {
        // state.reset();
        // mepc.reset();
        // mcause.reset();
        // mtval.reset();
        // returnToPipelineMode.reset();
        // flush.reset();
        // setPc.reset();
        // pcToSet.reset();
    }

  private:
    CSR* csr;
    Controller* crt;
    uint32_t state, mepc, mcause, mtval, returnToPipelineMode, flush, setPc, pcToSet;
};
