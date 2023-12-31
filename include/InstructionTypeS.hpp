#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeS : public InstructionType {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};

  public:
    InstructionTypeS(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        opcode = o;
        funct3 = (i >> 12) & 0x7;
        rs1 = (i >> 15) & 0x1f;
        rs2 = (i >> 20) & 0x1f;
        imm = ((i >> 25) << 5) | ((i >> 7) & 0x1f);
    }

    virtual void step() override {}
};