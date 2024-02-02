#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructAtom : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, funct5{0}, rs1{0}, rs2{0};
    uint32_t val_rs1{0}, val_rs2{0};

    bool aq{false}, rl{false};
    MemoryAccessWidth width;
    bool valSigned{false};
    uint32_t* regs;

    // saida

  public:
    InstructAtom(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {
        rd = calcRd(i);
        funct3 = calcFunct3(i);
        funct5 = calcFunct5(i);
        regs = x;

        uint8_t t = calcFunct7(i);
        aq = t & 0x02;
        rl = t & 0x01;

        rs1 = calcRs1(i);
        rs2 = calcRs2(i);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];
    }

    virtual void execute(Controller& controller) override {

        if (funct3 == 0b010) { // RV32A

            switch (funct5) {
                case 0b00010: { // LR.W

                    // FIXME: implementar trava no acesso a memoria
                    // address = val_rs1;
                    width = MemoryAccessWidth::Word;
                    valSigned = true;

                    std::cout << "LR.W";
                    if (aq)
                        std::cout << ".aq";

                    if (rl)
                        std::cout << ".rl";

                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs1];

                } break;

                case 0b00011: { // SC.W

                    // FIXME: implementar trava na memoria
                    // address = val_rs1;
                    width = MemoryAccessWidth::Word;

                    std::cout << "SC.W";
                    if (aq)
                        std::cout << ".aq";

                    if (rl)
                        std::cout << ".rl";

                    std::cout << " " << Debug::alias[rd] << ", (" << Debug::alias[rs1] << "), " << Debug::alias[rs2];

                } break;

                case 0b00001: { // AMOSWAP.W

                    // TODO: implementar
                    // address = val_rs1;
                    width = MemoryAccessWidth::Word;
                    valSigned = true;

                    std::cout << "AMOSWAP.W";
                    if (aq)
                        std::cout << ".aq";

                    if (rl)
                        std::cout << ".rl";

                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";
                } break;

                case 0b00000: // AMOADD.W
                              // TODO: implementar
                    std::cout << "AMOADD.W ";
                    break;

                case 0b00100: // AMOXOR.W
                              // TODO: implementar
                    std::cout << "AMOXOR.W ";
                    break;

                case 0b01100: // AMOAND.W
                              // TODO: implementar
                    std::cout << "AMOAND.W ";
                    break;

                case 0b01000: // AMOOR.W
                              // TODO: implementar
                    std::cout << "AMOOR.W ";
                    break;

                case 0b10000: // AMOMIN.W
                              // TODO: implementar
                    std::cout << "AMOMIN.W ";
                    break;

                case 0b10100: // AMOMAX.W
                              // TODO: implementar
                    std::cout << "AMOMAX.W ";
                    break;

                case 0b11000: // AMOMIMU.W
                              // TODO: implementar
                    std::cout << "AMOMIMU.W ";
                    break;

                case 0b11100: // AMOMAXU.W
                              // TODO: implementar
                    std::cout << "AMOMAXU.W ";
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

        if (((width == MemoryAccessWidth::Word && val_rs1 & 0b11) ||
             (width == MemoryAccessWidth::HalfWord && val_rs1 & 0b01))) {

            controller.trapException(Trap(val_rs1, MCause::LoadAddressMisaligned, opcode));
            return WriteBackData{0, 0, false};
        }

        const auto [erro, vAddress] = mmu.getPhysicalAddress(val_rs1, 0, MMU_ACC_READ | MMU_ACC_SUPER);
        if (erro == MMU_OK) {

            switch (funct5) {
                case 0b00010: // LR.W
                {
                    // FIXME: verificar trava
                    const auto [erro, value] = bus.load(vAddress, width, valSigned);

                    if (erro == MMU_OK)
                        return WriteBackData{rd, value, true};
                } break;

                case 0b00011: // SC.W
                {
                    // FIXME: falta a trava do memoria
                    auto [error, value] = bus.store(vAddress, width, val_rs2);

                    if (error == MMU_OK) { // FIXME: implementar corretamente
                        // FIXME: se trava falhar rd deve ser diferente de zero!!!!
                        return WriteBackData{rd, 0, true};
                    }

                } break;
                case 0b00001: { // AMOSWAP.W

                    // FIXME: verificar trava e sinal
                    const auto [erro, valLoad] = bus.load(vAddress, width, valSigned);

                    if (erro == MMU_OK) {

                        // FIXME: falta a trava do memoria
                        const auto [erro, _] = bus.store(vAddress, width, val_rs2);

                        if (erro == MMU_OK) { // FIXME: implementar corretamente

                            // val_rs2 = valLoad;
                            regs[rs2] = valLoad;

                            return WriteBackData{rd, valLoad, true};
                        }
                    }
                }
            }
        }

        controller.trapException(Trap(val_rs1, MCause::LoadAccessFault, opcode)); // FIXME: codigo d erro
        return WriteBackData{0, 0, false};
    }
};
