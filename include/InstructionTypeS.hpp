#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeS : public InstructionType {
  private:
    uint8_t rs1{0}, rs2{0}, funct3{0};
    int32_t imm{0};

    uint32_t val_rs1{0}, val_rs2{0};

    // memoryAcess data
    uint32_t address;
    MemoryAccessWidth width;

  public:
    InstructionTypeS(const OpCodeSet& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
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
                width = MemoryAccessWidth::Byte;
                break;

            case 0x1: // SH
                width = MemoryAccessWidth::HalfWord;
                break;

            case 0x2: // SW
                width = MemoryAccessWidth::Word;
                break;
        };

        // TODO: implementar a chamada para accesMemory
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        // const ExecuteData& d = execute->get();

        // data = d;
        // data.isValid = false;
        // data.value = 0;

        const bool isUnaligned = ((width == MemoryAccessWidth::Word && address & 0b11) ||
                                  (width == MemoryAccessWidth::HalfWord && address & 0b01));

        // TODO: implementar trap
        // if (isUnaligned) {
        //     csr->trapException(Trap(d.address, MCause::StoreAMOAddressMisaligned, d.decode.fetch.instr));
        //     return;
        // }

        bus.store(address, width, val_rs2);
        // csr->prt.printRegtoMemory(d.decode.rs2, d.valueRS2, d.address); // TODO: Melhorar o print
        return WriteBackData{0, 0, false};
    }
};