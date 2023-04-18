#pragma once
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

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

struct FetchData {
    FetchData() = default;
    FetchData(const FetchData& o) = default;
    ~FetchData() = default;
    uint32_t pc = 0;
    uint32_t pcPlus4 = 0;
    uint32_t instruction = 0;
};

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

struct ExecuteData {
    ExecuteData() = default;
    ExecuteData(const ExecuteData& o) = default;
    ~ExecuteData() = default;

    uint32_t address;
    uint32_t index;
    uint8_t memSize;
    bool valSigned;
    uint32_t valueRS2;

    OpCodeSet opcode;
    OpCodeSetSystem opcodeSys;
    uint8_t funct3;
};

struct MemoryAccessData {
    MemoryAccessData() = default;
    ~MemoryAccessData() = default;
    MemoryAccessData(const MemoryAccessData& o) = default;

    uint32_t value;
    uint32_t rd;
    bool isValid;
};

class Controller {
  public:
    Controller();
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

    bool ecall = false; // For program termination

    std::vector<std::string> alias;
    std::string printValue(const uint32_t& indice, const uint32_t value);

    std::string debugCommandRegs(const DecodeData& d);

  private:
    bool branchAddressValid = false;
    uint32_t branchAddress = 0;

    bool __beginTrap = false;
    bool __beginTrapReturn = false;
    uint32_t val = 10;
};