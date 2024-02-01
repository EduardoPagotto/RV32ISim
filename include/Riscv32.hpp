#pragma once
#include "Bus.hpp"
#include "Debug.hpp"
#include "InstructionTypeAtomic.hpp"
#include "InstructionTypeB.hpp"
#include "InstructionTypeCSR.hpp"
#include "InstructionTypeI.hpp"
#include "InstructionTypeInt.hpp"
#include "InstructionTypeJ.hpp"
#include "InstructionTypeR.hpp"
#include "InstructionTypeS.hpp"
#include "InstructionTypeU.hpp"
#include "mmu/MMU.hpp"

class Riscv32 {
  private:
    uint32_t x[32] = {0}; // CPU
    Controller controller;

  public:
    Riscv32(const uint32_t& startupAddr) { controller.setStartUpAddr(startupAddr); }
    virtual ~Riscv32() = default;

    void reset() { controller.reset(); }

    bool step(Bus& bus, MMU& mmu) {

        if (controller.noWaitTrap()) {
            controller.step();

            // FETCH
            const auto result = mmu.getPhysicalAddress(controller.getPC(), 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER);
            if (std::get<0>(result) == MMU_OK) {
                const auto result2 = bus.load(std::get<1>(result), MemoryAccessWidth::Word);

                if (std::get<0>(result2) == MMU_OK) {

                    const int32_t instr = std::get<1>(result2);
                    Debug::printAsHex(controller.getPC(), instr);

                    // DECODE
                    InstructionType* pipeline = this->decode(instr);
                    if (pipeline == nullptr) {
                        controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, (instr & 0x7f)));
                        return true;
                    }

                    // EXECUTE
                    pipeline->execute(controller);

                    // MEMORY
                    WriteBackData w = pipeline->memoryAccess(bus, mmu, controller);

                    // WRITEBACK
                    pipeline->writeBack(w, x);

                    Debug::newline();

                    delete pipeline;
                    pipeline = nullptr;
                } else {
                    // FIXME: Trap!!!
                    throw std::string("Fora da memoria: " + Debug::int_to_hex(controller.getPC()));
                }

            } else {
                // FIXME: Trap!!!
                throw std::string("Fora da memoria: " + Debug::int_to_hex(controller.getPC()));
            }
        }

        return true;
    }

  private:
    InstructionType* decode(const uint32_t& i) {

        if (controller.getResetSignal()) {
            // TODO: implementar
        } else {
            uint32_t opcode = (i & 0x7f);
            switch (opcode) {
                case OPC_ULA:
                    return new InstructionTypeR(opcode, i, x); // Instrucoes tipo R
                case OPC_LOAD:
                case OPC_ULAI:
                case OPC_JALR:
                    return new InstructionTypeI(opcode, i, x); // Instrucoes tipo I
                case OPC_AUIPC:
                case OPC_LUI:
                    return new InstructionTypeU(opcode, i); // Instrucoes tipo U
                case OPC_SAVE:
                    return new InstructionTypeS(opcode, i, x); // Instrucoes tipo S
                case OPC_BRANCH:
                    return new InstructionTypeB(opcode, i, x); // Instrucoes tipo B
                case OPC_JAL:
                    return new InstructionTypeJ(opcode, i); // Instrucoes tipo J
                    // case OPC_FENCE: // TODO: ler doc
                    //     break;
                case OPC_ATOMIC:
                    return new InstructionTypeAtomic(opcode, i, x); // RV32A (Atomic instrution)
                case OPC_SYSTEM:
                    if (InstructionType::calcFunct3(i) == 0) {
                        return new InstructionTypeInt(i, x);
                    } else {
                        return new InstructionTypeCSR(i, x);
                    }
                default:
                    break;
            }
        }
        return nullptr;
    }
};
