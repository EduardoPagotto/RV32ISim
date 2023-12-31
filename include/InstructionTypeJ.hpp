#pragma once
#include "InstructionType.hpp"
#include "defs.hpp"

class InstructionTypeJ : public InstructionType {
  private:
    uint8_t rd{0};
    int32_t imm{0};

    uint32_t address{0};

  public:
    InstructionTypeJ(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        rd = calcRd(i);
        imm = ((i >> 30) << 20) |          //  instr[20]
              (((i >> 12) & 0xff) << 12) | //  instr[19:12]
              (((i >> 20) & 0x1) << 11) |  //  instr[11]
              (((i >> 21) & 0x3ff) << 1);  //  instr[10:1]
    }

    virtual void execute(Controller& controller) override {

        // JAL
        address = controller.getPcplus4();
        controller.setBranchAddress(controller.getPC() + imm);

        // TODO: manda dados para memory com addres como valor no RD
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};