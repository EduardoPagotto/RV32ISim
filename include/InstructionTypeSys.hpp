#pragma once
#include "InstructionType.hpp"
#include <stdint.h>

class InstructionTypeSys : public InstructionType {
  private:
    uint8_t funct3{0};
    int32_t imm;
    OpCodeSetSystem opcodeSys;

  public:
    InstructionTypeSys(const OpCodeSet& o, const uint32_t& i) : InstructionType(o) {
        funct3 = calcFunct3(i);
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
                // csr->trapException(Trap(controller.getPC(), MCause::Breakpoint, 0));
                break;

            case OpCodeSetSystem::ECALL:
                // csr->trapException(Trap(controller.getPC(), MCause::EnvironmentCallFromMMode, 0));
                break;

            case OpCodeSetSystem::SRET:
            case OpCodeSetSystem::MRET:
                // csr->trapReturn();
                break;
            case OpCodeSetSystem::WFI:
                // throw std::string("WFI!!!!!");
                break;
            case OpCodeSetSystem::CSRRC:
            case OpCodeSetSystem::CSRRCI:
            case OpCodeSetSystem::CSRRS:
            case OpCodeSetSystem::CSRRSI:
            case OpCodeSetSystem::CSRRW:
            case OpCodeSetSystem::CSRRWI:
                // data.address = data.decode.imm32;
                break;
            case OpCodeSetSystem::INVALID:
                throw std::string("System Opcode desconhecido");
                break;
        }
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