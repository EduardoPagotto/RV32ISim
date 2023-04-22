#pragma once
#include "CSR.hpp"

class PipelineStage {
  public:
    PipelineStage(PipelineState s, CSR* c) : state(s), csr(c) {}
    virtual ~PipelineStage() = default;
    virtual void step() = 0;
    virtual void commit() = 0;
    virtual void reset() = 0;

  protected:
    CSR* csr;
    PipelineState state;
};