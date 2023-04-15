#pragma once
#include "Bus.hpp"
#include "CSR.hpp"
#include "Controller.hpp"
#include "Execute.hpp"

struct MemoryAccessData {

    MemoryAccessData() = default;
    ~MemoryAccessData() = default;
    MemoryAccessData(const MemoryAccessData& o) = default;

    uint32_t value;
    uint32_t rd;
    bool isValid;
};

class MemoryAccess {
  public:
    MemoryAccess(Controller* c, Bus* b, Execute* e, CSR* csr);
    virtual ~MemoryAccess() = default;

    void reset() {}
    void step();

    inline void commit() { this->done = this->data; }

    inline const MemoryAccessData& get() const { return done; }

  private:
    PipelineState state;
    Controller* crt;
    Bus* bus;
    Execute* execute;
    CSR* csr;

    MemoryAccessData data;
    MemoryAccessData done;
};