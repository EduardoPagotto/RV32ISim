#pragma once
#include "Fetch.hpp"
#include "Trap.hpp"

class Decode : public PipelineStage {
  public:
    Decode(CSR* c, Fetch* f, Trap* t) : PipelineStage(PipelineState::Decode, c), fetch(f), trap(t) {}
    virtual ~Decode() = default;

    virtual void step() override;
    virtual void reset() override{};
    virtual void commit() override { this->done = this->data; }

    inline const DecodeData& getData() const { return done; }

  private:
    DecodeData data;
    DecodeData done;
    Fetch* fetch;
    Trap* trap;
};