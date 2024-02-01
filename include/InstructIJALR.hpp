#pragma once
#include "Debug.hpp"
#include "Instruct.hpp"

class InstructIJALR : public Instruct {
  private:
    uint8_t rd{0}, rs1{0};
    int32_t imm{0};
    uint32_t val_rs1{0};

    //  saida
    uint32_t address{0};

  public:
    InstructIJALR(const uint32_t& o, const uint32_t& i, uint32_t* x) : Instruct(o) {

        rd = calcRd(i);
        rs1 = calcRs1(i);
        imm = i >> 20;

        // Negativar
        if (i & 0x80000000) {
            imm |= 0xFFFFF000;
        }

        val_rs1 = x[rs1];
    }

    virtual void execute(Controller& controller) override {

        std::cout << "JALR  " << Debug::alias[rd] << ", " << imm << "(" << Debug::alias[rs1] << ")  PC <- "
                  << Debug::int_to_hex(val_rs1 + imm) << " ";
        address = controller.getPcplus4();
        controller.setBranchAddress(val_rs1 + imm);
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{rd, address, true};
    }
};