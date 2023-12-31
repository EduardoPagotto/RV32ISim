#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeR : public InstructionType {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0}, rs2{0}, funct7{0};

  public:
    InstructionTypeR(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        funct7 = calcFunct7(i);
    }

    virtual void step() override {}
};
