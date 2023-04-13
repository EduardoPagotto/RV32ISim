#pragma once
#include <cstdint>

class Decode {
  public:
    Decode(const int32_t& instr);
    virtual ~Decode() = default;

    void compute();

  public:
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
};