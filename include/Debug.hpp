#pragma once
// #include "defs.hpp"
#include <stdint.h>
#include <string>
#include <vector>

class Debug {
  public:
    Debug();
    virtual ~Debug() = default;

    void printAsHex(const uint32_t& addr, const uint32_t& instr);
    std::string printValue(const uint32_t& indice, const uint32_t value);
    // std::string debugCommandRegs(const DecodeData& d);

    void printPC(const uint32_t& val);
    void printRegtoMemory(const uint8_t& index, const uint32_t& valRS, const uint32_t& address);
    void printRegVal(const uint8_t& index, const uint32_t& value);
    void printAddress(const uint32_t& addr);
    void printRegisters(uint32_t* regs);
    bool writeToFile(uint32_t* regs, const char* filepath);

  private:
    std::vector<std::string> alias;
};