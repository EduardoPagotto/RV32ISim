#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructAtom : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, funct5{0}, rs1{0}, rs2{0};
    uint32_t val_rs1{0}, val_rs2{0};

    bool aq{false}, rl{false};
    bool valSigned{false};
    uint32_t* regs;

    // saida
    void debug_flags() {
        if (aq)
            std::cout << ".aq";

        if (rl)
            std::cout << ".rl";
    }

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

                    valSigned = true;

                    std::cout << "LR.W";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs1];
                } break;

                case 0b00011: { // SC.W

                    std::cout << "SC.W";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", (" << Debug::alias[rs1] << "), " << Debug::alias[rs2];
                } break;

                case 0b00001: { // AMOSWAP.W

                    valSigned = true;

                    std::cout << "AMOSWAP.W";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";
                } break;

                case 0b00000: // AMOADD.W

                    // TODO: implementar
                    valSigned = true;

                    std::cout << "AMOADD.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";
                    break;

                case 0b00100: // AMOXOR.W

                    // TODO: implementar

                    std::cout << "AMOXOR.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";
                    break;

                case 0b01100: // AMOAND.W

                    // TODO: implementar

                    std::cout << "AMOAND.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";
                    break;

                case 0b01000: // AMOOR.W
                              // TODO: implementar
                    std::cout << "AMOOR.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";

                    break;

                case 0b10000: // AMOMIN.W
                              // TODO: implementar
                    std::cout << "AMOMIN.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";

                    break;

                case 0b10100: // AMOMAX.W
                              // TODO: implementar
                    std::cout << "AMOMAX.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";

                    break;

                case 0b11000: // AMOMIMU.W
                              // TODO: implementar
                    std::cout << "AMOMIMU.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";

                    break;

                case 0b11100: // AMOMAXU.W
                              // TODO: implementar
                    std::cout << "AMOMAXU.W ";
                    this->debug_flags();
                    std::cout << " " << Debug::alias[rd] << ", " << Debug::alias[rs2] << ", (" << Debug::alias[rs1]
                              << ")";

                    break;

                default:
                    // TODO: opcode ou i ?
                    controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                    break;
            }
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {

        if (val_rs1 & 0b11) {

            controller.trapException(Trap(val_rs1, MCause::LoadAddressMisaligned, opcode));
            return WriteBackData{0, 0, false};
        }

        const auto [erro, vAddress] = mmu.getPhysicalAddress(val_rs1, 0, MMU_ACC_READ | MMU_ACC_SUPER);
        if (erro == MMU_OK) {

            switch (funct5) {
                case 0b00010: // LR.W
                {
                    // FIXME: falta a trava do memoria
                    const auto [erro, value] = bus.load(vAddress, MemoryAccessWidth::Word, valSigned);
                    if (erro == MMU_OK)
                        return WriteBackData{rd, value, true};

                } break;

                case 0b00011: // SC.W
                {
                    // FIXME: falta a trava do memoria
                    auto [error, value] = bus.store(vAddress, MemoryAccessWidth::Word, val_rs2);
                    if (error == MMU_OK) { //
                        // FIXME: se trava falhar rd deve ser diferente de zero!!!!
                        return WriteBackData{rd, 0, true};
                    }

                } break;
                case 0b00001: // AMOSWAP.W
                {
                    // FIXME: falta a trava do memoria
                    const auto [erro, valLoad] = bus.load(vAddress, MemoryAccessWidth::Word, valSigned);

                    if (erro == MMU_OK) {
                        const auto [erro, _] = bus.store(vAddress, MemoryAccessWidth::Word, val_rs2);

                        if (erro == MMU_OK) { // FIXME: implementar corretamente
                            regs[rs2] = valLoad;
                            return WriteBackData{rd, valLoad, true};
                        }
                    }
                }
                case 0b00000: // AMOADD.W
                {
                    // FIXME: falta a trava do memoria
                    const auto [erro, valLoad] = bus.load(vAddress, MemoryAccessWidth::Word, valSigned);

                    if (erro == MMU_OK) {

                        const auto [erro, _] = bus.store(vAddress, MemoryAccessWidth::Word, valLoad + val_rs2);
                        if (erro == MMU_OK) // FIXME: implementar corretamente
                            return WriteBackData{rd, valLoad, true};
                    }

                } break;

                case 0b00100: // AMOXOR.W
                    break;
                case 0b01100: // AMOAND.W
                {
                    // FIXME: falta a trava do memoria
                    const auto [erro, valLoad] = bus.load(vAddress, MemoryAccessWidth::Word, valSigned);

                    if (erro == MMU_OK) {

                        const auto [erro, _] = bus.store(vAddress, MemoryAccessWidth::Word, valLoad & val_rs2);
                        if (erro == MMU_OK) // FIXME: implementar corretamente
                            return WriteBackData{rd, valLoad, true};
                    }

                } break;
                case 0b01000: // AMOOR.W
                {
                    // FIXME: falta a trava do memoria
                    const auto [erro, valLoad] = bus.load(vAddress, MemoryAccessWidth::Word, valSigned);
                    if (erro == MMU_OK) {

                        const auto [erro, _] = bus.store(vAddress, MemoryAccessWidth::Word, valLoad | val_rs2);
                        if (erro == MMU_OK) // FIXME: implementar corretamente
                            return WriteBackData{rd, valLoad, true};
                    }
                } break;
                case 0b10000: // AMOMIN.W
                    break;
                case 0b10100: // AMOMAX.W
                    break;
                case 0b11000: // AMOMIMU.W
                    break;
                case 0b11100: // AMOMAXU.W
                    break;
            }
        }

        controller.trapException(Trap(val_rs1, MCause::LoadAccessFault, opcode)); // FIXME: codigo d erro
        return WriteBackData{0, 0, false};
    }
};
