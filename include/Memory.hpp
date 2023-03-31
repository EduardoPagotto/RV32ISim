#pragma once
#include "Device.hpp"
#include "string"

class Memory : public Device {
  public:
    Memory(const uint32_t& start, const uint32_t& size, const uint8_t& status);
    virtual ~Memory();

    virtual bool store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes) override;
    virtual bool load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u = false) override;
    virtual void fill(const uint8_t& val) override;
    virtual inline bool okRead(const uint32_t& address, const uint8_t& size) const override {
        return (status & DSTAT_ENABLED) && (address >= start) && (address < top + size);
    }

    virtual inline bool okWrite(const uint32_t& address, const uint8_t& size) const {
        return ((status & (DSTAT_READWRITE | DSTAT_ENABLED)) == 0x5) && (address >= start) && ((address + size) < top);
    }

    uint32_t start = 0x0000;
    uint32_t top = 0x4000;
};
