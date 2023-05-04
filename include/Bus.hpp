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

class CSRBus {
  public:
    CSRBus() {}
    virtual ~CSRBus() {}

    uint32_t add(Device* dev) {
        vDevs.push_back(dev);
        return vDevs.size() - 1;
    }

    bool store(const uint32_t& reg, const uint32_t& address) {

        for (auto& v : vDevs) {

            const uint32_t size = 4; // static_cast<uint32_t>(width);
            const uint32_t addr = address << 2;

            if (v->validRange(addr, size)) {

                uint8_t array[] = {0, 0, 0, 0};
                for (uint8_t i = 0; i < size; i++)
                    array[i] = (uint8_t)(reg >> (8 * i)) & 0xff;

                v->write(addr, size, array);
                return true;
            }
        }

        return false;
    }

    bool load(uint32_t& retVal, const uint32_t& address) {

        for (auto& v : vDevs) {

            const uint32_t size = 4; // static_cast<uint32_t>(width);
            const uint32_t addr = address << 2;

            if (v->validRange(addr, size)) {

                retVal = 0;
                uint8_t array[] = {0, 0, 0, 0};
                v->read(addr, size, array);

                for (uint8_t i = 0; i < size; i++)
                    retVal = retVal | ((uint32_t)array[i]) << (8 * i);

                return true;
            }
        }

        return false;
    }

  private:
    std::vector<Device*> vDevs;
};