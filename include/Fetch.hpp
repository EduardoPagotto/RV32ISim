#pragma once
#include "Bus.hpp"
#include "PipelineStage.hpp"
#include <iostream>

class Fetch : public PipelineStage {
  public:
    Fetch(CSR* c, Bus* b, const uint32_t& startAddr)
        : PipelineStage(PipelineState::Fetch, c), bus(b), startAddr(startAddr) {
        this->reset();
    }

    virtual ~Fetch() = default;

    virtual void reset() override {
        data.pc = startAddr;
        data.pcPlus4 = startAddr;
    }

    virtual void commit() override { this->done = this->data; }

    inline const FetchData& get() const { return done; }
    // inline bool hasNext() const { return ((bus->hasData(data.pc)) & !this->csr->ecall); }

    virtual void step() override {

        if (csr->resetSignal()) {
            this->reset();

        } else if (!csr->shoulStall(state)) {

            data.pc = csr->getBranchAddressValid() ? csr->getBranchAddress() : data.pcPlus4;
            data.pcPlus4 = data.pc + 4;

            bus->load(data.instruction, data.pc, MemoryAccessWidth::Word);

            csr->prt.printAsHex(data.pc, data.instruction);
        }
    }

    void printProgram() {
        std::cout << "Program" << '\n' << std::endl;

        uint32_t addr = 0;
        do {

            uint32_t instr;
            bus->load(instr, 4 * addr, MemoryAccessWidth::Word);
            csr->prt.printAsHex(4 * addr, instr);
            std::cout << "" << '\n';

            addr++;

        } while (bus->hasData(addr));
    }

  private:
    Bus* bus = nullptr;
    uint32_t startAddr;
    FetchData data;
    FetchData done;
};