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

RV32ISim::RV32ISim() {
    pc = 0;
    length = 0;
    mem = NULL;
    capacity = 0;
    ecall = false;
    // Init registers
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }

    // Preallocating memory.
    mem = new unsigned char[0x100000];
    capacity = 0x100000;

    // Fill memory
    for (unsigned int i = 0; i < capacity; i++) {
        mem[i] = 0;
    }
}

RV32ISim::~RV32ISim() {
    // Implemnt
    if (mem != NULL) {
        delete[] mem;
        mem = NULL;
    }
}

bool RV32ISim::readFromFile(const char* filepath) {
    FILE* fp;
    unsigned int* buffer;
    long int fileSize;

    fp = fopen(filepath, "rb");
    if (fp == NULL) {
        return false;
    }

    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp); // size of file in bytes
    fseek(fp, 0, SEEK_SET);

    length = fileSize / 4;
    buffer = new unsigned int[length]; // Allocate buffer

    fread(buffer, sizeof(buffer), length, fp);
    fclose(fp);

    // Save the program to the memory
    for (int i = 0; i < length; i++) {
        save(buffer[i], i * 4, 4);
    }

    // Reclaim memory
    delete[] buffer;
    buffer = NULL;

    // File was read
    return true;
}

bool RV32ISim::writeToFile(const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)&regs, 32 * sizeof(int));
    outfile.close();
    return true;
}

