#pragma once
#include "Controller.hpp"
#include "Fetch.hpp"
#include <cstdint>

struct DecodeData {
    DecodeData() = default;
    ~DecodeData() = default;
    DecodeData(const DecodeData& o) = default;

    uint8_t funct3 = 0;
    uint8_t funct7 = 0;
    uint8_t rd = 0;
    uint8_t rs1 = 0;
    uint8_t rs2 = 0;
    OpCodeSet opcode = OpCodeSet::INVALID;
    OpCodeSetSystem opcodeSys = OpCodeSetSystem::INVALID;

    int32_t instr = 0;
    int32_t imm32 = 0;
    uint32_t pc = 0;
    uint32_t pcPlus4 = 0;
};

class Decode {
  public:
    Decode(Controller* c, Fetch* f);
    virtual ~Decode() = default;

    void step();
    void reset();

    inline void commit() { this->done = this->data; }
    inline const DecodeData& getData() const { return done; }
    std::string printValue(const uint32_t& indice, const uint32_t value);

  private:
    DecodeData data;
    DecodeData done;
    PipelineState state;
    Controller* crt;
    Fetch* fetch;
};