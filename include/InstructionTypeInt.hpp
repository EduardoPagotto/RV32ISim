#pragma once
#include "InstructionType.hpp"

class InstructionTypeInt : public InstructionType {
  private:
    // uint8_t rd{0}, rs1{0};
    // int32_t imm{0};
    // uint32_t val_rs1{0};

  public:
    InstructionTypeInt(const uint32_t& o, uint32_t* x) : InstructionType(o) {

        // rd = calcRd(i);
        // rs1 = calcRs1(i);
        // val_rs1 = x[rs1];
    }

    virtual void execute(Controller& controller) override {

        // returnFromTrap = data.decode.imm32 == 0x302;
        switch (opcode) {
            case OPC_EBREAK:
                std::cout << "Ebreak";
                controller.trapException(Trap(controller.getPC(), MCause::Breakpoint, 0));
                break;

            case OPC_ECALL:
                std::cout << "Ecall\t";
                controller.trapException(Trap(controller.getPC(), MCause::EnvironmentCallFromMMode, 0));
                break;

            case OPC_SRET:
                std::cout << "sret  ";
                break;
            case OPC_MRET:
                std::cout << "mret  \t";
                // csr->trapReturn();
                break;
            case OPC_WFI:
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