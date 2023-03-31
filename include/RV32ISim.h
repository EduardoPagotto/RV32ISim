/**
 * @file RV32ISim.h
 * @author Eduardo Pagotto
 * @brief
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "include/Bus.hpp"

struct Instr {
    uint32_t instr;
    uint32_t opcode;
    uint32_t funct3;
    uint32_t imm;
    uint32_t rd;
    uint32_t rs1;
    uint32_t rs2;

    Instr(const uint32_t& instr)
        : instr(instr), opcode(instr & 0x7f), funct3((instr >> 12) & 0x7), imm((instr >> 20)), rd((instr >> 7) & 0x1f),
          rs1((instr >> 15) & 0x1f), rs2((instr >> 20) & 0x1f) {}
};

class RV32ISim {
  public:
    RV32ISim(Bus* bus);
    ~RV32ISim();

    /**
     * @brief Dumps the registry content at current state to file
     *
     * @param filepath where to place file
     * @return true indication if write was successful \
     * @return false indication fail to write
     */
    bool writeToFile(const char* filepath);

    /**
     * @brief Step through the program by decoding and executing next instruction pointed
     * to by cpu_pc. This function contain the implementation of the RISC-V 32RVI
     * instruction subset For different subset/functionality this function must be
     * overwritten.
     */
    void step();

    /**
     * @brief indication if program is ended
     *
     * @return true \
     * @return false
     */
    inline bool hasNext() const { return ((bus->hasData(cpu_pc)) & !ecall); } // TODO: AQI!!!! ver esta variaveisZZ

    void printRegisters();
    void printProgram();

  private:
    uint32_t cpu_pc;   // Program counter
    uint32_t regs[32]; //
    bool ecall;        // For program termination
    Bus* bus = nullptr;

    /**
     * @brief Test function used to print content of memory
     *
     * @param instr instruction
     */
    void printAsHex(unsigned int instr);

    void loadRegister(const Instr& i);
    void ulai(const Instr& i);
    void auipc(const Instr& i);
    void saveRegister(const Instr& i);
    void ula(const Instr& i);
    void lui(const Instr& i);
    void branchCase(const Instr& i);
    void jalr(const Instr& i);
    void jal(const Instr& i);
};
