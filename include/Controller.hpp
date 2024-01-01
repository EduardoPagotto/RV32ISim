#pragma once
#include "CSR.hpp"

class Controller {
    bool branchAddressValid{false}; // CSR
    bool resetSignal{false};        // CSR
    uint32_t branchAddress{0};      // CSR
    uint32_t startupAddr{0};        // CPU
    uint32_t pc{0};                 // fetch
    uint32_t pcPlus4{0};            // fetch
    CSR csr;

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
        this->pc = this->getBranchAddressValid() ? this->getBranchAddress() : this->pcPlus4;
        this->pcPlus4 = this->pc + 4;
    }
};