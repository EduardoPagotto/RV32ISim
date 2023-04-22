#pragma once
#include "Fetch.hpp"

class Decode : public PipelineStage {
  public:
    Decode(CSR* c, Fetch* f) : PipelineStage(PipelineState::Decode, c), fetch(f) {}
    virtual ~Decode() = default;

    virtual void step() override;
    virtual void reset() override{};
    virtual void commit() override { this->done = this->data; }

    inline const DecodeData& getData() const { return done; }

  private:
    DecodeData data;
    DecodeData done;
    Fetch* fetch;
};