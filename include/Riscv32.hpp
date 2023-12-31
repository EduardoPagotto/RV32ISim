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

        InstructionType* pipeline = this->decode(this->fetch());

        pipeline->execute(controller);
        WriteBackData w = pipeline->memoryAccess(bus, controller);

        // WriteBack
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

    InstructionType* decode(const uint32_t& i) {

        if (controller.getResetSignal()) {
            // TODO: implementar
        } else {

            OpCodeSet opcode = static_cast<OpCodeSet>(i & 0x7f);
            switch (opcode) {
                case OpCodeSet::ULA:
                    return new InstructionTypeR(opcode, i, x); // Instrucoes tipo R *
                case OpCodeSet::LOAD:
                case OpCodeSet::ULAI:
                case OpCodeSet::JALR:
                    return new InstructionTypeI(opcode, i, x); // Instrucoes tipo I *
                case OpCodeSet::AUIPC:
                case OpCodeSet::LUI:
                    return new InstructionTypeU(opcode, i); // Instrucoes tipo U *
                case OpCodeSet::SAVE:
                    return new InstructionTypeS(opcode, i, x); // Instrucoes tipo S *
                case OpCodeSet::BRANCH:
                    return new InstructionTypeB(opcode, i, x); // Instrucoes tipo B *
                case OpCodeSet::JAL:
                    return new InstructionTypeJ(opcode, i); // Instrucoes tipo J *
                // case OpCodeSet::FENCE: // TODO: ler doc
                //     break;
                case OpCodeSet::SYSTEM:
                    return new InstructionTypeSys(opcode, i);
                default:
                    throw std::string("Opcode desconhecido");
                    break;
            }
        }
        return nullptr;
    }
};
