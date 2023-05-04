#pragma once
#include "Device.hpp"
#include <map>
#include <string>

enum class MemoryAccessWidth : __int8_t {
    Byte = 0x01,     // 1 byte 8 bits
    HalfWord = 0x02, // 2 bytes 16 bits (little endian)
    Word = 0x04,     // 4 bytes 32 bits (little endian)
    DWord = 0x08,    // 8 bytes 64 bits (little endian)
};

class Bus {
  public:
    Bus() {}
    virtual ~Bus() {}
    uint32_t add(Device* dev);
    Device* get(const uint32_t& index) { return vDevs[index]; }

    bool store(const uint32_t& reg, const uint32_t& address, const MemoryAccessWidth& width);
    bool load(uint32_t& retVal, const uint32_t& address, const MemoryAccessWidth& width, const bool& u = false);
    bool hasData(const uint32_t& address) const;
    void loadFile(const std::string& file, const uint32_t& index);

  private:
    std::vector<Device*> vDevs;
};
