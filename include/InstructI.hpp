#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructI : public Instruct {
  private:
    uint8_t rd{0}, funct3{0}, rs1{0};
    int32_t imm{0};
    uint32_t val_rs1{0};

    bool isLoadOpp{false};
    // saida
    uint32_t address{0};
    MemoryAccessWidth width;
    bool valSigned{false};

  public:
    InstructI(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {

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
        switch (opcode) {
            case OPC_LOAD:
                load(controller);
                break;
            case OPC_ULAI:
                ulai(controller);
                break;
            case OPC_JALR:
                jalr(controller);
                break;
            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {

        if (isLoadOpp) {

            const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                      (width == MemoryAccessWidth::HalfWord && address & 0b01));

            if (isUnaligned) {
                controller.trapException(Trap(address, MCause::LoadAddressMisaligned, opcode));
                return WriteBackData{0, 0, false};
            }

            const auto result = mmu.getPhysicalAddress(address, 0, MMU_ACC_READ | MMU_ACC_SUPER);
            if (std::get<0>(result) == MMU_OK) {
                const auto result2 = bus.load(std::get<1>(result), MemoryAccessWidth::Word);
                if (std::get<0>(result2) == MMU_OK)
                    return WriteBackData{rd, std::get<1>(result2), true};
            }

            controller.trapException(Trap(address, MCause::LoadAccessFault, opcode));
            return WriteBackData{0, 0, false};
        }

        return WriteBackData{rd, address, true};
    }

  private:
    void load(Controller& controller) {

        address = val_rs1 + imm;
        switch (funct3) {
            case 0x0: // LB
                std::cout << "LB    ";
                width = MemoryAccessWidth::Byte;
                valSigned = true;
                isLoadOpp = true;
                break;

            case 0x1: // LH
                std::cout << "LH    ";
                width = MemoryAccessWidth::HalfWord;
                valSigned = true;
                isLoadOpp = true;
                break;

            case 0x2: // LW
                std::cout << "LW    ";
                width = MemoryAccessWidth::Word;
                valSigned = false;
                isLoadOpp = true;
                break;

            case 0x4: // LBU
                std::cout << "LBU   ";
                width = MemoryAccessWidth::Byte;
                valSigned = false;
                isLoadOpp = true;
                break;

            case 0x5: // LHU
                std::cout << "LHU   ";
                width = MemoryAccessWidth::HalfWord;
                valSigned = false;
                isLoadOpp = true;
                break;
            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
        std::cout << Debug::alias[rd] << ", " << imm << "(" << Debug::alias[rs1] << ")";
    }

    void ulai(Controller& controller) {

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

    void jalr(Controller& controller) {
        std::cout << "JALR  " << Debug::alias[rd] << ", " << imm << "(" << Debug::alias[rs1] << ")  PC <- "
                  << Debug::int_to_hex(val_rs1 + imm) << " ";
        address = controller.getPcplus4();
        controller.setBranchAddress(val_rs1 + imm);
    }
};