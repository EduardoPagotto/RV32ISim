#pragma once
#include "InstructionType.hpp"
#include <iostream>
#include <stdint.h>

class InstructionTypeSys : public InstructionType {
  private:
    uint8_t funct3{0}, rd{0}, rs1{0};
    int32_t imm;
    OpCodeSetSystem opcodeSys;
    uint32_t val_rs1{0};

  public:
    InstructionTypeSys(const OpCodeSet& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {
        funct3 = calcFunct3(i);
        rd = calcRd(i);
        rs1 = calcRs1(i);
        val_rs1 = x[rs1];

        imm = i >> 20;

        if (funct3 == 0) {
            switch (imm) {
                case 0x0:
                    opcodeSys = OpCodeSetSystem::ECALL;
                    break;

                case 0x1:
                    opcodeSys = OpCodeSetSystem::EBREAK;
                    break;

                case 0x102:
                    opcodeSys = OpCodeSetSystem::SRET;
                    break;

                case 0x302:
                    opcodeSys = OpCodeSetSystem::MRET;
                    break;

                case 0x105:
                    opcodeSys = OpCodeSetSystem::WFI;
                    break;

                default:
                    throw std::string("Opcode desconhecido");
                    break;
            }
        } else {
            opcodeSys = static_cast<OpCodeSetSystem>(funct3); // CSR's
        }
    }

    virtual void execute(Controller& controller) override {

        // returnFromTrap = data.decode.imm32 == 0x302;
        switch (opcodeSys) {
            case OpCodeSetSystem::EBREAK:
                std::cout << "Ebreak";
                // csr->trapException(Trap(controller.getPC(), MCause::Breakpoint, 0));
                break;

            case OpCodeSetSystem::ECALL:
                std::cout << "Ecall\t";
                // csr->trapException(Trap(controller.getPC(), MCause::EnvironmentCallFromMMode, 0));
                break;

            case OpCodeSetSystem::SRET:
                std::cout << "sret  ";
                break;
            case OpCodeSetSystem::MRET:
                std::cout << "mret  \t";
                // csr->trapReturn();
                break;
            case OpCodeSetSystem::WFI:
                std::cout << "wfi   ";
                // throw std::string("WFI!!!!!");
                break;
            case OpCodeSetSystem::CSRRC:
                std::cout << "csrrc ";
                break;
            case OpCodeSetSystem::CSRRCI:
                std::cout << "csrrci";
                break;
            case OpCodeSetSystem::CSRRS:
                std::cout << "csrrs ";
                break;
            case OpCodeSetSystem::CSRRSI:
                std::cout << "csrrsi";
                break;
            case OpCodeSetSystem::CSRRW:
                std::cout << "csrrw " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm) << ", "
                          << Debug::alias[rs1];
                break;
            case OpCodeSetSystem::CSRRWI:
                std::cout << "csrrwi";
                // data.address = data.decode.imm32;
                break;
            case OpCodeSetSystem::INVALID:
                throw std::string("System Opcode desconhecido");
                break;
        }
        std::cout << "\t\t# ";
        std::cout << '\n';
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        bool isValid = false; // true; // TODO: tirar depois quando for testar
        switch (opcodeSys) {
            case OpCodeSetSystem::INVALID:
                break;

            case OpCodeSetSystem::CSRRC:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.valueRS1)));
                break;

            case OpCodeSetSystem::CSRRCI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.decode.rs1)));
                break;

            case OpCodeSetSystem::CSRRS:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.valueRS1);
                break;

            case OpCodeSetSystem::CSRRSI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.decode.rs1);
                break;

            case OpCodeSetSystem::CSRRW:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.valueRS1);
                break;

            case OpCodeSetSystem::CSRRWI:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.decode.rs1);
                break;
            default:
                break;
        }

        return WriteBackData{0, 0, isValid};
    }
};