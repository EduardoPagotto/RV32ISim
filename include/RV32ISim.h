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

class RV32ISim {
  public:
    /**
     * @brief Construct a new RV32ISim object
     *
     */
    RV32ISim(Bus* bus);

    /**
     * @brief Destroy the RV32ISim object
     *
     */
    ~RV32ISim();

    /**
     * @brief Function that read instructions from a binary file to main memory
     *
     * @param filepath the path to search for binary file
     * @return true indication if file was found \
     * @return false indication fail to wuite
     */
    bool readFromFile(const char* filepath);

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
     * to by pc. This function contain the implementation of the RISC-V 32RVI
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
    inline bool hasNext() const { return ((pc < length) & !ecall); } // TODO: AQI!!!! ver esta variaveisZZ

    void printRegisters();
    void printProgram();

  private:
    uint32_t length;   // Program length in number of instructions
    uint32_t capacity; // Memory capacity
    uint32_t pc;       // Program counter
    uint32_t regs[32]; //
    bool ecall;        // For program termination

    // unsigned char* mem;    // Main memory

    Bus* bus = nullptr;

    /**
     * @brief Test function used to print content of memory
     *
     * @param instr instruction
     */
    void printAsHex(unsigned int instr);
};
