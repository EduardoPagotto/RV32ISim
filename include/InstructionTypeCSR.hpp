#pragma once
#include "InstructionType.hpp"
#include <iostream>
#include <stdint.h>

class InstructionTypeCSR : public InstructionType {
  private:
    uint8_t rd{0}, rs1{0};
    int32_t imm;
    OpCodeCSR opcodeCSR;
    uint32_t val_rs1{0};

  public:
    InstructionTypeCSR(const OpCode& o, const uint32_t& i, uint32_t* x) : InstructionType(o) {

        opcodeCSR = static_cast<OpCodeCSR>(calcFunct3(i));
        rd = calcRd(i);
        rs1 = calcRs1(i);
        val_rs1 = x[rs1];
        imm = i >> 20;
    }

    virtual void execute(Controller& controller) override {

        // returnFromTrap = data.decode.imm32 == 0x302;
        switch (opcodeCSR) {
            case OpCodeCSR::CSRRC:
                std::cout << "csrrc ";
                break;
            case OpCodeCSR::CSRRCI:
                std::cout << "csrrci";
                break;
            case OpCodeCSR::CSRRS:
                std::cout << "csrrs ";
                break;
            case OpCodeCSR::CSRRSI:
                std::cout << "csrrsi";
                break;
            case OpCodeCSR::CSRRW:
                std::cout << "csrrw " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm) << ", "
                          << Debug::alias[rs1];
                break;
            case OpCodeCSR::CSRRWI:
                std::cout << "csrrwi";
                // data.address = data.decode.imm32;
                break;
            default:
                throw std::string("System Opcode desconhecido");
                break;
        }
        std::cout << "\t\t# ";
        std::cout << '\n';
    }

    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) override {

        bool isValid = false; // true; // TODO: tirar depois quando for testar
        switch (opcodeCSR) {
            case OpCodeCSR::CSRRC:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.valueRS1)));
                break;

            case OpCodeCSR::CSRRCI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.decode.rs1)));
                break;

            case OpCodeCSR::CSRRS:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.valueRS1);
                break;

            case OpCodeCSR::CSRRSI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.decode.rs1);
                break;

            case OpCodeCSR::CSRRW:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.valueRS1);
                break;

            case OpCodeCSR::CSRRWI:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.decode.rs1);
                break;
            default:
                break;
        }

        return WriteBackData{0, 0, isValid};
    }
};