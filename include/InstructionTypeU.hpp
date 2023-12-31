#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeU : public InstructionType {
  private:
    uint8_t rd{0};
    int32_t imm{0};

  public:
    InstructionTypeU(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        opcode = o;
        rd = ((i >> 7) & 0x1f);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void step() override {}
};
