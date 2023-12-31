#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeS : public InstructionType {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};

    uint32_t val_rs1{0};
    uint32_t val_rs2{0};

  public:
    InstructionTypeS(const OpCodeSet& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        imm = ((i >> 25) << 5) | ((i >> 7) & 0x1f);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];
    }

    virtual void step() override {

        uint32_t address = val_rs1 + imm;
        MemoryAccessWidth width;

        switch (funct3) {
            case 0x0:
                width = MemoryAccessWidth::Byte;
                break;

            case 0x1:
                width = MemoryAccessWidth::HalfWord;
                break;

            case 0x2:
                width = MemoryAccessWidth::Word;
                break;
        };

        // TODO: implementar a chamada para accesMemory
    }
};