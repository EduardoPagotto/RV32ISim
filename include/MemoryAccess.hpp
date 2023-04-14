#pragma once
#include "Bus.hpp"
#include "CSR.hpp"
#include "Controller.hpp"
#include "Execute.hpp"

class MemoryAccess {
  public:
    MemoryAccess(Controller* c, Bus* b, Execute* e, CSR* csr);
    virtual ~MemoryAccess() = default;

    void reset() {}
    void commit() {}
    void step();

  private:
    PipelineState state;
    Controller* crt;
    Bus* bus;
    Execute* execute;
    CSR* csr;
};