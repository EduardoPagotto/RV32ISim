#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include <fstream>
#include <iomanip>
#include <iostream>

class Debug {
  public:
    // Debug();
    // virtual ~Debug() = default;

    template <typename T>
    static inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
        std::stringstream ss;
        ss << "0x" << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
        return ss.str();
    }

    static void init();
    static void printAsHex(const uint32_t& addr, const uint32_t& instr);
    static std::string printValue(const uint32_t& indice, const uint32_t value);
    // std::string debugCommandRegs(const DecodeData& d);

    static void printPC(const uint32_t& val);
    static void printRegtoMemory(const uint8_t& index, const uint32_t& valRS, const uint32_t& address);
    static void printRegVal(const uint8_t& index, const uint32_t& value);
    static void printAddress(const uint32_t& addr);
    static void printRegisters(uint32_t* regs);
    static bool writeToFile(uint32_t* regs, const char* filepath);
    static void newline();

    inline static std::vector<std::string> alias;
};