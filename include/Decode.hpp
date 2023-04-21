#pragma once
#include "Controller.hpp"
#include "Fetch.hpp"
#include <cstdint>

class Decode {
  public:
    Decode(Controller* c, Fetch* f);
    virtual ~Decode() = default;

    void step();
    void reset();

    inline void commit() { this->done = this->data; }
    inline const DecodeData& getData() const { return done; }
    std::string printValue(const uint32_t& indice, const uint32_t value);

  private:
    DecodeData data;
    DecodeData done;
    PipelineState state;
    Controller* crt;
    Fetch* fetch;
};