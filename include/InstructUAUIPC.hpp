#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructUAUPC : public Instruct {
  private:
    uint8_t rd{0};
    int32_t imm{0};
    uint32_t address{0};

  public:
    InstructUAUPC(const uint32_t& o, const uint32_t& i) : Instruct(o) {
        rd = calcRd(i);
        imm = ((i >> 12) & 0xfffff) << 12;
    }

    virtual void execute(Controller& controller) override {
        std::cout << "AUIPC " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm);
        address = controller.getPC() + static_cast<uint32_t>(imm);
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};
