#pragma once
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

enum class PipelineState { Fetch, Decode, Execute, MemoryAccess, WriteBack };

enum class OpCodeSet : __uint8_t {
    LOAD = 0x03,
    ULAI = 0x13,
    AUIPC = 0x17,
    SAVE = 0x23,
    ULA = 0x33,
    LUI = 0x37,
    BRANCH = 0x63,
    JALR = 0x67,
    JAL = 0x6f,
    FENCE = 0x0f,
    SYSTEM = 0x73,
    INVALID = 0xFF
};

enum class OpCodeSetSystem : __uint8_t {
    ECALL = 0b0000,
    EBREAK = 0b1000,
    CSRRW = 0b0001,
    CSRRS = 0b0010,
    CSRRC = 0b0011,
    CSRRWI = 0b0101,
    CSRRSI = 0b0110,
    CSRRCI = 0b0111,
    INVALID = 0b1111
};

class Controller {
  public:
    Controller() = default;
    virtual ~Controller() = default;
    const bool resetSignal() const { return false; }
    const bool getBranchAddressValid() { return branchAddressValid; }
    const uint32_t getBranchAddress() {
        branchAddressValid = false;
        return branchAddress;
    }

    void setBranchAddress(const uint32_t& addr) {
        branchAddress = addr;
        branchAddressValid = true;
    }

    // bool shoulStall(PipelineState state) { return !(state == this->state); } // FIXME: no pipeline
    bool shoulStall(PipelineState state) { return false; }

    PipelineState state = PipelineState::Fetch;

    bool beginTrap() { return __beginTrap; }
    bool beginTrapReturn() { return __beginTrapReturn; }

    void printAsHex(const uint32_t& addr, const uint32_t& instr);

    template <typename T>
    inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
        using namespace std;
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
        return ss.str();
    }

    bool ecall = false; // For program termination

  private:
    bool branchAddressValid = false;
    uint32_t branchAddress = 0;

    bool __beginTrap = false;
    bool __beginTrapReturn = false;
    uint32_t val = 10;
};