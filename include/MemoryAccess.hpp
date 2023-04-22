#pragma once
#include "Bus.hpp"
#include "Execute.hpp"

class MemoryAccess : public PipelineStage {
  public:
    MemoryAccess(CSR* c, Bus* b, Execute* e) : PipelineStage(PipelineState::MemoryAccess, c), bus(b), execute(e) {}
    virtual ~MemoryAccess() = default;

    virtual void reset() override {}
    virtual void step() override;
    virtual void commit() override { this->done = this->data; }

    inline const MemoryAccessData& get() const { return done; }

  private:
    Bus* bus;
    Execute* execute;
    MemoryAccessData data;
    MemoryAccessData done;
};