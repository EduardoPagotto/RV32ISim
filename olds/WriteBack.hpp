#pragma once
#include "Bus.hpp"
#include "MemoryAccess.hpp"

class WriteBack : public PipelineStage {
  public:
    WriteBack(CSR* c, Bus* b, MemoryAccess* m, uint32_t regs[])
        : PipelineStage(PipelineState::WriteBack, c), bus(b), memory(m), regs(regs) {}

    virtual ~WriteBack() = default;

    virtual void reset() override {}
    virtual void commit() override {}
    virtual void step() override;

  private:
    Bus* bus;
    MemoryAccess* memory;
    uint32_t* regs;
};