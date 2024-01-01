#pragma once
#include "InstructionType.hpp"
#include <iostream>
#include <stdint.h>

class InstructionTypeInt : public InstructionType {
  private:
    // uint8_t rd{0}, rs1{0};
    int32_t imm{0};
    OpCodeInt opcodeInt;
    // uint32_t val_rs1{0};

  public:
    InstructionTypeInt(const OpCode& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {

        // rd = calcRd(i);
        // rs1 = calcRs1(i);
        // val_rs1 = x[rs1];

        imm = i >> 20;
        switch (imm) {
            case 0x0:
                opcodeInt = OpCodeInt::ECALL;
                break;

            case 0x1:
                opcodeInt = OpCodeInt::EBREAK;
                break;

            case 0x102:
                opcodeInt = OpCodeInt::SRET;
                break;

            case 0x302:
                opcodeInt = OpCodeInt::MRET;
                break;

            case 0x105:
                opcodeInt = OpCodeInt::WFI;
                break;

            default:
                throw std::string("Opcode desconhecido");
                break;
        }
    }

    virtual void execute(Controller& controller) override {

        // returnFromTrap = data.decode.imm32 == 0x302;
        switch (opcodeInt) {
            case OpCodeInt::EBREAK:
                std::cout << "Ebreak";
                // csr->trapException(Trap(controller.getPC(), MCause::Breakpoint, 0));
                break;

            case OpCodeInt::ECALL:
                std::cout << "Ecall\t";
                // csr->trapException(Trap(controller.getPC(), MCause::EnvironmentCallFromMMode, 0));
                break;

            case OpCodeInt::SRET:
                std::cout << "sret  ";
                break;
            case OpCodeInt::MRET:
                std::cout << "mret  \t";
                // csr->trapReturn();
                break;
            case OpCodeInt::WFI:
                std::cout << "wfi   ";
                // throw std::string("WFI!!!!!");
                break;
            default:
                break;
        }
        std::cout << "\t\t# ";
        std::cout << '\n';
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        bool isValid = false; // true; // TODO: tirar depois quando for testar
        return WriteBackData{0, 0, isValid};
    }
};