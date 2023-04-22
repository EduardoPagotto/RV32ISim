#pragma once
#include "defs.hpp"

// const MSTATUS_MASK = (1 << 3) | (1 << 7);

class CSR {
  public:
    CSR() = default;
    virtual ~CSR() = default;

    void step() { mem.cycles++; }
    void commit() {}

    uint32_t read(uint32_t address);
    void write(uint32_t address, uint32_t value);

  public: // FIXME em Trap
    CsrMem mem;
};
