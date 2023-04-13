#pragma once
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

enum class PipelineState { Fetch, Decode, Execute, MemoryAccess, WriteBack };

class Controller {
  public:
    Controller() = default;
    virtual ~Controller() = default;
    const bool resetSignal() const { return false; }
    const bool getBranchAddressValid() const { return false; }
    const uint32_t getBranchAddress() const { return 0; }

    bool shoulStall(PipelineState state) { return !(state == this->state); }

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

  private:
    bool __beginTrap = false;
    bool __beginTrapReturn = false;
    uint32_t val = 10;
};