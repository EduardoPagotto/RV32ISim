#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructIULAI : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0};
    int32_t imm{0};
    uint32_t val_rs1{0};

    // saida
    uint32_t address{0};

  public:
    InstructIULAI(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {

        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        imm = i >> 20;

        // Negativar
        if (i & 0x80000000) {
            imm |= 0xFFFFF000;
        }

        val_rs1 = x[rs1];
    }

    virtual void execute(Controller& controller) override {

        switch (funct3) {
            case 0x0: // ADDI
                std::cout << "ADDI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = val_rs1 + imm;
                break;

            case 0x1: // SLLI
                std::cout << "SLLI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = val_rs1 << imm;
                break;

            case 0x2: // SLTI
                std::cout << "SLTI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = (val_rs1 < imm) ? 1 : 0;
                break;

            case 0x3: // SLTIU
                std::cout << "SLTIU " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", "
                          << (unsigned int)(imm & 0xfff);
                address = (val_rs1 < ((unsigned int)(imm & 0xfff))) ? 1 : 0;
                break;

            case 0x4: // XORI
                std::cout << "XORI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = val_rs1 ^ imm;
                break;

            case 0x5: // SLRI / SRAI
                if ((imm & 0xf00) == 0) {
                    std::cout << "SLRI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                    address = val_rs1 >> imm;
                } else {
                    std::cout << "SRAI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << (imm & 0x1f);
                    address = val_rs1 >> (imm & 0x1f);
                }
                break;

            case 0x6: // ORI
                std::cout << "ORI   " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = val_rs1 | imm;
                break;

            case 0x7: // ANDI
                std::cout << "ANDI  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
                address = val_rs1 & imm;
                break;

            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};