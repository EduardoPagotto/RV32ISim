#pragma once
#include "Instruct.hpp"

// OpCode Interrupt
#define OPC_ECALL 0b00000000000000000000000001110011  // syscall
#define OPC_EBREAK 0b00000000000100000000000001110011 // syscall
#define OPC_SRET 0b00010000001000000000000001110011   // trap return
#define OPC_MRET 0b00110000001000000000000001110011   // trap return
#define OPC_WFI 0b00010000010100000000000001110011    // Wait for Interruption

class InstructInterrupt : public Instruct {

  public:
    InstructInterrupt(const uint32_t& o, uint32_t* x) : Instruct(o) {}

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