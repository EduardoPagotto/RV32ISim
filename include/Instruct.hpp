#pragma once
#include "Bus.hpp"
#include "Controller.hpp"
#include "defs.hpp"
#include "mmu/MMU.hpp"

struct WriteBackData {
    uint8_t rd;
    uint32_t value;
    bool isValid;
};

class Instruct {
  protected:
    uint32_t opcode;

  public:
    Instruct(const uint32_t& o) : opcode(o) {}
    virtual ~Instruct() = default;
    virtual void execute(Controller& controller) = 0;
    virtual const WriteBackData memoryAccess(Bus& bus, MMU& mmu, Controller& controller) = 0;

    void writeBack(const WriteBackData& w, uint32_t* x) {
        if (w.isValid) {
            if (w.rd != 0) {
                x[w.rd] = w.value;
                Debug::printRegVal(w.rd, w.value);
            } else {
                x[0] = 0;
            }
        }
    }

    static inline uint8_t calcRd(const uint32_t& i) { return ((i >> 7) & 0x1f); }
    static inline uint8_t calcFunct3(const uint32_t& i) { return (i >> 12) & 0x7; }
    static inline uint8_t calcRs1(const uint32_t& i) { return (i >> 15) & 0x1f; }
    static inline uint8_t calcRs2(const uint32_t& i) { return (i >> 20) & 0x1f; }
    static inline uint8_t calcFunct7(const uint32_t& i) { return ((i >> 25) & 0x7f); }
    static inline uint8_t calcFunct5(const uint32_t i) { return i >> 27; }
};
