/**
 * @file RV32ISim.cpp
 * @author  Eduardo Pagotto
 * @brief  Implementation for the RV32ISim class
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "include/RV32ISim.h"
#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

RV32ISim::RV32ISim(Bus* bus) {
    cpu_pc = 0;
    ecall = false;
    this->bus = bus;

    // Init registers
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }

    regAlias.push_back("r0 "); // 0  -> zero
    regAlias.push_back("ra "); // 1  -> return address
    regAlias.push_back("sp "); // 2  -> stackp pointer
    regAlias.push_back("gp "); // 3  -> global pointer
    regAlias.push_back("tp "); // 4  -> thread pointer
    regAlias.push_back("t0 "); // 5  -> temporary / alternative link
    regAlias.push_back("t1 "); // 6  -> temporary
    regAlias.push_back("t2 "); // 7  -> temporary

    regAlias.push_back("s0 "); // 8  -> saved / frame pointer
    regAlias.push_back("s1 "); // 9  -> saved
    regAlias.push_back("a0 "); // 10 -> func arg / return value
    regAlias.push_back("a1 "); // 11 -> func arg / return value
    regAlias.push_back("a2 "); // 12 -> func arg
    regAlias.push_back("a3 "); // 13 -> func arg
    regAlias.push_back("a4 "); // 14 -> func arg
    regAlias.push_back("a5 "); // 15 -> func arg

    regAlias.push_back("a6 "); // 16 -> func arg
    regAlias.push_back("a7 "); // 17 -> func arg
    regAlias.push_back("s2 "); // 18 -> saved
    regAlias.push_back("s3 "); // 19 -> saved
    regAlias.push_back("s4 "); // 20 -> saved
    regAlias.push_back("s5 "); // 21 -> saved
    regAlias.push_back("s6 "); // 22 -> saved
    regAlias.push_back("s7 "); // 23 -> saved

    regAlias.push_back("s8 "); // 24 -> saved
    regAlias.push_back("s9 "); // 25 -> saved
    regAlias.push_back("s10"); // 26 -> saved
    regAlias.push_back("s11"); // 27 -> saved
    regAlias.push_back("t3 "); // 28 -> temporary
    regAlias.push_back("t4 "); // 29 -> temporary
    regAlias.push_back("t5 "); // 30 -> temporary
    regAlias.push_back("t6 "); // 31 -> temporary
}

RV32ISim::~RV32ISim() {}

bool RV32ISim::writeToFile(const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)&regs, 32 * sizeof(int));
    outfile.close();
    return true;
}

void RV32ISim::loadRegister(const Instr& i) {
    switch (i.funct3) {
        case 0x0: // lb
            std::cout << "lb    " << regAlias[i.rd] << " " << i.imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 1);
            break;
        case 0x1: // lH
            std::cout << "lh    " << regAlias[i.rd] << " " << i.imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 2);
            break;
        case 0x2: // lW
            std::cout << "lw    " << regAlias[i.rd] << " " << i.imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 4);
            break;
        case 0x4: // lbu
            std::cout << "lbu   " << regAlias[i.rd] << " " << i.imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 1, true);
            break;
        case 0x5: // lhu
            std::cout << "lhu   " << regAlias[i.rd] << " " << i.imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 2, true);
            break;
    }
}

void RV32ISim::ulai(const Instr& i) {

    uint32_t val_rs = regs[i.rs1];
    switch (i.funct3) {
        case 0x0:
            regs[i.rd] = regs[i.rs1] + i.imm;

            std::cout << "addi  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << " # ";
            std::cout << regAlias[i.rd] << "= 0x" << int_to_hex(regs[i.rd]) << ", " << static_cast<int32_t>(regs[i.rd])
                      << " ; ";
            std::cout << regAlias[i.rs1] << "= 0x" << int_to_hex(val_rs) << ", " << static_cast<int32_t>(val_rs)
                      << '\n';

            break;
        case 0x1: //
            std::cout << "slli  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] << i.imm;
            break;
        case 0x2:
            std::cout << "slti  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
            if (regs[i.rs1] < i.imm) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x3:
            std::cout << "sltiu " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << (unsigned int)(i.imm & 0xfff)
                      << '\n';
            if (regs[i.rs1] < ((unsigned int)(i.imm & 0xfff))) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x4:
            std::cout << "xori  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] ^ i.imm;
            break;
        case 0x5:
            if ((i.imm & 0xf00) == 0) { // TODO: check!
                std::cout << "slri  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
                regs[i.rd] = regs[i.rs1] >> i.imm;
            } else {
                std::cout << "srai  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << (i.imm & 0x1f) << '\n';
                regs[i.rd] = regs[i.rs1] >> (i.imm & 0x1f);
            }
            break;
        case 0x6:
            std::cout << "ori   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] | i.imm;
            break;
        case 0x7:
            std::cout << "andi  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] & i.imm;
            break;
    }
}

void RV32ISim::auipc(const Instr& i) {
    int32_t imm = (i.instr >> 12) << 12;
    std::cout << "auipc " << regAlias[i.rd] << " " << imm << '\n';
    regs[i.rd] = cpu_pc * 4 + imm;
}

void RV32ISim::saveRegister(const Instr& i) {
    int32_t imm = (i.imm >> 5) << 5 | i.rd; // Get immediate field
    switch (i.funct3) {
        case 0x0:
            std::cout << "sb    " << regAlias[i.rs2] << " " << imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->store(regs[i.rs2], regs[i.rs1] + imm, 1);
            break;
        case 0x1:
            std::cout << "sh    " << regAlias[i.rs2] << " " << imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->store(regs[i.rs2], regs[i.rs1] + imm, 2);
            break;
        case 0x2:
            std::cout << "sw    " << regAlias[i.rs2] << " " << imm << "(" << regAlias[i.rs1] << ")" << '\n';
            bus->store(regs[i.rs2], regs[i.rs1] + imm, 4);
            // std::cout << "Memory saved " << (unsigned int)
            // mem[regs[rs1]+imm] << " at address: " << regs[rs1]+imm
            // << '\n'; //REMOVE
            break;
    }
}

void RV32ISim::ula(const Instr& i) {

    switch (i.funct3) {
        case 0x0:
            if ((i.instr >> 25) == 0) {
                std::cout << "add   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
                regs[i.rd] = regs[i.rs1] + regs[i.rs2];
            } else {
                std::cout << "sub   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
                regs[i.rd] = regs[i.rs1] - regs[i.rs2];
            }
            break;
        case 0x1:
            std::cout << "sll   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2 & 0x1f] << '\n';
            regs[i.rd] = regs[i.rs1] << (regs[i.rs2] & 0x1f);
            break;
        case 0x2:
            std::cout << "slt   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
            if (regs[i.rs1] < regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x3:
            std::cout << "sltu  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
            if (regs[i.rs1] < (unsigned int)regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x4:
            std::cout << "xor   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regs[i.rs2] << '\n';
            regs[i.rd] = regs[i.rs1] ^ regs[i.rs2];
            break;
        case 0x5:
            if ((i.instr >> 25) == 0) {
                std::cout << "srl   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << (regAlias[i.rs2 & 0x1f])
                          << '\n';
                regs[i.rd] = ((unsigned int)regs[i.rs1]) >> (regs[i.rs2] & 0x1f);
            } else {
                std::cout << "sra   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << (regAlias[i.rs2 & 0x1f])
                          << '\n';
                regs[i.rd] = regs[i.rs1] >> (regs[i.rs2] & 0x1f);
            }
            break;
        case 0x6:
            std::cout << "OR    " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
            regs[i.rd] = regs[i.rs1] | regs[i.rs2];
            break;
        case 0x7:
            std::cout << "and   " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << regAlias[i.rs2] << '\n';
            regs[i.rd] = regs[i.rs1] & regs[i.rs2];
            break;
    }
}

void RV32ISim::lui(const Instr& i) {
    std::cout << "lui   " << regAlias[i.rd] << " " << ((i.instr >> 12) & 0xfffff) << '\n';
    regs[i.rd] = ((i.instr >> 12) & 0xfffff) << 12;
}

void RV32ISim::branchCase(const Instr& i) {
    int32_t offset = ((i.instr >> 25) << 5) + ((i.instr >> 7) & 0x1f) - 1;
    if (offset > 0) {
        offset++;
    }
    switch (i.funct3) {
        case 0x0:
            std::cout << "beq   " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] == regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x1:
            std::cout << "bne   " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] != regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x4:
            std::cout << "blt   " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] < regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x5:
            std::cout << "bge   " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] >= regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset >> 2) - 1;
            }
            break;
        case 0x6:
            std::cout << "bltu  " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] < (unsigned)regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x7:
            std::cout << "bgeu  " << regAlias[i.rs1] << " " << regAlias[i.rs2] << " " << offset << '\n';
            if (regs[i.rs1] >= (unsigned)regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset >> 2) - 1;
            }
            break;
    }
}

void RV32ISim::jalr(const Instr& i) {

    std::cout << "jalr  " << regAlias[i.rd] << " " << regAlias[i.rs1] << " " << i.imm << std::endl;

    regs[i.rd] = (cpu_pc + 1) << 2;
    cpu_pc = (regs[i.rs1] + i.imm) >> 2;
    cpu_pc = cpu_pc - 1;
}

void RV32ISim::jal(const Instr& i) {
    // Get imm
    int32_t imm = (i.instr >> 30) << 20;          // Get instr[20]
    imm = imm | (((i.instr >> 12) & 0xff) << 12); // Get instr[19:12]
    imm = imm | (((i.instr >> 20) & 0x1) << 11);  // Get instr[11]
    imm = imm | (((i.instr >> 21) & 0x3ff) << 1); // Get instr[10:1]

    std::cout << "jal " << regAlias[i.rd] << " " << imm << std::endl;

    regs[i.rd] = (cpu_pc + 1) << 2;
    cpu_pc = cpu_pc + (imm >> 2) - 1; // Because of inc after switch
}

void RV32ISim::step() {

    // Get fields
    uint32_t instr = 0;
    bus->load(instr, 4 * cpu_pc, 4);
    printAsHex(instr); // REMOVE

    Instr i(static_cast<int32_t>(instr));

    switch (i.opcode) {
        case 0x03:
            loadRegister(i); // load Instructions
            break;
        case 0x13:
            ulai(i); // ULAi
            break;
        case 0x17:
            auipc(i); // auipc
            break;
        case 0x23:
            saveRegister(i); // Save Instructions
            break;
        case 0x33:
            ula(i); // ULA
            break;
        case 0x37:
            lui(i); // LUI
            break;
        case 0x63:
            branchCase(i); // Branch case
            break;
        case 0x67:
            jalr(i); // jalr
            break;
        case 0x6f:
            jal(i); // jal
            break;
        case 0x73:
            std::cout << "Ecall - Exiting program" << '\n';
            ecall = true;
            break;
    }
    regs[0] = 0; // jalr x0 xX is not supposed to cast exception, so this is
                 // the easier way
    cpu_pc++;    // Increment program counter
}

void RV32ISim::printRegisters() {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << " " << regs[i] << std::endl;
    }
}

void RV32ISim::printProgram() {
    std::cout << "Program" << '\n' << std::endl;

    uint32_t addr = 0;
    do {

        uint32_t instr;
        bus->load(instr, 4 * addr, 4);
        printAsHex(instr);
        std::cout << "" << '\n';

        addr++;

    } while (bus->hasData(addr));
}

void RV32ISim::printAsHex(unsigned int instr) {
    int res = 0;

    std::cout << int_to_hex(cpu_pc * 4) << " ";

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
