#pragma once
#include "Bus.hpp"
#include "InstructionTypeB.hpp"
#include "InstructionTypeI.hpp"
#include "InstructionTypeJ.hpp"
#include "InstructionTypeR.hpp"
#include "InstructionTypeS.hpp"
#include "InstructionTypeU.hpp"

class Riscv32 {
  private:
    uint32_t x[32] = {0};    // CPU
    uint32_t startupAddr{0}; // CPU

    bool branchAddressValid{false}; // CSR
    bool resetSignal{false};        // CSR
    uint32_t branchAddress{0};      // CSR

    uint32_t pc{0};      // fetch
    uint32_t pcPlus4{0}; // fetch

    Bus bus;

  public:
    Riscv32(const uint32_t& startupAddr) : startupAddr(startupAddr) {}
    virtual ~Riscv32() {}
    Bus& getBus() { return bus; }
    void reset() {
        this->branchAddressValid = true;
        this->branchAddress = startupAddr;
    }
    bool step() {

        this->fetch();

        return true;
    }

  private:
    void fetch() {

        this->pc = this->branchAddressValid ? this->branchAddress : this->pcPlus4;
        this->pcPlus4 = this->pc + 4;

        auto v = bus.load(this->pc, MemoryAccessWidth::Word);
        if (v.has_value()) {
            this->decode(v.value());
        } else {
            throw std::string("Fora da memoria");
        }
    }

    void decode(const uint32_t& i) {
        if (resetSignal) {
            // TODO: implementar
        } else {

            OpCodeSet opcode = static_cast<OpCodeSet>(i & 0x7f);

            InstructionType* executer = nullptr;

            switch (opcode) {
                case OpCodeSet::ULA:
                    executer = new InstructionTypeR(opcode, i); // Instrucoes tipo R
                    break;
                case OpCodeSet::LOAD:
                case OpCodeSet::ULAI:
                case OpCodeSet::JALR:
                case OpCodeSet::FENCE:                          // FIXME: tipo diferente depois separa
                    executer = new InstructionTypeI(opcode, i); // Instrucoes tipo I
                    break;
                case OpCodeSet::AUIPC:
                case OpCodeSet::LUI:
                    executer = new InstructionTypeU(opcode, i, pc); // Instrucoes tipo U
                    break;
                case OpCodeSet::SAVE:
                    executer = new InstructionTypeS(opcode, i, x); // Instrucoes tipo S
                    break;
                case OpCodeSet::BRANCH:
                    executer = new InstructionTypeB(opcode, i); // Instrucoes tipo B
                    break;
                case OpCodeSet::JAL:
                    executer = new InstructionTypeJ(opcode, i); // Instrucoes tipo J
                    break;
                case OpCodeSet::SYSTEM:
                    // this->imm32 = iImm;
                    // if (this->funct3 == 0) {

                    //     switch (this->imm32) {
                    //         case 0x0:
                    //             this->opcodeSys = OpCodeSetSystem::ECALL;
                    //             break;

                    //         case 0x1:
                    //             this->opcodeSys = OpCodeSetSystem::EBREAK;
                    //             break;

                    //         case 0x102:
                    //             this->opcodeSys = OpCodeSetSystem::SRET;
                    //             break;

                    //         case 0x302:
                    //             this->opcodeSys = OpCodeSetSystem::MRET;
                    //             break;

                    //         case 0x105:
                    //             this->opcodeSys = OpCodeSetSystem::WFI;
                    //             break;

                    //         default:
                    //             throw std::string("Opcode desconhecido");
                    //             break;
                    //     }

                    // } else {
                    //     this->opcodeSys = static_cast<OpCodeSetSystem>(this->funct3); // CSR's
                    // }
                    break;

                default:
                    throw std::string("Opcode desconhecido");
                    break;
            }

            executer->step();
        }
    }
};
