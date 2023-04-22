#pragma once
#include "defs.hpp"
#include <string>
#include <vector>

class PrintAs {
  public:
    PrintAs();
    virtual ~PrintAs() = default;

    void printAsHex(const uint32_t& addr, const uint32_t& instr);
    std::string printValue(const uint32_t& indice, const uint32_t value);
    std::string debugCommandRegs(const DecodeData& d);

  private:
    std::vector<std::string> alias;
};