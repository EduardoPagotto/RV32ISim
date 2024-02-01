#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructU : public Instruct {
  private:
    uint8_t rd{0};
    int32_t imm{0};
    uint32_t address{0};

  public:
    InstructU(const uint32_t& o, const uint32_t& i) : Instruct(o) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void execute(Controller& controller) override {

        switch (opcode) {
            case OPC_AUIPC: // AUIPC
                std::cout << "AUIPC " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
                address = controller.getPC() + static_cast<uint32_t>(imm);
                break;
            case OPC_LUI: // LUI
                std::cout << "LUI   " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
                address = imm;
                break;
            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
