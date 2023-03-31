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
#include <iostream>

RV32ISim::RV32ISim(Bus* bus) {
    cpu_pc = 0;
    ecall = false;
    this->bus = bus;

    // Init registers
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }
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
            std::cout << "lb x" << i.rd << " " << i.imm << "(x" << i.rs1 << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 1);
            break;
        case 0x1: // lH
            std::cout << "lh x" << i.rd << " " << i.imm << "(x" << i.rs1 << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 2);
            break;
        case 0x2: // lW
            std::cout << "lw x" << i.rd << " " << i.imm << "(x" << i.rs1 << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 4);
            break;
        case 0x4: // lbu
            std::cout << "lbu x" << i.rd << " " << i.imm << "(x" << i.rs1 << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 1, true);
            break;
        case 0x5: // lhu
            std::cout << "lhu x" << i.rd << " " << i.imm << "(x" << i.rs1 << ")" << '\n';
            bus->load(regs[i.rd], regs[i.rs1] + i.imm, 2, true);
            break;
    }
}

void RV32ISim::ulai(const Instr& i) {

    switch (i.funct3) {
        case 0x0:
            std::cout << "addi x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] + i.imm;
            break;
        case 0x1: //
            std::cout << "slli x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] << i.imm;
            break;
        case 0x2:
            std::cout << "slti x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            if (regs[i.rs1] < i.imm) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x3:
            std::cout << "sltiu x" << i.rd << " x" << i.rs1 << " " << (unsigned int)(i.imm & 0xfff) << '\n';
            if (regs[i.rs1] < ((unsigned int)(i.imm & 0xfff))) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x4:
            std::cout << "xori x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] ^ i.imm;
            break;
        case 0x5:
            if ((i.imm & 0xf00) == 0) { // TODO: check!
                std::cout << "slri x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
                regs[i.rd] = regs[i.rs1] >> i.imm;
            } else {
                std::cout << "srai x" << i.rd << " x" << i.rs1 << " " << (i.imm & 0x1f) << '\n';
                regs[i.rd] = regs[i.rs1] >> (i.imm & 0x1f);
            }
            break;
        case 0x6:
            std::cout << "ori x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] | i.imm;
            break;
        case 0x7:
            std::cout << "andi x" << i.rd << " x" << i.rs1 << " " << i.imm << '\n';
            regs[i.rd] = regs[i.rs1] & i.imm;
            break;
    }
}

void RV32ISim::auipc(const Instr& i) {
    int32_t imm = (i.instr >> 12) << 12;
    std::cout << "auipc x" << i.rd << " " << imm << '\n';
    regs[i.rd] = cpu_pc * 4 + imm;
}

void RV32ISim::saveRegister(const Instr& i) {
    int32_t imm = (i.imm >> 5) << 5 | i.rd; // Get immediate field
    switch (i.funct3) {
        case 0x0:
            std::cout << "sb x" << i.rs2 << " " << imm << "("
                      << "x" << i.rs1 << ")" << '\n';
            bus->store(regs[i.rs2], regs[i.rs1] + imm, 1);
            break;
        case 0x1:
            std::cout << "sh x" << i.rs2 << " " << imm << "("
                      << "x" << i.rs1 << ")" << '\n';
            bus->store(regs[i.rs2], regs[i.rs1] + imm, 2);
            break;
        case 0x2:
            std::cout << "sw x" << i.rs2 << " " << imm << "("
                      << "x" << i.rs1 << ")" << '\n';
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
                std::cout << "add x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
                regs[i.rd] = regs[i.rs1] + regs[i.rs2];
            } else {
                std::cout << "sub x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
                regs[i.rd] = regs[i.rs1] - regs[i.rs2];
            }
            break;
        case 0x1:
            std::cout << "sll x" << i.rd << " x" << i.rs1 << " x" << (regs[i.rs2] & 0x1f) << '\n';
            regs[i.rd] = regs[i.rs1] << (regs[i.rs2] & 0x1f);
            break;
        case 0x2:
            std::cout << "slt x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
            if (regs[i.rs1] < regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x3:
            std::cout << "sltu x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
            if (regs[i.rs1] < (unsigned int)regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x4:
            std::cout << "xor x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
            regs[i.rd] = regs[i.rs1] ^ regs[i.rs2];
            break;
        case 0x5:
            if ((i.instr >> 25) == 0) {
                std::cout << "srl x" << i.rd << " x" << i.rs1 << " x" << (regs[i.rs2] & 0x1f) << '\n';
                regs[i.rd] = ((unsigned int)regs[i.rs1]) >> (regs[i.rs2] & 0x1f);
            } else {
                std::cout << "sra x" << i.rd << " x" << i.rs1 << " x" << (regs[i.rs2] & 0x1f) << '\n';
                regs[i.rd] = regs[i.rs1] >> (regs[i.rs2] & 0x1f);
            }
            break;
        case 0x6:
            std::cout << "OR x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
            regs[i.rd] = regs[i.rs1] | regs[i.rs2];
            break;
        case 0x7:
            std::cout << "and x" << i.rd << " x" << i.rs1 << " x" << i.rs2 << '\n';
            regs[i.rd] = regs[i.rs1] & regs[i.rs2];
            break;
    }
}

void RV32ISim::lui(const Instr& i) {
    std::cout << "lui x" << i.rd << " " << ((i.instr >> 12) & 0xfffff) << '\n';
    regs[i.rd] = ((i.instr >> 12) & 0xfffff) << 12;
}

void RV32ISim::branchCase(const Instr& i) {
    int32_t offset = ((i.instr >> 25) << 5) + ((i.instr >> 7) & 0x1f) - 1;
    if (offset > 0) {
        offset++;
    }
    switch (i.funct3) {
        case 0x0:
            std::cout << "beq x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] == regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x1:
            std::cout << "bne x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] != regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x4:
            std::cout << "blt x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] < regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x5:
            std::cout << "bge x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] >= regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset >> 2) - 1;
            }
            break;
        case 0x6:
            std::cout << "bltu x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] < (unsigned)regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset / 4) - 1;
            }
            break;
        case 0x7:
            std::cout << "bgeu x" << i.rs1 << " x" << i.rs2 << " " << offset << '\n';
            if (regs[i.rs1] >= (unsigned)regs[i.rs2]) {
                cpu_pc = cpu_pc + (offset >> 2) - 1;
            }
            break;
    }
}

void RV32ISim::jalr(const Instr& i) {

    std::cout << "jalr x" << i.rd << " x" << i.rs1 << " " << i.imm << std::endl;

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

    std::cout << "jal x" << i.rd << " " << imm << std::endl;

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
    std::cout << "0x";
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
