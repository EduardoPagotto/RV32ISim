#pragma once
#include "Debug.hpp"
#include "InstructionType.hpp"

class InstructionTypeU : public InstructionType {
  private:
    uint8_t rd{0};
    int32_t imm{0};
    uint32_t address{0};

  public:
    InstructionTypeU(const uint32_t& o, const uint32_t& i) : InstructionType(o) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void execute(Controller& controller) override {

        switch (opcode) {
            case OPC_AUIPC: // AUIPC
                std::cout << "auipc " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
                address = controller.getPC() + static_cast<uint32_t>(imm);
                break;
            case OPC_LUI: // LUI
                std::cout << "lui   " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
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
