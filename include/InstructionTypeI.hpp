#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeI : public InstructionType {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0};
    int32_t imm{0};

  public:
    InstructionTypeI(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {

        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        imm = i >> 20; // FIXME: ver se precisa negativar!!!!
    }

    virtual void step() override {}
};