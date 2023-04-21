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

    void commit() { this->done = this->data; }

    void step();
    void reset();

    void printRegisters();
    // void printProgram();

    inline const ExecuteData& get() const { return data; }

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
    OpCodeSet opcode;
    OpCodeSetSystem opcodeSys;
    int32_t instr;
    int32_t imm32;

    uint32_t pc;
    uint32_t pcPlus4;

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

    void setSystem();

    std::string printCommandRegs(const std::string& com, const int32_t& imm);

    // bool returnFromTrap = false;
    // bool csrShouldRead, csrShouldWrite;
    // int32_t mepc, trap, mtval, mcause;
    // uint32_t csrSource, csrAddress;

    ExecuteData data;
    ExecuteData done;
};
