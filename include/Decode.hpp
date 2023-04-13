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

  private:
    uint8_t funct3;
    uint8_t funct7;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t opcode;

    int32_t instr;

    // int32_t iImm;
    // int32_t sImm;
    // int32_t uImm;
    // int32_t jImm;
    // int32_t bImm;

    PipelineState state;

    int32_t imm32;

    Controller* crt;
    Fetch* fetch;

    bool returnFromTrap = false;
    int32_t mepc, trap, mtval, mcause;
    uint32_t csrSource, csrAddress;
    bool csrShouldRead, csrShouldWrite;
};