#pragma once
#include "Bus.hpp"
#include <stdint.h>

class Riscv32 {
  private:
    uint32_t x[32] = {0};
    Bus* bus = nullptr;

  public:
    Riscv32(Bus* bus) : bus(bus) {}
    virtual ~Riscv32() {}
    void step() {}
};
