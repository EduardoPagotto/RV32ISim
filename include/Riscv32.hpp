#pragma once
#include "Bus.hpp"
#include "Debug.hpp"
#include "InstructionTypeB.hpp"
#include "InstructionTypeCSR.hpp"
#include "InstructionTypeI.hpp"
#include "InstructionTypeInt.hpp"
#include "InstructionTypeJ.hpp"
#include "InstructionTypeR.hpp"
#include "InstructionTypeS.hpp"
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

        uint32_t instr = this->fetch();
        Debug::printAsHex(controller.getPC(), instr);

        InstructionType* pipeline = this->decode(instr);

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

        Debug::newline();

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
            OpCode opcode = static_cast<OpCode>(i & 0x7f);
            switch (opcode) {
                case OpCode::ULA:
                    return new InstructionTypeR(opcode, i, x); // Instrucoes tipo R *
                case OpCode::LOAD:
                case OpCode::ULAI:
                case OpCode::JALR:
                    return new InstructionTypeI(opcode, i, x); // Instrucoes tipo I *
                case OpCode::AUIPC:
                case OpCode::LUI:
                    return new InstructionTypeU(opcode, i); // Instrucoes tipo U *
                case OpCode::SAVE:
                    return new InstructionTypeS(opcode, i, x); // Instrucoes tipo S
                case OpCode::BRANCH:
                    return new InstructionTypeB(opcode, i, x); // Instrucoes tipo B *
                case OpCode::JAL:
                    return new InstructionTypeJ(opcode, i); // Instrucoes tipo J *
                // case OpCode::FENCE: // TODO: ler doc
                //     break;
                case OpCode::SYSTEM:
                    if (InstructionType::calcFunct3(i) == 0) {
                        return new InstructionTypeInt(opcode, i, x);
                    } else {
                        return new InstructionTypeCSR(opcode, i, x);
                    }

                    break;
                default:
                    throw std::string("Opcode desconhecido");
                    break;
            }
        }
        return nullptr;
    }
};
