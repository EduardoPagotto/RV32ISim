#include "include/PrintAs.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

PrintAs::PrintAs() {
    alias.push_back("r0"); // 0  -> zero
    alias.push_back("ra"); // 1  -> return address
    alias.push_back("sp"); // 2  -> stackp pointer
    alias.push_back("gp"); // 3  -> global pointer
    alias.push_back("tp"); // 4  -> thread pointer
    alias.push_back("t0"); // 5  -> temporary / alternative link
    alias.push_back("t1"); // 6  -> temporary
    alias.push_back("t2"); // 7  -> temporary

    alias.push_back("s0"); // 8  -> saved / frame pointer
    alias.push_back("s1"); // 9  -> saved
    alias.push_back("a0"); // 10 -> func arg / return value
    alias.push_back("a1"); // 11 -> func arg / return value
    alias.push_back("a2"); // 12 -> func arg
    alias.push_back("a3"); // 13 -> func arg
    alias.push_back("a4"); // 14 -> func arg
    alias.push_back("a5"); // 15 -> func arg

    alias.push_back("a6"); // 16 -> func arg
    alias.push_back("a7"); // 17 -> func arg
    alias.push_back("s2"); // 18 -> saved
    alias.push_back("s3"); // 19 -> saved
    alias.push_back("s4"); // 20 -> saved
    alias.push_back("s5"); // 21 -> saved
    alias.push_back("s6"); // 22 -> saved
    alias.push_back("s7"); // 23 -> saved

    alias.push_back("s8");  // 24 -> saved
    alias.push_back("s9");  // 25 -> saved
    alias.push_back("s10"); // 26 -> saved
    alias.push_back("s11"); // 27 -> saved
    alias.push_back("t3");  // 28 -> temporary
    alias.push_back("t4");  // 29 -> temporary
    alias.push_back("t5");  // 30 -> temporary
    alias.push_back("t6");  // 31 -> temporary
}

std::string PrintAs::printValue(const uint32_t& indice, const uint32_t value) {

    std::stringstream ss;

    if (indice == 2)
        ss << alias[indice] << " = " << int_to_hex(value);
    else
        ss << alias[indice] << " = " << static_cast<int32_t>(value);

    return ss.str();
}

void PrintAs::printAsHex(const uint32_t& addr, const uint32_t& instr) {
    int res = 0;

    std::cout << int_to_hex(addr) << " ";

    for (int i = 0; i < 8; i++) {
        res = (instr >> (4 * (7 - i))) & 0xf;
        switch (res) {
            case 10:
                std::cout << "a";
                break;
            case 11:
                std::cout << "b";
                break;
            case 12:
                std::cout << "c";
                break;
            case 13:
                std::cout << "d";
                break;
            case 14:
                std::cout << "e";
                break;
            case 15:
                std::cout << "f";
                break;
            default:
                std::cout << res;
                break;
        }
    }
    std::cout << "    ";
}

