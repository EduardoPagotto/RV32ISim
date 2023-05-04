#pragma once
#include "Bus.hpp"
#include <cstdint>

// #define OPC_LOAD 0x03
// #define OPC_ULAI 0x13
// #define OPC_AUIPC 0x17
// #define OPC_SAVE 0x23
// #define OPC_ULA 0x33
// #define OPC_LUI 0x37
// #define OPC_BRANCH 0x63
// #define OPC_JALR 0x67
// #define OPC_JAL 0x6f
// #define OPC_FENCE 0x0f
// #define OPC_SYSTEM 0x73
// #define OPC_INVALID 0xFF

// #define OPC_MASK 0x0000007f        // mask bits [6:0]
// #define OPC_MASK_RD 0x00000f80     // mask bits [11:7]
// #define OPC_MASK_FUNCT3 0x0007000  // mask bits [14:12]
// #define OPC_MASK_RS1 0x000f8000    // mask bits [19:15]
// #define OPC_MASK_RS2 0x01f00000    // mask bits [24:20]
// #define OPC_MASK_FUNCT7 0xfe000000 // mask bits [31:25]

// // syscall
// #define SYS_ECALL 0x0073
// #define SYS_EBREAK 0x0873
// // trap return
// #define SYS_SRET 0x01020073
// #define SYS_MRET 0x03020073
// // interrupt managent instrucion
// #define SYS_WFI 0x15
// // CSR in funct3
// #define SYS_CSRRW 0x00001073
// #define SYS_CSRRS 0x00002073
// #define SYS_CSRRC 0x00003073
// #define SYS_CSRRWI 0x00005073
// #define SYS_CSRRSI 0x00006073
// #define SYS_CSRRCI 0x00007073
// // not existe in doc
// #define SYS_INVALID 0xff

// // TODO: implementar
// enum class TypeField : __uint8_t { CSR, Memory, Fetch, Decode, Execute };

// // TODO: implementar
// struct CSRFields {
//     TypeField type;
//     uint8_t rs1Index;
//     uint8_t rdIndex;
//     int32_t address;
//     uint32_t rs1Value;
//     uint32_t rdValue;
// };

// // TODO: implementar
// struct MemoryFields {
//     TypeField type;
//     uint32_t address;
//     uint32_t value;
//     uint8_t memSize;
//     bool isSigned;
// };

// // TODO: implementar
// struct FetchFields {
//     TypeField type;
//     uint32_t pc;
//     uint32_t pcPlus4;
//     uint32_t instruction;
// };

// // TODO: implementar
// struct DecodeFilelds {
//     TypeField type;
//     uint32_t pc = 0;
//     uint32_t pcPlus4 = 0;
//     int32_t instr = 0;
//     int32_t imm32 = 0;
//     uint8_t funct3 = 0;
//     uint8_t funct7 = 0;
//     uint8_t rd = 0;
//     uint8_t rs1 = 0;
//     uint8_t rs2 = 0;
//     OpCodeSet opcode = OpCodeSet::INVALID;
//     OpCodeSetSystem opcodeSys = OpCodeSetSystem::INVALID;
// };

// // TODO: implementar
// struct ExecuteFields {
//     TypeField type;
//     uint32_t address;
//     uint32_t index;
//     uint32_t valueRS;
//     uint32_t valueRD;
//     uint8_t memSize;
//     bool valSigned;
//     OpCodeSet opcode;
//     OpCodeSetSystem opcodeSys;
//     uint8_t funct3;
// };

// // TODO: implementar
// union Fields {
//     TypeField type;
//     FetchFields fetch;
//     MemoryFields mem;
//     DecodeFilelds decode;
//     ExecuteFields exec;
//     CSRFields csr;
// };

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
    // syscall
    ECALL = 0x00,
    EBREAK = 0x08,
    // trap return
    SRET = 0x10,
    MRET = 0x11,
    // interrupt managent instrucion
    WFI = 0x18,
    // CSR in funct3
    CSRRW = 0x01,
    CSRRS = 0x02,
    CSRRC = 0x03,
    CSRRWI = 0x05,
    CSRRSI = 0x06,
    CSRRCI = 0x07,
    // not existe in doc
    INVALID = 0xff
};

enum class CPUState : __uint8_t { Pipeline, Trap };

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
    uint8_t index;
    MemoryAccessWidth width;
    bool valSigned;
    uint32_t valueRS;
    uint32_t valueRD;

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
