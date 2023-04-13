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

    void step();
    void reset();

    void printRegisters();
    // void printProgram();

  private:
    // uint32_t cpu_pc; // Program counter
    uint32_t* regs; //
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

    uint32_t pc;

    std::vector<std::string> alias;

    PipelineState state;

    // void printAsHex(unsigned int instr);

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
