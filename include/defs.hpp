#pragma once

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

// OpCode Interrupt
#define OPC_ECALL 0b00000000000000000000000001110011  // syscall
#define OPC_EBREAK 0b00000000000100000000000001110011 // syscall
#define OPC_SRET 0b00010000001000000000000001110011   // trap return
#define OPC_MRET 0b00110000001000000000000001110011   // trap return
#define OPC_WFI 0b00010000010100000000000001110011    // Wait for Interruption

// OpCode CSRs
#define OPC_CSRRW 0x01 // in func3
#define OPC_CSRRS 0x02
#define OPC_CSRRC 0x03
#define OPC_CSRRWI 0x05
#define OPC_CSRRSI 0x06
#define OPC_CSRRCI 0x07

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
