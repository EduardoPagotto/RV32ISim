#pragma once
#include "Instruct.hpp"

class InstructJ : public Instruct {
  private:
    uint8_t rd{0};
    int32_t imm{0};

    uint32_t address{0};

  public:
    InstructJ(const uint32_t& o, const uint32_t& i) : Instruct(o) {
        rd = calcRd(i);
        imm = ((i >> 30) << 20) |          //  instr[20]
              (((i >> 12) & 0xff) << 12) | //  instr[19:12]
              (((i >> 20) & 0x1) << 11) |  //  instr[11]
              (((i >> 21) & 0x3ff) << 1);  //  instr[10:1]

        // Negativar
        if (i & 0x80000000) {
            imm |= 0xFFFFF000;
        }
    }

    virtual void execute(Controller& controller) override {
        // JAL
        uint32_t temp = controller.getPC() + imm;
        std::cout << "JAL   " << Debug::alias[rd] << ", " << Debug::int_to_hex(temp);
        address = controller.getPcplus4();
        controller.setBranchAddress(temp);
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};