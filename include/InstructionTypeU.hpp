#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeU : public InstructionType {
  private:
    uint8_t rd{0};
    int32_t imm{0};
    uint32_t address{0};

  public:
    InstructionTypeU(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void execute(Controller& controller) override {

        switch (opcode) {
            case OpCodeSet::AUIPC: // AUIPC
                address = controller.getPC() + static_cast<uint32_t>(imm);
                break;
            case OpCodeSet::LUI: // LUI
                address = imm;
                break;
            default:
                break;
        }

        // TODO: implementar a chamada do writeback carregando o address no apontado por rd
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
