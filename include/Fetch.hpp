#pragma once
#include "Bus.hpp"
#include "Controller.hpp"

class Fetch {
  public:
    Fetch(Controller* c, Bus* b, const uint32_t& startAddr)
        : crt(c), bus(b), startAddr(startAddr), state(PipelineState::Fetch) {
        this->reset();
    }

    virtual ~Fetch() = default;

    void reset() {
        data.pc = startAddr;
        data.pcPlus4 = startAddr;
    }

    inline void commit() { this->done = this->data; }
    inline const FetchData& get() const { return done; }
    inline bool hasNext() const { return ((bus->hasData(data.pc)) & !this->crt->ecall); }

    void step() {

        if (crt->resetSignal()) {
            this->reset();

        } else if (!crt->shoulStall(state)) {

            data.pc = crt->getBranchAddressValid() ? crt->getBranchAddress() : data.pcPlus4;
            data.pcPlus4 = data.pc + 4;

            bus->load(data.instruction, data.pc, 4);

            crt->printAsHex(data.pc, data.instruction);
        }
    }

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
    uint32_t startAddr;
    FetchData data;
    FetchData done;
};