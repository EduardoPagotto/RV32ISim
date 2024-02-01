#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructILoad : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0};
    int32_t imm{0};
    uint32_t val_rs1{0};

    //  saida
    uint32_t address{0};
    MemoryAccessWidth width;
    bool valSigned{false};

  public:
    InstructILoad(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {

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

        address = val_rs1 + imm;
        switch (funct3) {
            case 0x0: // LB
                std::cout << "LB    ";
                width = MemoryAccessWidth::Byte;
                valSigned = true;
                break;

            case 0x1: // LH
                std::cout << "LH    ";
                width = MemoryAccessWidth::HalfWord;
                valSigned = true;
                break;

            case 0x2: // LW
                std::cout << "LW    ";
                width = MemoryAccessWidth::Word;
                valSigned = false;
                break;

            case 0x4: // LBU
                std::cout << "LBU   ";
                width = MemoryAccessWidth::Byte;
                valSigned = false;
                break;

            case 0x5: // LHU
                std::cout << "LHU   ";
                width = MemoryAccessWidth::HalfWord;
                valSigned = false;
                break;
            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
        std::cout << Debug::alias[rd] << ", " << imm << "(" << Debug::alias[rs1] << ")";
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {

        const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                  (width == MemoryAccessWidth::HalfWord && address & 0b01));

        if (isUnaligned) {
            controller.trapException(Trap(address, MCause::LoadAddressMisaligned, opcode));
            return WriteBackData{0, 0, false};
        }

        const auto result = mmu.getPhysicalAddress(address, 0, MMU_ACC_READ | MMU_ACC_SUPER);
        if (std::get<0>(result) == MMU_OK) {
            const auto result2 = bus.load(std::get<1>(result), MemoryAccessWidth::Word, valSigned);
            if (std::get<0>(result2) == MMU_OK)
                return WriteBackData{rd, std::get<1>(result2), true};
        }

        controller.trapException(Trap(address, MCause::LoadAccessFault, opcode));
        return WriteBackData{0, 0, false};
    }
};