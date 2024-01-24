#pragma once
#include "InstructionType.hpp"

class InstructionTypeInt : public InstructionType {

  public:
    InstructionTypeInt(const uint32_t& o, uint32_t* x) : InstructionType(o) {}

    virtual void execute(Controller& controller) override {

        switch (opcode) {
            case OPC_EBREAK:
                std::cout << "EBREAK";
                controller.trapException(Trap(controller.getPC(), MCause::Breakpoint, 0));
                break;

            case OPC_ECALL:
                std::cout << "ECALL ";
                controller.trapException(Trap(controller.getPC(), MCause::EnvironmentCallFromMMode, 0));
                break;

            case OPC_SRET:
                std::cout << "SRET  ";
                controller.trapReturn(); // FIXME: so pelo root
                break;
            case OPC_MRET:
                std::cout << "MRET  ";
                controller.trapReturn();
                break;
            case OPC_WFI:
                std::cout << "WFI   ";
                // TODO: implementar loop infinito com processador em halt
                break;
            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {
        return WriteBackData{0, 0, false};
    }
};