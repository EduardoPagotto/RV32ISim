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

// enum class PipelineState { Fetch, Decode, Execute, MemoryAccess, WriteBack };

enum class OpCode : __uint8_t {
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

enum class OpCodeInt : __uint16_t {

    ECALL = 0x00,  // syscall
    EBREAK = 0x08, // syscall
    //
    SRET = 0x102, // 0b000100000010, // trap return
    MRET = 0x302, // 0b001100000010, // trap return
    WFI = 0x105,  // 0b000100000101, // Wait for Interruption
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

enum class OpCodeCSR : __uint8_t {
    CSRRW = 0x01, // in func3
    CSRRS = 0x02,
    CSRRC = 0x03,
    CSRRWI = 0x05,
    CSRRSI = 0x06,
    CSRRCI = 0x07
};

// enum class CPUState : __uint8_t { Pipeline, Trap };

// struct FetchData {
//     FetchData() = default;
//     FetchData(const FetchData& o) = default;
//     ~FetchData() = default;
//     uint32_t pc = 0;
//     uint32_t pcPlus4 = 0;
//     uint32_t instr = 0;
// };

// struct DecodeData {
//     DecodeData() = default;
//     DecodeData(const DecodeData& o) = default;
//     DecodeData(const FetchData& f) { fetch = f; }
//     ~DecodeData() = default;
//     FetchData fetch;
//     uint8_t funct3 = 0;
//     uint8_t funct7 = 0;
//     uint8_t rd = 0;
//     uint8_t rs1 = 0;
//     uint8_t rs2 = 0;
//     int32_t imm32 = 0;
//     OpCode opcode = OpCode::INVALID;
//     OpCodeSystem opcodeSys = OpCodeSystem::INVALID;
// };

// struct ExecuteData {
//     ExecuteData() = default;
//     ExecuteData(const ExecuteData& o) = default;
//     ExecuteData(const DecodeData& d) { decode = d; };
//     ~ExecuteData() = default;
//     DecodeData decode;
//     MemoryAccessWidth width;
//     bool valSigned;
//     uint32_t address;
//     uint32_t valueRS1;
//     uint32_t valueRS2;
//     uint32_t valueRD;
// };

// struct MemoryAccessData {
//     MemoryAccessData() = default;
//     MemoryAccessData(const MemoryAccessData& o) = default;
//     MemoryAccessData(const ExecuteData& e) { execute = e; };
//     ~MemoryAccessData() = default;
//     ExecuteData execute;
//     uint32_t value;
//     bool isValid;
// };
