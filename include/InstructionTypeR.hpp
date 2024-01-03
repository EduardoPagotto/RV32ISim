#pragma once
#include "Debug.hpp"
#include "InstructionType.hpp"

class InstructionTypeR : public InstructionType {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0}, rs2{0};
    uint32_t val_rs1{0}, val_rs2{0};
    bool flagAdd{false};

    // saida
    uint32_t address{0};

  public:
    InstructionTypeR(const uint32_t& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];

        flagAdd = ((i >> 25) == 0);
    }

    virtual void execute(Controller& controller) override {

        switch (funct3) {
            case 0x0: // ADD / SUB
                if (flagAdd) {
                    address = val_rs1 + val_rs2;
                    std::cout << "ADD   ";
                } else {
                    address = val_rs1 - val_rs2;
                    std::cout << "SUB   ";
                }
                // address = flagAdd ? val_rs1 + val_rs2 : val_rs1 - val_rs2;
                break;

            case 0x1: // SLL
                std::cout << "SLL   ";
                address = val_rs1 << (val_rs2 & 0x1f);
                break;

            case 0x2: // SLT
                std::cout << "SLT   ";
                address = (val_rs1 < val_rs2) ? 1 : 0;
                break;

            case 0x3: // SLTU
                std::cout << "SLTU  ";
                address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                break;

            case 0x4: // XOR
                std::cout << "XOR   ";
                address = val_rs1 ^ val_rs2;
                break;

            case 0x5: // SRL / SRA
                if (flagAdd) {
                    std::cout << "SRL   ";
                    address = ((unsigned int)val_rs1) >> (val_rs2 & 0x1f);
                } else {
                    std::cout << "SRA   ";
                    address = val_rs1 >> (val_rs2 & 0x1f);
                }
                // address = flagAdd ? ((unsigned int)val_rs1) >> (val_rs2 & 0x1f) : val_rs1 >> (val_rs2 & 0x1f);
                break;

            case 0x6: // OR
                std::cout << "OR    ";
                address = val_rs1 | val_rs2;
                break;

            case 0x7: // AND
                std::cout << "AND   ";
                address = val_rs1 & val_rs2;
                break;
            default:
                throw std::string("Opp desconhecida");
                break;
        }

        std::cout << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << Debug::alias[rs2 & 0x1f];

        // TODO: implementar a chamada a memory
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
