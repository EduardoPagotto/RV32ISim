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

#include "Bus.hpp"

struct Instr {
    uint8_t funct3;
    uint8_t funct7;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t opcode;

    int32_t instr;

    int32_t iImm;
    int32_t sImm;
    int32_t uImm;
    int32_t jImm;
    int32_t bImm;

    Instr(const int32_t& instr) {

        this->instr = instr;
        this->opcode = instr & 0x7f;
        this->rs1 = (instr >> 15) & 0x1f;
        this->rs2 = (instr >> 20) & 0x1f;
        this->rd = ((instr >> 7) & 0x1f);

        this->funct3 = (instr >> 12) & 0x7;
        this->funct7 = ((instr >> 25) & 0x7f);

        this->iImm = instr >> 20; // instr[11:0] [ 0xFFF => 2048 => -1024:1023 ]

        this->sImm = ((instr >> 25) << 5) // instr[11:5]  [ 0xFFF => 2048 => -1024:1023 ]
                     | this->rd;          // instr[4:0]

        this->uImm = ((this->instr >> 12) & 0xfffff) << 12; // instr[31:12] [ 0xFFFFFFFF => 4.294.967.295 ]

        this->jImm = ((this->instr >> 30) << 20) |          //  instr[20] [ 0xFFFFF => 1.048.575 =>  524.288:524.287 ]
                     (((this->instr >> 12) & 0xff) << 12) | //  instr[19:12]
                     (((this->instr >> 20) & 0x1) << 11) |  //  instr[11]
                     (((this->instr >> 21) & 0x3ff) << 1);  //  instr[10:1]

        // this->bImm = ((this->instr & 0x800) >> 11) | //  instr[12] // [0x1FFF => 8191 => -4096:4095]
        //              ((this->instr & 0x80) << 4) |   //  instr[11]
        //              ((this->instr >> 25) << 5) |    //  instr[10:5]
        //              ((this->instr >> 7) & 0x1e);    //  instr[4:1]

        this->bImm = ((this->instr >> 25) << 5) + ((this->instr >> 7) & 0x1f) - 1;
        if (this->bImm > 0) {
            this->bImm++;
        }
    }
};

class RV32ISim {
  public:
    RV32ISim(Bus* bus);
    virtual ~RV32ISim();

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

    std::string printCommandRegs(const std::string& com, const Instr& i);

    std::vector<std::string> alias;
    std::string printIndexValue(const uint32_t& indice);
    std::string printValue(const uint32_t& indice, const uint32_t value);
};
