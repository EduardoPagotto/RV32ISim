#pragma once
#include "Device.hpp"
#include <map>
#include <string>

class Bus {
  public:
    Bus() {}
    virtual ~Bus() {}
    uint32_t add(Device* dev);

    bool store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes);
    bool load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u = false);
    void loadFile(const std::string& file, const uint32_t& index);

  private:
    std::vector<Device*> vDevs;
};
