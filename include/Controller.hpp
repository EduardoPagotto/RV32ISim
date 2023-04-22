#pragma once
#include "defs.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

class Controller { // TODO: mover para o CSR
  public:
    Controller() = default;
    virtual ~Controller() = default;

    const bool resetSignal() const { return false; }
    const bool getBranchAddressValid() { return branchAddressValid; }
    const uint32_t getBranchAddress() {
        branchAddressValid = false;
        return branchAddress;
    }
    void setBranchAddress(const uint32_t& addr) {
        branchAddress = addr;
        branchAddressValid = true;
    }

    // bool shoulStall(PipelineState state) { return !(state == this->state); } // FIXME: no pipeline
    bool shoulStall(PipelineState state) { return false; }

    PipelineState state = PipelineState::Fetch;

    bool beginTrap() { return __beginTrap; }
    bool beginTrapReturn() { return __beginTrapReturn; }

    bool ecall = false; // For program termination

  private:
    uint32_t branchAddress = 0;
    bool branchAddressValid = false;
    bool __beginTrap = false;
    bool __beginTrapReturn = false;
};