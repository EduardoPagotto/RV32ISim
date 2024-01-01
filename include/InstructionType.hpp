#pragma once
#include "Bus.hpp"
#include "Controller.hpp"
#include "defs.hpp"

struct WriteBackData {
    uint8_t rd;
    uint32_t value;
    bool isValid;
};

class InstructionType {
  protected:
    OpCode opcode{OpCode::INVALID};

  public:
    InstructionType(const OpCode& o) : opcode(o) {}
    virtual ~InstructionType() = default;
    virtual void execute(Controller& controller) = 0;
    virtual const WriteBackData memoryAccess(Bus& bus, Controller& controller) = 0;

    static inline uint8_t calcRd(const uint32_t& i) { return ((i >> 7) & 0x1f); }
    static inline uint8_t calcFunct3(const uint32_t& i) { return (i >> 12) & 0x7; }
    static inline uint8_t calcRs1(const uint32_t& i) { return (i >> 15) & 0x1f; }
    static inline uint8_t calcRs2(const uint32_t& i) { return (i >> 20) & 0x1f; }
    static inline uint8_t calcFunct7(const uint32_t& i) { return ((i >> 25) & 0x7f); }
};
