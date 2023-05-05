#pragma once
#include "Bus.hpp"
#include "Decode.hpp"

class Execute : public PipelineStage {
  public:
    Execute(CSR* c, Bus* bus, Decode* d, uint32_t regs[])
        : PipelineStage(PipelineState::Execute, c), bus(bus), decode(d), regs(regs) {}

    virtual ~Execute() = default;

    virtual void commit() override { this->done = this->data; }
    virtual void step() override;
    virtual void reset() override{};
    inline const ExecuteData& get() const { return data; }

  private:
    uint32_t* regs;
    Bus* bus = nullptr;
    Decode* decode;

    ExecuteData data;
    ExecuteData done;
};
