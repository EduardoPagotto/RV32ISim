#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeB : public InstructionType {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};
    uint32_t val_rs1{0}, val_rs2{0};

  public:
    InstructionTypeB(const OpCodeSet& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        imm = ((i >> 25) << 5) + ((i >> 7) & 0x1f) - 1;
        if (imm > 0) {
            imm++;
        }

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];
    }

    virtual void execute(Controller& controller) override {

        switch (funct3) {
            case 0x0: // BEQ
                std::cout << "beq   ";
                if (val_rs1 == val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;

            case 0x1: // BNE
                std::cout << "bne   ";
                if (val_rs1 != val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;

            case 0x4: // BLT
                std::cout << "blt   ";
                if (val_rs1 < val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;

            case 0x5: // BGE
                std::cout << "bge   ";
                if (val_rs1 >= val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;

            case 0x6: // BLTU
                std::cout << "bltu  ";
                if (val_rs1 < (unsigned)val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;

            case 0x7: // BGEU
                std::cout << "bgeu  ";
                if (val_rs1 >= (unsigned)val_rs2)
                    controller.setBranchAddress(controller.getPC() + imm);
                break;
        }
        std::cout << Debug::alias[rs1] << ", " << Debug::alias[rs2] << ", " << imm;
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {
        return WriteBackData{0, 0, false};
    }
};