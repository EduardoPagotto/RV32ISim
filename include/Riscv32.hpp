#pragma once
#include "Bus.hpp"
#include "InstructionTypeB.hpp"
#include "InstructionTypeI.hpp"
#include "InstructionTypeJ.hpp"
#include "InstructionTypeR.hpp"
#include "InstructionTypeS.hpp"
#include "InstructionTypeSys.hpp"
#include "InstructionTypeU.hpp"

class Riscv32 {
  private:
    uint32_t x[32] = {0}; // CPU
    Bus bus;
    Controller controller;

  public:
    Riscv32(const uint32_t& startupAddr) { controller.setStartUpAddr(startupAddr); }

    virtual ~Riscv32() {}
    Bus& getBus() { return bus; }
    void reset() { controller.reset(); }

    bool step() {

        uint32_t instruction = this->fetch();
        this->decode(instruction);

        return true;
    }

  private:
    uint32_t fetch() {

        controller.step();

        auto v = bus.load(controller.getPC(), MemoryAccessWidth::Word);
        if (v.has_value()) {
            return v.value();
        } else {
            throw std::string("Fora da memoria");
        }
    }

    void decode(const uint32_t& i) {
        if (controller.getResetSignal()) {
            // TODO: implementar
        } else {

            OpCodeSet opcode = static_cast<OpCodeSet>(i & 0x7f);

            InstructionType* pipeline = nullptr;

            switch (opcode) {
                case OpCodeSet::ULA:
                    pipeline = new InstructionTypeR(opcode, i, x); // Instrucoes tipo R *
                    break;
                case OpCodeSet::LOAD:
                case OpCodeSet::ULAI:
                case OpCodeSet::JALR:
                    pipeline = new InstructionTypeI(opcode, i, x); // Instrucoes tipo I *
                    break;
                case OpCodeSet::AUIPC:
                case OpCodeSet::LUI:
                    pipeline = new InstructionTypeU(opcode, i); // Instrucoes tipo U *
                    break;
                case OpCodeSet::SAVE:
                    pipeline = new InstructionTypeS(opcode, i, x); // Instrucoes tipo S *
                    break;
                case OpCodeSet::BRANCH:
                    pipeline = new InstructionTypeB(opcode, i, x); // Instrucoes tipo B *
                    break;
                case OpCodeSet::JAL:
                    pipeline = new InstructionTypeJ(opcode, i); // Instrucoes tipo J *
                    break;
                // case OpCodeSet::FENCE: // TODO: ler doc
                //     break;
                case OpCodeSet::SYSTEM:
                    pipeline = new InstructionTypeSys(opcode, i);
                default:
                    throw std::string("Opcode desconhecido");
                    break;
            }

            pipeline->execute(controller);
            WriteBackData w = pipeline->memoryAccess(bus, controller);
            if (w.isValid) {
                if (w.rd != 0) {
                    x[w.rd] = w.value;
                    // csr->prt.printRegVal(m.execute.decode.rd, m.value); // TODO: Melhorar o print
                } else {
                    x[0] = 0;
                }
            }

            delete pipeline;
            pipeline = nullptr;
        }
    }
};
