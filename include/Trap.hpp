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
        state.set(static_cast<uint32_t>(TrapState::Idle));
    }

    virtual ~Trap() = default;

    void trapException(uint32_t mepc, uint32_t mcause, uint32_t mtval) {
        this->mepc.set(mepc);
        this->mcause.set(mcause);
        this->mtval.set(mtval);
        this->state.set(static_cast<uint32_t>(TrapState::SetCSRJump));
    }

    void trapReturn() { state.set(static_cast<uint32_t>(TrapState::ReturnFromTrap)); }

    void compute() {
        if (this->crt->beginTrap()) { // FIXME funcao aqui mover para o controller!!!!

            state.set(static_cast<uint32_t>(TrapState::SetCSRJump));
            flush.set(1);

        } else if (this->crt->beginTrapReturn()) { // FIXME funcao aqui mover para o controller !!!!

            state.set(static_cast<uint32_t>(TrapState::ReturnFromTrap));
            flush.set(1);

        } else {
            switch (static_cast<TrapState>(state.get())) {
                case TrapState::Idle: {
                    returnToPipelineMode.set(0);
                    setPc.set(0);
                    flush.set(0);
                    return;
                }

                case TrapState::SetCSRJump: {
                    csr->mepc = mepc.get();
                    csr->mcause = mcause.get();
                    csr->mtval = mtval.get();

                    const uint32_t mie = (csr->mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
                    // Unset MPIE bit
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
                    // Save MIE to MPIE
                    csr->mstatus = (csr->mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
                    // Unset mie
                    csr->mstatus = (csr->mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

                    const uint32_t index = mcause.get() & 0x7fffffff;
                    const uint32_t isInterrupt = mcause.get() & 0x80000000;
                    const uint32_t offset = isInterrupt ? 0 : 48;

                    pcToSet.set((csr->mtvec & 0xfffffffc) + offset + (index << 2));

                    setPc.set(1);
                    returnToPipelineMode.set(1);
                    flush.set(0);

                    state.set(static_cast<uint32_t>(TrapState::Idle));

                    return;
                }

                case TrapState::SetPc: {
                    setPc.set(1);
                    returnToPipelineMode.set(1);
                    state.set(static_cast<uint32_t>(TrapState::Idle));
                    flush.set(0);
                    return;
                }

                case TrapState::ReturnFromTrap: {
                    pcToSet.set(csr->mepc);
                    state.set(static_cast<uint32_t>(TrapState::SetPc));
                    flush.set(0);

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
        state.commit();
        mepc.commit();
        mcause.commit();
        mtval.commit();
        returnToPipelineMode.commit();
        flush.commit();
        setPc.commit();
        pcToSet.commit();
    }
    void reset() {
        state.reset();
        mepc.reset();
        mcause.reset();
        mtval.reset();
        returnToPipelineMode.reset();
        flush.reset();
        setPc.reset();
        pcToSet.reset();
    }

  private:
    CSR* csr;
    Controller* crt;
    RLatch<uint32_t> state, mepc, mcause, mtval, returnToPipelineMode, flush, setPc, pcToSet;
};
