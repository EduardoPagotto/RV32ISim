#pragma once
#include "Debug.hpp"
#include "InstructionType.hpp"

class InstructionTypeI : public InstructionType {
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
    InstructionTypeI(const uint32_t& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {

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
                load();
                break;
            case OPC_ULAI:
                ulai();
                break;
            case OPC_JALR:
                jalr(controller);
                break;
            default:
                // TODO: implementar erro
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        if (isLoadOpp) {

            const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                      (width == MemoryAccessWidth::HalfWord && address & 0b01));

            if (isUnaligned) {
                controller.trapException(Trap(address, MCause::LoadAddressMisaligned, opcode));
                return WriteBackData{0, 0, false};
            }

            auto retVal = bus.load(address, width, valSigned);
            if (retVal.has_value()) {
                address = retVal.value();
            } else {
                controller.trapException(Trap(address, MCause::LoadAccessFault, opcode));
                return WriteBackData{0, 0, false};
            }
        }

        // Debug::printAddress(address); // TODO: Melhorar o print

        return WriteBackData{rd, address, true};
    }

  private:
    void load() {

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
                throw std::string("Acesso desconhecido");
                break;
        }
        std::cout << Debug::alias[rd] << ", " << imm << "(" << Debug::alias[rs1] << ")";
    }

    void ulai() {

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
                // address = ((imm & 0xf00) == 0) ? val_rs1 >> imm : val_rs1 >> (imm & 0x1f);
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
                throw std::string("Opp desconhecido");
                break;
        }
    }

    void jalr(Controller& controller) {
        std::cout << "JALR  " << Debug::alias[rd] << ", " << Debug::alias[rs1] << ", " << imm;
        address = controller.getPcplus4();
        controller.setBranchAddress(val_rs1 + imm);
    }
};