std::string PrintAs::debugCommandRegs(const DecodeData& d) {
    std::stringstream ss;

    switch (d.opcode) {
        case OpCodeSet::LOAD:
            switch (d.funct3) {
                case 0x0: // lb
                    ss << "lb    ";
                    break;

                case 0x1: // lH
                    ss << "lh    ";
                    break;

                case 0x2: // lW
                    ss << "lw    ";
                    break;

                case 0x4: // lbu
                    ss << "lbu   ";
                    break;

                case 0x5: // lhu
                    ss << "lhu   ";
                    break;
            }
            ss << this->alias[d.rd] << " " << d.imm32 << "(" << this->alias[d.rs1] << ")";
            break;

        case OpCodeSet::ULAI:
            switch (d.funct3) {
                case 0x0: // ADDI
                    ss << "addi  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;

                case 0x1: // SLLI
                    ss << "slli  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;

                case 0x2: // SLTI
                    ss << "slti  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;

                case 0x3: // SLTIU
                    ss << "sltiu " << this->alias[d.rd] << " " << this->alias[d.rs1] << " "
                       << (unsigned int)(d.imm32 & 0xfff);
                    break;

                case 0x4: // XORI
                    ss << "xori  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;

                case 0x5: // SLRI / SRAI  TODO: check!
                    if ((d.imm32 & 0xf00) == 0)
                        ss << "slri  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    else
                        ss << "srai  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << (d.imm32 & 0x1f);
                    break;

                case 0x6: // ORI
                    ss << "ori   " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;
                case 0x7: // ANDI
                    ss << "andi  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
                    break;
            }
            break;

        case OpCodeSet::AUIPC:
            ss << "auipc " << this->alias[d.rd] << " " << int_to_hex(d.imm32);
            break;

        case OpCodeSet::LUI:
            ss << "lui   " << this->alias[d.rd] << " " << int_to_hex(d.imm32);
            break;

        case OpCodeSet::SAVE:
            switch (d.funct3) {
                case 0x0:
                    ss << "sb    ";
                    break;
                case 0x1:
                    ss << "sh    ";
                    break;
                case 0x2:
                    ss << "sw    ";
                    break;
            }

            ss << this->alias[d.rs2] << " " << d.imm32 << "(" << this->alias[d.rs1] << ")";
            break;

        case OpCodeSet::ULA:
            switch (d.funct3) {
                case 0x0:
                    if ((d.instr >> 25) == 0) {
                        ss << "add   ";
                    } else {
                        ss << "sub   ";
                    }
                    break;
                case 0x1:
                    ss << "sll   ";
                    break;
                case 0x2:
                    ss << "slt   ";
                    break;
                case 0x3:
                    ss << "sltu  ";
                    break;
                case 0x4:
                    ss << "xor   ";
                    break;
                case 0x5:
                    if ((d.instr >> 25) == 0) {
                        ss << "srl   ";
                    } else {
                        ss << "sra   ";
                    }
                    break;
                case 0x6:
                    ss << "OR    ";
                    break;
                case 0x7:
                    ss << "and   ";
                    break;
            }

            ss << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << this->alias[d.rs2 & 0x1f];
            break;

        case OpCodeSet::BRANCH:
            switch (d.funct3) {
                case 0x0:
                    ss << "beq   ";
                    break;

                case 0x1:
                    ss << "bne   ";
                    break;

                case 0x4:
                    ss << "blt   ";
                    break;

                case 0x5:
                    ss << "bge   ";
                    break;

                case 0x6:
                    ss << "bltu  ";
                    break;

                case 0x7:
                    ss << "bgeu  ";
                    break;
            }

            ss << this->alias[d.rs1] << " " << this->alias[d.rs2] << " " << d.imm32;
            break;

        case OpCodeSet::JALR:
            ss << "jalr  " << this->alias[d.rd] << " " << this->alias[d.rs1] << " " << d.imm32;
            break;

        case OpCodeSet::JAL:
            ss << "jal " << this->alias[d.rd] << " " << d.imm32;
            break;

        case OpCodeSet::FENCE:
            break;

        case OpCodeSet::SYSTEM:
            switch (d.opcodeSys) {
                case OpCodeSetSystem::EBREAK:
                    ss << "Ebreak - Exiting program" << '\n';
                    break;
                case OpCodeSetSystem::ECALL:
                    ss << "Ecall - Exiting program" << '\n';
                    break;
                default:

                    break;
            }

            break;

        case OpCodeSet::INVALID:
            break;
    }

    ss << " \t\t# ";
    // ss << '\n'

    return ss.str();
}

void PrintAs::printPC(const uint32_t& val) { std::cout << "pc = " << int_to_hex(val) << " "; }

void PrintAs::printRegtoMemory(const uint8_t& index, const uint32_t& valRS, const uint32_t& address) {
    std::cout << alias[index] << " = " << int_to_hex(valRS) << " -> (0x" << int_to_hex(address) << ")";
}

void PrintAs::printRegVal(const uint8_t& index, const uint32_t& value) {
    std::cout << alias[index] << " = " << int_to_hex(value);
}

void PrintAs::printAddress(const uint32_t& addr) { std::cout << "(0x" << int_to_hex(addr) << ") -> "; }

void PrintAs::printRegisters(uint32_t* regs) {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << " " << regs[i] << std::endl;
    }
}

bool PrintAs::writeToFile(uint32_t* regs, const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)regs, 32 * sizeof(int));
    outfile.close();
    return true;
}
