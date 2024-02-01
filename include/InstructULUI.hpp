#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructULUI : public Instruct {
  private:
    uint8_t rd{0};
    int32_t imm{0};
    uint32_t address{0};

  public:
    InstructULUI(const uint32_t& o, const uint32_t& i) : Instruct(o) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void execute(Controller& controller) override {
        std::cout << "LUI   " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
        address = imm;
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
