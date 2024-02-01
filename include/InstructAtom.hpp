#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructAtom : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, funct5{0}, rs1{0}, rs2{0};
    uint32_t val_rs1{0}, val_rs2{0};
    // bool flagAdd{false};

    bool aq{false}, rl{false};

    // saida
    uint32_t address{0};

  public:
    InstructAtom(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {
        rd = calcRd(i);
        funct3 = calcFunct3(i);
        funct5 = calcFunct5(i);

        uint8_t t = calcFunct7(i);
        aq = t & 0x02;
        rl = t & 0x01;

        rs1 = calcRs1(i);
        rs2 = calcRs2(i);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];

        // flagAdd = ((i >> 25) == 0);
    }

    virtual void execute(Controller& controller) override {

        if (funct3 == 0b010) { // RV32A

            switch (funct5) {
                case 0b00010: // LR.W
                              // TODO: implementar
                    std::cout << "LR.W  ";
                    break;

                case 0b00011: // SC.W
                              // TODO: implementar
                    std::cout << "SC.W  ";
                    // address = val_rs1 << (val_rs2 & 0x1f);
                    break;

                case 0b00001: // AMOSWAP.W
                              // TODO: implementar
                    std::cout << "AMOSWAP.W ";
                    // address = (val_rs1 < val_rs2) ? 1 : 0;
                    break;

                case 0b00000: // AMOADD.W
                              // TODO: implementar
                    std::cout << "AMOADD.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b00100: // AMOXOR.W
                              // TODO: implementar
                    std::cout << "AMOXOR.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b01100: // AMOAND.W
                              // TODO: implementar
                    std::cout << "AMOAND.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b01000: // AMOOR.W
                              // TODO: implementar
                    std::cout << "AMOOR.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b10000: // AMOMIN.W
                              // TODO: implementar
                    std::cout << "AMOMIN.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b10100: // AMOMAX.W
                              // TODO: implementar
                    std::cout << "AMOMAX.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b11000: // AMOMIMU.W
                              // TODO: implementar
                    std::cout << "AMOMIMU.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                case 0b11100: // AMOMAXU.W
                              // TODO: implementar
                    std::cout << "AMOMAXU.W ";
                    // address = (val_rs1 < (unsigned int)val_rs2) ? 1 : 0;
                    break;

                default:
                    // TODO: opcode ou i ?
                    controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                    break;
            }
        }

        std::cout << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << Debug::alias[rs2 & 0x1f];
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
