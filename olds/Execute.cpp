#include "include/Execute.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

void Execute::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        data = decode->getData();

        data.valueRS1 = regs[data.decode.rs1];
        data.valueRS2 = regs[data.decode.rs2];
        data.valueRD = regs[data.decode.rd];

        switch (data.decode.opcode) {
            case OpCode::LOAD:
                data.address = data.valueRS1 + data.decode.imm32;
                switch (data.decode.funct3) {
                    case 0x0: // lb
                        data.width = MemoryAccessWidth::Byte;
                        data.valSigned = false;
                        break;

                    case 0x1: // lH
                        data.width = MemoryAccessWidth::HalfWord;
                        data.valSigned = false;
                        break;

                    case 0x2: // lW
                        data.width = MemoryAccessWidth::Word;
                        data.valSigned = false;
                        break;

                    case 0x4: // lbu
                        data.width = MemoryAccessWidth::Byte;
                        data.valSigned = true;
                        break;

                    case 0x5: // lhu
                        data.width = MemoryAccessWidth::Byte;
                        data.valSigned = true;
                        break;
                }
                break;

            case OpCode::ULAI:
                switch (data.decode.funct3) {
                    case 0x0: // ADDI
                        data.address = data.valueRS1 + data.decode.imm32;
                        break;

                    case 0x1: // SLLI
                        data.address = data.valueRS1 << data.decode.imm32;
                        break;

                    case 0x2: // SLTI
                        data.address = (data.valueRS1 < data.decode.imm32) ? 1 : 0;
                        break;

                    case 0x3: // SLTIU
                        data.address = (data.valueRS1 < ((unsigned int)(data.decode.imm32 & 0xfff))) ? 1 : 0;
                        break;

                    case 0x4: // XORI
                        data.address = data.valueRS1 ^ data.decode.imm32;
                        break;

                    case 0x5: // SLRI / SRAI
                        data.address = ((data.decode.imm32 & 0xf00) == 0) ? data.valueRS1 >> data.decode.imm32
                                                                          : data.valueRS1 >> (data.decode.imm32 & 0x1f);
                        break;

                    case 0x6: // ORI
                        data.address = data.valueRS1 | data.decode.imm32;
                        break;

                    case 0x7: // ANDI
                        data.address = data.valueRS1 & data.decode.imm32;
                        break;
                }
                break;

            case OpCode::AUIPC:
                data.address = data.decode.fetch.pc + static_cast<uint32_t>(data.decode.imm32);
                break;

            case OpCode::SAVE:
                data.address = data.valueRS1 + data.decode.imm32;
                switch (data.decode.funct3) {
                    case 0x0:
                        data.width = MemoryAccessWidth::Byte;
                        break;

                    case 0x1:
                        data.width = MemoryAccessWidth::HalfWord;
                        break;

                    case 0x2:
                        data.width = MemoryAccessWidth::Word;
                        break;
                }
                break;

            case OpCode::ULA:
                switch (data.decode.funct3) {
                    case 0x0:
                        data.address = ((data.decode.fetch.instr >> 25) == 0) ? data.valueRS1 + data.valueRS2
                                                                              : data.valueRS1 - data.valueRS2;
                        break;

                    case 0x1:
                        data.address = data.valueRS1 << (data.valueRS2 & 0x1f);
                        break;

                    case 0x2:
                        data.address = (data.valueRS1 < data.valueRS2) ? 1 : 0;
                        break;

                    case 0x3:
                        data.address = (data.valueRS1 < (unsigned int)data.valueRS2) ? 1 : 0;
                        break;

                    case 0x4:
                        data.address = data.valueRS1 ^ data.valueRS2;
                        break;

                    case 0x5:
                        data.address = ((data.decode.fetch.instr >> 25) == 0)
                                           ? ((unsigned int)data.valueRS1) >> (data.valueRS2 & 0x1f)
                                           : data.valueRS1 >> (data.valueRS2 & 0x1f);
                        break;

                    case 0x6:
                        data.address = data.valueRS1 | data.valueRS2;
                        break;

                    case 0x7:
                        data.address = data.valueRS1 & data.valueRS2;
                        break;
                }
                break;

            case OpCode::LUI:
                data.address = data.decode.imm32;
                break;

            case OpCode::BRANCH: {
                switch (data.decode.funct3) {
                    case 0x0:
                        if (data.valueRS1 == data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;

                    case 0x1:
                        if (data.valueRS1 != data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;

                    case 0x4:
                        if (data.valueRS1 < data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;

                    case 0x5:
                        if (data.valueRS1 >= data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;

                    case 0x6:
                        if (data.valueRS1 < (unsigned)data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;

                    case 0x7:
                        if (data.valueRS1 >= (unsigned)data.valueRS2)
                            csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                        break;
                }
            } break;

            case OpCode::JALR:
                data.address = data.decode.fetch.pcPlus4;
                csr->setBranchAddress(data.valueRS1 + data.decode.imm32);
                break;

            case OpCode::JAL:
                data.address = data.decode.fetch.pcPlus4;
                csr->setBranchAddress(data.decode.fetch.pc + data.decode.imm32);
                break;

            case OpCode::FENCE:
                // TODO: implementar
                break;

            case OpCode::SYSTEM:
                // returnFromTrap = data.decode.imm32 == 0x302;
                switch (data.decode.opcodeSys) {
                    case OpCodeSystem::EBREAK:
                        csr->trapException(Trap(data.decode.fetch.pc, MCause::Breakpoint, 0));
                        break;

                    case OpCodeSystem::ECALL:
                        csr->trapException(Trap(data.decode.fetch.pc, MCause::EnvironmentCallFromMMode, 0));
                        break;

                    case OpCodeSystem::SRET:
                    case OpCodeSystem::MRET:
                        csr->trapReturn();
                        break;
                    case OpCodeSystem::WFI:
                        throw std::string("WFI!!!!!");
                        break;
                    case OpCodeSystem::CSRRC:
                    case OpCodeSystem::CSRRCI:
                    case OpCodeSystem::CSRRS:
                    case OpCodeSystem::CSRRSI:
                    case OpCodeSystem::CSRRW:
                    case OpCodeSystem::CSRRWI:
                        data.address = data.decode.imm32;
                        break;
                    case OpCodeSystem::INVALID:
                        throw std::string("System Opcode desconhecido");
                        break;
                }
                break;

            default:
                throw std::string("Opcode desconhecido");
                break;
        }
        regs[0] = 0;
    }
}
