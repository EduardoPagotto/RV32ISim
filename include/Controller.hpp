#pragma once
#include "CSR.hpp"
#include "Trap.hpp"

class Controller {
    bool branchAddressValid{false};
    bool resetSignal{false};
    uint32_t branchAddress{0};
    uint32_t startupAddr{0};
    uint32_t pc{0};
    uint32_t pcPlus4{0};
    CSR csr;
    Trap trap;

  public:
    Controller() {}
    virtual ~Controller() = default;
    void setStartUpAddr(const uint32_t addr) { this->startupAddr = addr; }

    const uint32_t getPC() const { return this->pc; }
    const uint32_t getPcplus4() const { return this->pcPlus4; }
    const bool getResetSignal() const { return this->resetSignal; }
    const bool getBranchAddressValid() { return branchAddressValid; }
    CSR& getCSR() { return csr; }

    const uint32_t getBranchAddress() {
        branchAddressValid = false;
        return branchAddress;
    }

    void reset() {
        this->branchAddressValid = true;
        this->branchAddress = startupAddr;
    }

    void setBranchAddress(const uint32_t& addr) {
        branchAddress = addr;
        branchAddressValid = true;
    }

    void step() {

        // FIXME: entender!!! // After las Writeback
        csr.increment64(CSR_INSTRET, CSR_INSTRETH, true);

        this->pc = this->getBranchAddressValid() ? this->getBranchAddress() : this->pcPlus4;
        this->pcPlus4 = this->pc + 4;
    }

    bool noWaitTrap() {

        // FIXME: entender!!!
        csr.increment64(CSR_CYCLE, CSR_CYCLEH, true);

        switch (trap.trapState) {
            case TrapState::Idle: // 3 - depois de 2 entra em iddle (este e o trap inicial)
                return true;

            case TrapState::SetCSRJump: { // 1 - disparado o trap de algum lugar

                // TODO: antes do preenchimento verificar se ja nao estite um trap anterior e colocar no stack!!!!

                csr.write(CSR_MEPC, trap.mepc);
                csr.write(CSR_MCAUSE, static_cast<uint32_t>(trap.mcause));
                csr.write(CSR_MTVAL, trap.mtval);

                uint32_t mstatus = csr.read(CSR_MSTATUS);
                const uint32_t mie = (mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
                // Unset MPIE bit
                mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
                // Save MIE to MPIE
                mstatus = (mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
                // Unset mie
                mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

                csr.write(CSR_MSTATUS, mstatus);

                this->setOriginalPC(csr.read(CSR_MTVEC));

                return false;
            }

            case TrapState::SetPc: { // 2 - executar carga de novo PC e vai a idlle
                                     // 5 - retorna PC antigo e volta a idle
                this->setBranchAddress(trap.pcToSet);
                // std::cout << '\n';
                // this->cpuState = CPUState::Pipeline;
                // this->pipelineState = PipelineState::WriteBack; // PipelineState::Fetch;
                this->trap.trapState = TrapState::Idle;
                return true;
            }

            case TrapState::ReturnFromTrap: { // 4 sinalizado retorno do trap

                // TODO
                trap.pcToSet = csr.read(CSR_MEPC) + 4;
                trap.trapState = TrapState::SetPc;

                uint32_t mstatus = csr.read(CSR_MSTATUS);
                const uint32_t mpie = (mstatus & MSTATUS_MPIE_MASK) >> MSTATUS_MPIE_BIT;
                // Unset MIE bit
                mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>
                // Save MPIE to MIE
                mstatus = (mstatus | (mpie << MSTATUS_MIE_BIT)) >> 0; // >>>
                // Unset mpie
                mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>

                csr.write(CSR_MSTATUS, mstatus);

                return false;
            }
        }

        return true;
    }

    void trapException(const Trap& t) {
        // TODO: falta toda a camada de segurança
        trap = t;
    }

    void trapReturn() {
        // TODO: falta toda a camada de segurança
        trap.trapState = TrapState::ReturnFromTrap;
    }

  private:
    void setOriginalPC(const uint32_t& mtvec) {

        const uint32_t mc = static_cast<uint32_t>(trap.mcause);

        const uint32_t index = mc & 0x7fffffff;

        const bool isInterrupt = mc & 0x80000000;

        const uint32_t offset = isInterrupt ? 0 : 48;

        if ((mtvec & 01) == 01) {
            trap.pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        } else {
            trap.pcToSet = (mtvec & 0xfffffffc);
        }

        // pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        trap.trapState = TrapState::SetPc;
    }
};