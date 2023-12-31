#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeU : public InstructionType {
  private:
    uint8_t rd{0};
    int32_t imm{0}, pc{0};

  public:
    InstructionTypeU(const OpCodeSet& o, const uint32_t& i, const uint32_t pc) : InstructionType(o), pc(pc) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void step() override {

        uint32_t address;

        switch (opcode) {
            case OpCodeSet::AUIPC:
                address = pc + static_cast<uint32_t>(imm);
                break;
            case OpCodeSet::LUI:
                address = imm;
                break;
            default:
                break;
        }

        // TODO: implementar a chamada do writeback carregando o address no apontado por rd
    }
};
