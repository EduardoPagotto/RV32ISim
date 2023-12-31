#pragma once
#include "defs.hpp"

class InstructionType {
  protected:
    OpCodeSet opcode{OpCodeSet::INVALID};

  public:
    InstructionType(const OpCodeSet& o) : opcode(o) {}
    virtual ~InstructionType() = default;
    virtual void step() = 0;
    // virtual void memoryAccess(Bus& bus);

  protected:
    static inline uint8_t calcRd(const uint32_t& i) { return ((i >> 7) & 0x1f); }
    static inline uint8_t calcFunct3(const uint32_t& i) { return (i >> 12) & 0x7; }
    static inline uint8_t calcRs1(const uint32_t& i) { return (i >> 15) & 0x1f; }
    static inline uint8_t calcRs2(const uint32_t& i) { return (i >> 20) & 0x1f; }
    static inline uint8_t calcFunct7(const uint32_t& i) { return ((i >> 25) & 0x7f); }
};