void RV32ISim::step() {

    // Get fields
    int instr = 0;
    load(instr, 4 * pc, 4);
    printAsHex(instr); // REMOVE
    int opcode = instr & 0x7f;
    int funct3 = (instr >> 12) & 0x7;
    int imm = (instr >> 20);
    int rd = (instr >> 7) & 0x1f;
    int rs1 = (instr >> 15) & 0x1f;
    int rs2 = imm & 0x1f;
    int offset; // For program branch

    switch (opcode) {
        case 0x03: // load Instructions
            switch (funct3) {
                case 0x0: // lb
                    std::cout << "lb x" << rd << " " << imm << "(x" << rs1 << ")" << '\n';
                    load(regs[rd], regs[rs1] + imm, 1);
                    break;
                case 0x1: // lH
                    std::cout << "lh x" << rd << " " << imm << "(x" << rs1 << ")" << '\n';
                    load(regs[rd], regs[rs1] + imm, 2);
                    break;
                case 0x2: // lW
                    std::cout << "lw x" << rd << " " << imm << "(x" << rs1 << ")" << '\n';
                    load(regs[rd], regs[rs1] + imm, 4);
                    break;
                case 0x4: // lbu
                    std::cout << "lbu x" << rd << " " << imm << "(x" << rs1 << ")" << '\n';
                    load(regs[rd], regs[rs1] + imm, 1, true);
                    break;
                case 0x5: // lhu
                    std::cout << "lhu x" << rd << " " << imm << "(x" << rs1 << ")" << '\n';
                    load(regs[rd], regs[rs1] + imm, 2, true);
                    break;
            }
            break;
        case 0x13:
            switch (funct3) {
                case 0x0:
                    std::cout << "addi x" << rd << " x" << rs1 << " " << imm << '\n';
                    regs[rd] = regs[rs1] + imm;
                    break;
                case 0x1: //
                    std::cout << "slli x" << rd << " x" << rs1 << " " << imm << '\n';
                    regs[rd] = regs[rs1] << imm;
                    break;
                case 0x2:
                    std::cout << "slti x" << rd << " x" << rs1 << " " << imm << '\n';
                    if (regs[rs1] < imm) {
                        regs[rd] = 1;
                    } else {
                        regs[rd] = 0;
                    }
                    break;
                case 0x3:
                    std::cout << "sltiu x" << rd << " x" << rs1 << " " << (unsigned int)(imm & 0xfff) << '\n';
                    if (regs[rs1] < ((unsigned int)(imm & 0xfff))) {
                        regs[rd] = 1;
                    } else {
                        regs[rd] = 0;
                    }
                    break;
                case 0x4:
                    std::cout << "xori x" << rd << " x" << rs1 << " " << imm << '\n';
                    regs[rd] = regs[rs1] ^ imm;
                    break;
                case 0x5:
                    if ((imm & 0xf00) == 0) { // TODO: check!
                        std::cout << "slri x" << rd << " x" << rs1 << " " << imm << '\n';
                        regs[rd] = regs[rs1] >> imm;
                    } else {
                        std::cout << "srai x" << rd << " x" << rs1 << " " << (imm & 0x1f) << '\n';
                        regs[rd] = regs[rs1] >> (imm & 0x1f);
                    }
                    break;
                case 0x6:
                    std::cout << "ori x" << rd << " x" << rs1 << " " << imm << '\n';
                    regs[rd] = regs[rs1] | imm;
                    break;
                case 0x7:
                    std::cout << "andi x" << rd << " x" << rs1 << " " << imm << '\n';
                    regs[rd] = regs[rs1] & imm;
                    break;
            }
            break;
        case 0x17: // auipc
            imm = ((instr >> 12) << 12);
            std::cout << "auipc x" << rd << " " << imm << '\n';
            regs[rd] = pc * 4 + imm;
            break;
        case 0x23:                      // Save Instructions
            imm = (imm >> 5) << 5 | rd; // Get immediate field
            switch (funct3) {
                case 0x0:
                    std::cout << "sb x" << rs2 << " " << imm << "("
                              << "x" << rs1 << ")" << '\n';
                    save(regs[rs2], regs[rs1] + imm, 1);
                    break;
                case 0x1:
                    std::cout << "sh x" << rs2 << " " << imm << "("
                              << "x" << rs1 << ")" << '\n';
                    save(regs[rs2], regs[rs1] + imm, 2);
                    break;
                case 0x2:
                    std::cout << "sw x" << rs2 << " " << imm << "("
                              << "x" << rs1 << ")" << '\n';
                    save(regs[rs2], regs[rs1] + imm, 4);
                    // std::cout << "Memory saved " << (unsigned int)
                    // mem[regs[rs1]+imm] << " at address: " << regs[rs1]+imm
                    // << '\n'; //REMOVE
                    break;
            }
            break;
        case 0x33:
            switch (funct3) {
                case 0x0:
                    if ((instr >> 25) == 0) {
                        std::cout << "add x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                        regs[rd] = regs[rs1] + regs[rs2];
                    } else {
                        std::cout << "sub x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                        regs[rd] = regs[rs1] - regs[rs2];
                    }
                    break;
                case 0x1:
                    std::cout << "sll x" << rd << " x" << rs1 << " x" << (regs[rs2] & 0x1f) << '\n';
                    regs[rd] = regs[rs1] << (regs[rs2] & 0x1f);
                    break;
                case 0x2:
                    std::cout << "slt x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                    if (regs[rs1] < regs[rs2]) {
                        regs[rd] = 1;
                    } else {
                        regs[rd] = 0;
                    }
                    break;
                case 0x3:
                    std::cout << "sltu x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                    if (regs[rs1] < (unsigned int)regs[rs2]) {
                        regs[rd] = 1;
                    } else {
                        regs[rd] = 0;
                    }
                    break;
                case 0x4:
                    std::cout << "xor x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                    regs[rd] = regs[rs1] ^ regs[rs2];
                    break;
                case 0x5:
                    if ((instr >> 25) == 0) {
                        std::cout << "srl x" << rd << " x" << rs1 << " x" << (regs[rs2] & 0x1f) << '\n';
                        regs[rd] = ((unsigned int)regs[rs1]) >> (regs[rs2] & 0x1f);
                    } else {
                        std::cout << "sra x" << rd << " x" << rs1 << " x" << (regs[rs2] & 0x1f) << '\n';
                        regs[rd] = regs[rs1] >> (regs[rs2] & 0x1f);
                    }
                    break;
                case 0x6:
                    std::cout << "OR x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                    regs[rd] = regs[rs1] | regs[rs2];
                    break;
                case 0x7:
                    std::cout << "and x" << rd << " x" << rs1 << " x" << rs2 << '\n';
                    regs[rd] = regs[rs1] & regs[rs2];
                    break;
            }
            break;
        case 0x37:
            std::cout << "lui x" << rd << " " << ((instr >> 12) & 0xfffff) << '\n';
            regs[rd] = ((instr >> 12) & 0xfffff) << 12;
            break;
        case 0x63: // Branch case
            offset = ((instr >> 25) << 5) + ((instr >> 7) & 0x1f) - 1;
            if (offset > 0) {
                offset++;
            }
            switch (funct3) {
                case 0x0:
                    std::cout << "beq x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] == regs[rs2]) {
                        pc = pc + (offset / 4) - 1;
                    }
                    break;
                case 0x1:
                    std::cout << "bne x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] != regs[rs2]) {
                        pc = pc + (offset / 4) - 1;
                    }
                    break;
                case 0x4:
                    std::cout << "blt x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] < regs[rs2]) {
                        pc = pc + (offset / 4) - 1;
                    }
                    break;
                case 0x5:
                    std::cout << "bge x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] >= regs[rs2]) {
                        pc = pc + (offset >> 2) - 1;
                    }
                    break;
                case 0x6:
                    std::cout << "bltu x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] < (unsigned)regs[rs2]) {
                        pc = pc + (offset / 4) - 1;
                    }
                    break;
                case 0x7:
                    std::cout << "bgeu x" << rs1 << " x" << rs2 << " " << offset << '\n';
                    if (regs[rs1] >= (unsigned)regs[rs2]) {
                        pc = pc + (offset >> 2) - 1;
                    }
                    break;
            }
            break;
        case 0x67: // jalr
            std::cout << "jalr x" << rd << " x" << rs1 << " " << imm << std::endl;
            regs[rd] = (pc + 1) << 2;
            pc = (regs[rs1] + imm) >> 2;
            pc = pc - 1;
            break;
        case 0x6f: // jal
            // Get imm
            imm = (instr >> 30) << 20;                  // Get instr[20]
            imm = imm | (((instr >> 12) & 0xff) << 12); // Get instr[19:12]
            imm = imm | (((instr >> 20) & 0x1) << 11);  // Get instr[11]
            imm = imm | (((instr >> 21) & 0x3ff) << 1); // Get instr[10:1]
            std::cout << "jal x" << rd << " " << imm << std::endl;
            regs[rd] = (pc + 1) << 2;
            pc = pc + (imm >> 2) - 1; // Because of inc after switch
            break;
        case 0x73:
            std::cout << "Ecall - Exiting program" << '\n';
            ecall = true;
            break;
    }
    regs[0] = 0; // jalr x0 xX is not supposed to cast exception, so this is
                 // the easier way
    pc++;        // Increment program counter
}

void RV32ISim::printRegisters() {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << " " << regs[i] << std::endl;
    }
}

void RV32ISim::printProgram() {
    std::cout << "Program" << '\n' << std::endl;
    for (int i = 0; i < length; i++) {
        int instr;
        load(instr, 4 * i, 4);
        printAsHex(instr);
        std::cout << "" << '\n';
    }
}

bool RV32ISim::save(int w, unsigned int idx, unsigned int bytes) {
    for (unsigned int i = 0; i < bytes; i++) {
        mem[idx + i] = (unsigned char)(w >> (8 * i)) & 0xff;
    }
    return true;
}

bool RV32ISim::load(int& w, unsigned int sp, unsigned int bytes, bool u) {
    w = 0;
    for (unsigned int i = 0; i < bytes; i++) {
        w = w | ((unsigned int)mem[sp + i]) << (8 * i);
    }

    if (u != true) {
        if ((mem[sp + bytes - 1] >> 7) == 1) { // For sign extension
            for (int i = 4; i > bytes; i--) {
                w = w | 0xff << (8 * (i - 1));
            }
        }
    }
    return true;
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
