#pragma once
#include <cstdint>
#include <vector>

#define DEV_OPENED 0x01  // status bitwise 0b00000001
#define DEV_CHANGED 0x02 // status bitwise 0b00000010
#define DEV_RW 0x04      // status bitwise 0b00000100

class Device {
  public:
    Device(const uint32_t& start, const uint32_t& size, const uint8_t& status);
    virtual ~Device();

    inline void open() { this->status |= DEV_OPENED; }
    inline void close() { this->status &= (~DEV_OPENED); }
    inline const bool isOpen() const { return this->status & DEV_OPENED; }
    inline const uint8_t& getFlags() const { return status; }
    inline std::vector<uint8_t>& getRaw() { return mem; }

    bool store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes);
    bool load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u = false);

    inline bool isValid(const uint32_t& address, const uint8_t& size) const {
        return (status & DEV_OPENED) && (address >= start) && (address < top + size);
    }

  private:
    inline bool okWrite(const uint32_t& address, const uint8_t& size) const {
        return ((status & (DEV_RW | DEV_OPENED)) == 0x5) && (address >= start) && ((address + size) < top);
    }

    uint8_t status;
    uint32_t start, top;
    std::vector<uint8_t> mem;
};
