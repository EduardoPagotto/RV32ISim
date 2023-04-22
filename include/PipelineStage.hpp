#pragma once
#include "Controller.hpp"

class PipelineStage {
  public:
    PipelineStage(PipelineState s, Controller* c) : state(s), crt(c) {}
    virtual ~PipelineStage() {}

    virtual void step() = 0;
    virtual void commit() = 0;
    virtual void reset() = 0;

  protected:
    Controller* crt;
    PipelineState state;
};