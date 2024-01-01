#pragma once
#include "Debug.hpp"
#include "InstructionType.hpp"

class InstructionTypeCSR : public InstructionType {
  private:
    uint8_t rd{0}, rs1{0}; // FIXME ver se zimm ??
    int32_t imm;
    uint32_t val_rs1{0};

  public:
    InstructionTypeCSR(const uint32_t& o, uint32_t* x) : InstructionType(o) {
        rd = calcRd(o);
        rs1 = calcRs1(o);
        val_rs1 = x[rs1];
        imm = o >> 20;
    }

    virtual void execute(Controller& controller) override {

        // returnFromTrap = data.decode.imm32 == 0x302;
        switch (opcode) {
            case OPC_CSRRC:
                std::cout << "csrrc ";
                break;
            case OPC_CSRRCI:
                std::cout << "csrrci";
                break;
            case OPC_CSRRS:
                std::cout << "csrrs ";
                break;
            case OPC_CSRRSI:
                std::cout << "csrrsi";
                break;
            case OPC_CSRRW:
                std::cout << "csrrw " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm) << ", "
                          << Debug::alias[rs1];
                break;
            case OPC_CSRRWI:
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
        switch (opcode) {
            case OPC_CSRRC:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.valueRS1)));
                break;

            case OPC_CSRRCI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, (data.value & (~d.decode.rs1)));
                break;

            case OPC_CSRRS:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.valueRS1);
                break;

            case OPC_CSRRSI:
                // data.value = this->csr->read(d.address);
                // if (d.decode.rs1 != 0)
                //     this->csr->write(d.address, data.value | d.decode.rs1);
                break;

            case OPC_CSRRW:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.valueRS1);
                break;

            case OPC_CSRRWI:
                // data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                // this->csr->write(d.address, d.decode.rs1);
                break;
            default:
                break;
        }

        return WriteBackData{0, 0, isValid};
    }
};