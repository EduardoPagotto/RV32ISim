#pragma once

#include "Bus.hpp"
#include "Controller.hpp"
#include "Decode.hpp"
#include "Execute.hpp"
#include "Fetch.hpp"

class RV32ISim {
  public:
    RV32ISim(Bus* bus);
    virtual ~RV32ISim();
    void play();
    void printRegisters();

  private:
    uint32_t regs[32];
    Controller crt;

    Bus* bus = nullptr;
    Fetch* fetch = nullptr;
    Decode* decode = nullptr;
    Execute* execute = nullptr;
};
