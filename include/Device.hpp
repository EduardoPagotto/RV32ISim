#pragma once
#include <cstdint>
#include <vector>

#define DSTAT_ENABLED 0x01   // status bitwise 0b00000001
#define DSTAT_CHANGED 0x02   // status bitwise 0b00000010
#define DSTAT_READWRITE 0x04 // status bitwise 0b00000100

class Device {
  public:
    Device(const uint8_t& status) : status(status) {}
    virtual ~Device() = default;
    virtual bool store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes) = 0;
    virtual bool load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u = false) = 0;
    virtual bool okRead(const uint32_t& address, const uint8_t& size) const = 0;
    virtual void fill(const uint8_t& val) = 0;
    inline virtual std::vector<uint8_t>& getRaw() { return mem; }

  protected:
    std::vector<uint8_t> mem;
    uint8_t status = 0;
};
