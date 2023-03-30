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

class RV32ISim {
  public:
    /**
     * @brief Construct a new RV32ISim object
     *
     */
    RV32ISim();

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
    inline bool hasNext() const { return ((pc < length) & !ecall); }

    void printRegisters();
    void printProgram();

  private:
    int length;   // Program length in number of instructions
    int pc;       // Program counter
    int regs[32]; //
    bool ecall;   // For program termination

    unsigned int capacity; // Memory capacity
    unsigned char* mem;    // Main memory

    /**
     * @brief Saves 1-4 bytes from the integer w to memory
     * Byte ordering is little endian format
     *
     * @param w integer to be saved
     * @param idx memory idx to save least sig. byte to
     * @param bytes number of bytes to save
     * @return true
     * @return false
     */
    bool save(int w, unsigned int idx, unsigned int bytes);

    /**
     * @brief Single function to load from memory
     *
     * @param w register to place content passed by reference
     * @param sp pointer to location of least significant byte (little endian format)
     * @param bytes number of bytes to load
     * @param u Default false is unsigned
     * @return true
     * @return false
     */
    bool load(int& w, unsigned int sp, unsigned int bytes, bool u = false);

    /**
     * @brief Test function used to print content of memory
     *
     * @param instr instruction
     */
    void printAsHex(unsigned int instr);
};
