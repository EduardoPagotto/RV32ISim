#pragma once
#include "RegisterBank.hpp"

class PipelineStage {
  public:
    PipelineStage() { regs = new RegisterBank(); }
    virtual ~PipelineStage() {
        delete regs;
        regs = nullptr;
    }

    virtual void compute() = 0;
    virtual void latchNext() = 0;
    void reset() { this->regs->reset(); }

  private:
    RegisterBank* regs;
};