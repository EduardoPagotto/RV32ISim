#pragma once
#include "Bus.hpp"
#include "Controller.hpp"

class Fetch {
  public:
    Fetch(Controller* c, Bus* b, const uint32_t& startAddr) {
        this->crt = c;
        this->bus = b;
        this->startAddr = startAddr;
        this->state = PipelineState::Fetch;
        this->reset();
    }

    virtual ~Fetch() = default;

    void reset() {
        this->pc = startAddr;
        this->pcPlus4 = startAddr;
    }

    inline bool hasNext() const { return ((bus->hasData(pc)) & !this->crt->ecall); }

    void step() {

        if (crt->resetSignal()) {
            this->reset();

        } else if (!crt->shoulStall(state)) {

            pc = crt->getBranchAddressValid() ? crt->getBranchAddress() : pcPlus4;
            pcPlus4 = pc + 4;

            bus->load(instruction, pc, 4);

            crt->printAsHex(pc, instruction);
        }
    }

    const uint32_t getPc() const { return pc; }
    const uint32_t getPcPlus4() const { return pcPlus4; }
    const uint32_t getInstruction() const { return instruction; }

    void printProgram() {
        std::cout << "Program" << '\n' << std::endl;

        uint32_t addr = 0;
        do {

            uint32_t instr;
            bus->load(instr, 4 * addr, 4);
            crt->printAsHex(4 * addr, instr);
            std::cout << "" << '\n';

            addr++;

        } while (bus->hasData(addr));
    }

  private:
    PipelineState state;
    Bus* bus = nullptr;
    Controller* crt;
    uint32_t pc, pcPlus4, instruction, startAddr;
};