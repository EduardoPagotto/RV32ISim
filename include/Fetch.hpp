#pragma once
#include "Bus.hpp"
#include "Controller.hpp"

class Fetch {
  public:
    Fetch(Controller* c, Bus* b, const uint32_t& startAddr) {
        this->crt = c;
        this->bus = b;
        this->pcPlus4 = startAddr;
        this->startAddr = startAddr;
        this->state = PipelineState::Fetch;
    }

    virtual ~Fetch() = default;

    void reset() { this->pcPlus4 = startAddr; }

    void step() {

        if (crt->resetSignal()) {
            this->reset();

        } else if (!crt->shoulStall(state)) {

            pc = crt->getBranchAddressValid() ? crt->getBranchAddress() : pcPlus4;
            pcPlus4 = pc + 4;

            bus->load(instruction, pc, 4);

            // crt->printAsHex(pc, instruction);
        }
    }

    const uint32_t getPc() const { return pc; }
    const uint32_t getPcPlus4() const { return pcPlus4; }
    const uint32_t getInstruction() const { return instruction; }

  private:
    PipelineState state;
    Bus* bus = nullptr;
    Controller* crt;
    uint32_t pc, pcPlus4, instruction, startAddr;
};