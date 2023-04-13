#pragma once

#include "Bus.hpp"
#include "Controller.hpp"
#include "Decode.hpp"

class Execute {
  public:
    Execute(Controller* c, Bus* bus, Decode* d, uint32_t regs[]);
    virtual ~Execute();

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

    void reset();

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
    uint32_t cpu_pc; // Program counter
    uint32_t* regs;  //
    bool ecall;      // For program termination
    Bus* bus = nullptr;
    Controller* crt;
    Decode* decode;

    uint8_t funct3;
    uint8_t funct7;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t opcode;
    int32_t instr;
    int32_t imm32;

    std::vector<std::string> alias;

    PipelineState state;

    void printAsHex(unsigned int instr);

    void loadRegister();
    void ulai();
    void auipc();
    void saveRegister();
    void ula();
    void lui();
    void branchCase();
    void jalr();
    void jal();

    std::string printCommandRegs(const std::string& com);

    std::string printIndexValue(const uint32_t& indice);
    std::string printValue(const uint32_t& indice, const uint32_t value);
};
