#pragma once
#include "Controller.hpp"
#include "Fetch.hpp"
#include <cstdint>

class Decode {
  public:
    Decode(Controller* c, Fetch* f);
    virtual ~Decode() = default;

    void step();
    void reset();

    inline const uint8_t& getFunct3() const { return this->funct3; }
    inline const uint8_t& getFunct7() const { return this->funct7; }
    inline const uint8_t& getRD() const { return this->rd; }
    inline const uint8_t& getRS1() const { return this->rs1; }
    inline const uint8_t& getRS2() const { return this->rs2; }
    inline const uint8_t& getOpCode() const { return this->opcode; }

    inline const int32_t& getInstruction() const { return this->instr; }
    inline const int32_t& getImm32() const { return this->imm32; }

    inline const uint32_t getPc() const { return this->pc; }

    std::string printValue(const uint32_t& indice, const uint32_t value);

  private:
    uint8_t funct3;
    uint8_t funct7;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t opcode;
    int32_t instr;
    int32_t imm32;

    PipelineState state;

    Controller* crt;
    Fetch* fetch;

    uint32_t pc;

    bool returnFromTrap = false;
    bool csrShouldRead, csrShouldWrite;
    int32_t mepc, trap, mtval, mcause;
    uint32_t csrSource, csrAddress;
};