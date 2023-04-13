#pragma once

#include "Bus.hpp"
#include "Execute.hpp"
// #include "Decode.hpp"

class RV32ISim {
  public:
    RV32ISim(Bus* bus);
    virtual ~RV32ISim();
    void play();

  private:
    uint32_t regs[32];
    Bus* bus = nullptr;

    Execute* execute;
};
