#pragma once
#include "Instruct.hpp"

class InstructS : public Instruct {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};

    uint32_t val_rs1{0}, val_rs2{0};

    // memoryAcess data
    uint32_t address;
    MemoryAccessWidth width;

  public:
    InstructS(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        rs2 = calcRs2(i);
        imm = ((i >> 25) << 5) | ((i >> 7) & 0x1f);

        val_rs1 = x[rs1];
        val_rs2 = x[rs2];
    }

    virtual void execute(Controller& controller) override {

        address = val_rs1 + imm;
        switch (funct3) {
            case 0x0: // SB
                std::cout << "SB    ";
                width = MemoryAccessWidth::Byte;
                break;

            case 0x1: // SH
                std::cout << "SH    ";
                width = MemoryAccessWidth::HalfWord;
                break;

            case 0x2: // SW
                std::cout << "SW    ";
                width = MemoryAccessWidth::Word;
                break;

            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        };

        std::cout << Debug::alias[rs2] << ", " << imm << "(" << Debug::alias[rs1] << ")";
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {

        const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                  (width == MemoryAccessWidth::HalfWord && address & 0b01));

        if (isUnaligned) {
            controller.trapException(Trap(address, MCause::StoreAMOAddressMisaligned, opcode));
            return WriteBackData{0, 0, false};
        }

        const auto [erro, vAddress] = mmu.getPhysicalAddress(address, 0, MMU_ACC_WRITE | MMU_ACC_SUPER);
        if (erro == MMU_OK) {

            auto [error, _] = bus.store(vAddress, width, val_rs2);
            if (error == MMU_OK) {
                return WriteBackData{0, 0, false};
            }
        }

        controller.trapException(Trap(controller.getPC(), MCause::StoreAMOAccessFault, opcode));
        return WriteBackData{0, 0, false};
    }
};