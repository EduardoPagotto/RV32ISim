#pragma once
#include <cstdint>
#include <optional>
#include <vector>

class Device {
  private:
    uint32_t top;
    std::vector<uint8_t> bank;

  public:
    Device(const uint32_t& size, const uint8_t& initval = 0xFC) : top(size) {
        this->bank.reserve(size);
        for (uint32_t i = 0; i < size; i++)
            this->bank.push_back(initval);
    }

    Device() = delete;
    Device(const Device& o) = delete;

    virtual ~Device() { this->bank.clear(); }

    inline const bool validRange(const uint32_t& address, const uint32_t& size) const {
        return ((address + size) < top);
    }

    const std::optional<uint32_t> read(const uint32_t& address, const uint32_t& size, const bool& signedVal = false) {

        if (validRange(address, size)) {

            uint32_t value{0};

            for (uint32_t i = 0; i < size; i++)
                value |= this->bank[address + i] << (8 * i);

            if (signedVal) {
                const uint32_t lastval = address + size - 1;
                if ((this->bank[lastval] >> 7) == 1) {
                    for (int i = 4; i > size; i--)
                        value = value | 0xff << (8 * (i - 1)); // fill FF MSB if negative
                }
            }

            return value;
        }
        return {};
    };

    const bool write(const uint32_t& address, const uint32_t& size, const uint32_t& val) {

        if (validRange(address, size)) {
            for (uint32_t i = 0; i < size; i++)
                this->bank[address + i] = static_cast<uint8_t>(val >> (8 * i)) & 0xff;

            return true;
        }
        return false;
    };
};