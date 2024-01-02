#pragma once
#include "Memory.hpp"
#include <map>
#include <string>

enum class MemoryAccessWidth : __int8_t {
    Byte = 0x01,     // 1 byte 8 bits
    HalfWord = 0x02, // 2 bytes 16 bits (little endian)
    Word = 0x04,     // 4 bytes 32 bits (little endian)
    DWord = 0x08,    // 8 bytes 64 bits (little endian)
};

class Bus {
  private:
    std::vector<Memory*> banks;

  public:
    Bus() = default;
    virtual ~Bus() = default;

    void add(Memory* buffer) { banks.push_back(buffer); }

    const bool store(const uint32_t& address, const MemoryAccessWidth& width, const uint32_t& reg) {

        const uint32_t size = static_cast<uint32_t>(width);
        for (auto& v : banks) {
            if (v->validRange(address, size))
                return v->write(address, size, reg);
        }

        return false;
    }

    const std::optional<uint32_t> load(const uint32_t& address, const MemoryAccessWidth& width,
                                       const bool& signedVal = false) {

        const uint32_t size = static_cast<uint32_t>(width);
        for (auto& v : banks) {
            if (v->validRange(address, size)) {
                auto oValue = v->read(address, size, signedVal);
                if (oValue.has_value()) {
                    return oValue;
                }
            }
        }

        return {};
    }
};
