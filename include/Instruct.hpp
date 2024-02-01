#pragma once
#include "Bus.hpp"
#include "Controller.hpp"
#include "mmu/MMU.hpp"

// OpCode RV32I
#define OPC_LOAD 0x03
#define OPC_ULAI 0x13
#define OPC_AUIPC 0x17
#define OPC_SAVE 0x23
#define OPC_ULA 0x33
#define OPC_LUI 0x37
#define OPC_BRANCH 0x63
#define OPC_JALR 0x67
#define OPC_JAL 0x6f
#define OPC_FENCE 0x0f
#define OPC_SYSTEM 0x73

// OpCode RV32A
#define OPC_ATOMIC 0x2f

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
