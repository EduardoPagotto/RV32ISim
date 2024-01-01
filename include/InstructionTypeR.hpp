#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeR : public InstructionType {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0}, rs2{0}, funct7{0};
    uint32_t val_rs1{0}, val_rs2{0};
    bool flagAdd{false};

    // saida
    uint32_t address{0};

  public:
    InstructionTypeR(const OpCode& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        funct7 = calcFunct7(i);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];

        flagAdd = ((i >> 25) == 0);
    }

    virtual void execute(Controller& controller) override {

        switch (funct3) {
            case 0x0: // ADD / SUB
                if (flagAdd) {
                    address = val_rs1 + val_rs2;
                    std::cout << "add   ";
                } else {
                    address = val_rs1 - val_rs2;
                    std::cout << "sub   ";
                }
                // address = flagAdd ? val_rs1 + val_rs2 : val_rs1 - val_rs2;
                break;

            case 0x1: // SLL
                std::cout << "sll   ";
                address = val_rs1 << (val_rs2 & 0x1f);
                break;

            case 0x2: // SLT
                std::cout << "slt   ";
                address = (val_rs1 < val_rs2) ? 1 : 0;
                break;

            case 0x3: // SLTU
                std::cout << "sltu  ";
                address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                break;

            case 0x4: // XOR
                std::cout << "xor   ";
                address = val_rs1 ^ val_rs2;
                break;

            case 0x5: // SRL / SRA
                if (flagAdd) {
                    address = ((unsigned int)val_rs1) >> (val_rs2 & 0x1f);
                } else {
                    address = val_rs1 >> (val_rs2 & 0x1f);
                }
                // address = flagAdd ? ((unsigned int)val_rs1) >> (val_rs2 & 0x1f) : val_rs1 >> (val_rs2 & 0x1f);
                break;

            case 0x6: // OR
                std::cout << "OR    ";
                address = val_rs1 | val_rs2;
                break;

            case 0x7: // AND
                std::cout << "and   ";
                address = val_rs1 & val_rs2;
                break;
        }

        std::cout << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << Debug::alias[rs2 & 0x1f];

        // TODO: implementar a chamada a memory
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
