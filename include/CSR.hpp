#pragma once
#include "PrintAs.hpp"
#include "defs.hpp"

// const MSTATUS_MASK = (1 << 3) | (1 << 7);

class CSR {
  public:
    CSR() = default;
    virtual ~CSR() = default;

    void step() { mem.cycles++; }
    void commit() {}
    uint32_t read(uint32_t address);
    void write(uint32_t address, uint32_t value);

    // Controller
    const bool resetSignal() const { return false; }
    const bool getBranchAddressValid() { return branchAddressValid; }
    const uint32_t getBranchAddress() {
        branchAddressValid = false;
        return branchAddress;
    }
    void setBranchAddress(const uint32_t& addr) {
        branchAddress = addr;
        branchAddressValid = true;

        prt.printPC(addr); // TODO: melhorar
    }

    bool shoulStall(PipelineState state) { return !(state == this->pipelineState); }

    bool beginTrap() { return __beginTrap; }
    bool beginTrapReturn() { return __beginTrapReturn; }

    // bool ecall = false; // For program termination

    void nextState();
    void cpuControl();

  public: // FIXME em Trap
    CsrMem mem;
    PrintAs prt;

  private:
    // controller
    bool trapStall = false;
    CPUState cpuState = CPUState::Pipeline;
    PipelineState pipelineState = PipelineState::Fetch;
    uint32_t branchAddress = 0;
    bool branchAddressValid = false;
    bool __beginTrap = false;
    bool __beginTrapReturn = false;
};
