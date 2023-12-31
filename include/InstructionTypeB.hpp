#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeB : public InstructionType {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};

  public:
    InstructionTypeB(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        imm = ((i >> 25) << 5) + ((i >> 7) & 0x1f) - 1;
        if (imm > 0) {
            imm++;
        }
    }

    virtual void step() override {}
};