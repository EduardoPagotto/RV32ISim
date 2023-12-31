#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

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
    InstructionTypeI(const OpCodeSet& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {

        rd = calcRd(i);
        funct3 = calcFunct3(i);
        rs1 = calcRs1(i);
        imm = i >> 20; // FIXME: ver se precisa negativar!!!!

        val_rs1 = x[rs1];
    }

    virtual void execute(Controller& controller) override {
        switch (opcode) {
            case OpCodeSet::LOAD:
                load();
                break;
            case OpCodeSet::ULAI:
                ulai();
                break;
            case OpCodeSet::JALR:
                jalr(controller);
                break;
            default:
                // TODO: implementar erro
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                  (width == MemoryAccessWidth::HalfWord && address & 0b01));

        if (isUnaligned) {
            // csr->trapException(Trap(d.address, MCause::LoadAddressMisaligned, d.decode.fetch.instr));
            // return;
        }

        if (isLoadOpp) {
            auto retVal = bus.load(address, width, valSigned);
            if (retVal.has_value()) {
                address = retVal.value();
            } else {
                // TODO: trap!!!
            }
        }

        // csr->prt.printAddress(d.address); // TODO: Melhorar o print

        return WriteBackData{rd, address, true};
    }

  private:
    void load() {

        address = val_rs1 + imm;
        switch (funct3) {
            case 0x0: // LB
                width = MemoryAccessWidth::Byte;
                valSigned = false;
                isLoadOpp = true;
                break;

            case 0x1: // LH
                width = MemoryAccessWidth::HalfWord;
                valSigned = false;
                isLoadOpp = true;
                break;

            case 0x2: // LW
                width = MemoryAccessWidth::Word;
                valSigned = false;
                isLoadOpp = true;
                break;

            case 0x4: // LBU
                width = MemoryAccessWidth::Byte;
                valSigned = true;
                isLoadOpp = true;
                break;

            case 0x5: // LHU
                width = MemoryAccessWidth::Byte;
                valSigned = true;
                isLoadOpp = true;
                break;
        }
    }

    void ulai() {

        switch (funct3) {
            case 0x0: // ADDI
                address = val_rs1 + imm;
                break;

            case 0x1: // SLLI
                address = val_rs1 << imm;
                break;

            case 0x2: // SLTI
                address = (val_rs1 < imm) ? 1 : 0;
                break;

            case 0x3: // SLTIU
                address = (val_rs1 < ((unsigned int)(imm & 0xfff))) ? 1 : 0;
                break;

            case 0x4: // XORI
                address = val_rs1 ^ imm;
                break;

            case 0x5: // SLRI / SRAI
                address = ((imm & 0xf00) == 0) ? val_rs1 >> imm : val_rs1 >> (imm & 0x1f);
                break;

            case 0x6: // ORI
                address = val_rs1 | imm;
                break;

            case 0x7: // ANDI
                address = val_rs1 & imm;
                break;
        }
    }

    void jalr(Controller& controller) {
        address = controller.getPcplus4();
        controller.setBranchAddress(val_rs1 + imm);
    }
};