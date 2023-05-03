#pragma once

#include "Bus.hpp"
#include "CSR.hpp"
#include "Decode.hpp"
#include "Execute.hpp"
#include "Fetch.hpp"
#include "MemoryAccess.hpp"
#include "Trap.hpp"
#include "WriteBack.hpp"

class RV32ISim {
  public:
    RV32ISim(Bus* bus);
    virtual ~RV32ISim();
    void play();

  private:
    void step();
    void commit();

    uint32_t regs[32];
    CSR csr;

    Bus* bus = nullptr;
    Fetch* fetch = nullptr;
    Decode* decode = nullptr;
    Execute* execute = nullptr;
    MemoryAccess* memory = nullptr;
    WriteBack* writeBack = nullptr;
    Trap* trap = nullptr;
};
