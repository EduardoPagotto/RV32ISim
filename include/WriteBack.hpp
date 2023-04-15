#pragma once
#include "Bus.hpp"
#include "Controller.hpp"
#include "MemoryAccess.hpp"

class WriteBack {
  public:
    WriteBack(Controller* c, Bus* b, MemoryAccess* m, uint32_t regs[]);
    virtual ~WriteBack() = default;

    void reset() {}
    void commit() {}
    void step();

  private:
    PipelineState state;
    Controller* crt;
    Bus* bus;
    MemoryAccess* memory;
    uint32_t* regs;
};