#pragma once
#include "Debug.hpp"
#include "InstructionType.hpp"

class InstructionTypeCSR : public InstructionType {
  private:
    uint8_t rd{0}, rs1{0}, funct3{0}; // FIXME ver se zimm ??
    int32_t imm;
    uint32_t val_rs1{0};
    uint32_t val_rd{0};

  public:
    InstructionTypeCSR(const uint32_t& o, uint32_t* x) : InstructionType(o) {
        rd = calcRd(o);
        rs1 = calcRs1(o);
        val_rs1 = x[rs1];
        val_rd = x[rd];
        funct3 = calcFunct3(o);
        imm = o >> 20;
    }

    virtual void execute(Controller& controller) override {
        // returnFromTrap = data.decode.imm32 == 0x302;
    }

    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) override {

        uint32_t value{0};
        switch (funct3) {
            case OPC_CSRRC:
                std::cout << "CSRRC ";
                value = controller.getCSR().read(imm);
                if (rs1 != 0)
                    controller.getCSR().write(imm, (value & (~val_rs1)));
                break;

            case OPC_CSRRCI:
                std::cout << "CSRRCI";
                value = controller.getCSR().read(imm);
                if (rs1 != 0)
                    controller.getCSR().write(imm, (value & (~rs1))); // val_rs1 ??
                break;

            case OPC_CSRRS:
                std::cout << "CSRRS ";
                value = controller.getCSR().read(imm);
                if (rs1 != 0)
                    controller.getCSR().write(imm, value | val_rs1);
                break;

            case OPC_CSRRSI:
                std::cout << "CSRRSI";
                value = controller.getCSR().read(imm);
                if (rs1 != 0)
                    controller.getCSR().write(imm, value | rs1); // val_rs1 ??
                break;

            case OPC_CSRRW:
                std::cout << "CSRRW " << Debug::alias[rd] << ", " << Debug::int_to_hex(imm) << ", " << Debug::alias[rs1]
                          << " (" << Debug::int_to_hex(val_rs1) << ")";

                value = (val_rd != 0) ? controller.getCSR().read(imm) : 0;
                controller.getCSR().write(imm, val_rs1);
                break;

            case OPC_CSRRWI:
                std::cout << "CSRRWI";
                value = (val_rd != 0) ? controller.getCSR().read(imm) : 0;
                controller.getCSR().write(imm, rs1); // val_rs1 ??
                break;

            default:
                // TODO: opcode ou i ?
                controller.trapException(Trap(controller.getPC(), MCause::IllegalInstruction, opcode));
                break;
        }

        return WriteBackData{rd, value, true};
    }
};