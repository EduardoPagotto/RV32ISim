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
    Bus() {}
    virtual ~Bus() {}
    void add(Memory* buffer) { banks.push_back(buffer); }

    const bool store(const uint32_t& address, const MemoryAccessWidth& width, const uint32_t& reg) {

        const uint32_t size = static_cast<uint32_t>(width);
        for (auto& v : banks) {
            if (v->validRange(address, size))
                return v->write(address, size, reg);
        }

        return false;
    }

    const std::optional<uint32_t> load(const uint32_t& address, const MemoryAccessWidth& width, const bool& u) {

        for (auto& v : banks) {

            const uint32_t size = static_cast<uint32_t>(width);

            if (v->validRange(address, size)) {

                auto oValue = v->read(address, size);
                if (oValue.has_value()) {
                    return oValue;
                }
            }
        }

        return {};
    }
};

// class CSRBus {
//   public:
//     CSRBus() {}
//     virtual ~CSRBus() {}

//     uint32_t add(Device* buffer) {
//         banks.push_back(buffer);
//         return banks.size() - 1;
//     }

//     bool store(const uint32_t& reg, const uint32_t& address) {

//         for (auto& v : banks) {

//             const uint32_t size = 4; // static_cast<uint32_t>(width);
//             const uint32_t addr = address << 2;

//             if (v->validRange(addr, size)) {

//                 uint8_t array[] = {0, 0, 0, 0};
//                 for (uint8_t i = 0; i < size; i++)
//                     array[i] = (uint8_t)(reg >> (8 * i)) & 0xff;

//                 v->write(addr, size, array);
//                 return true;
//             }
//         }

//         return false;
//     }

//     bool load(uint32_t& retVal, const uint32_t& address) {

//         for (auto& v : banks) {

//             const uint32_t size = 4; // static_cast<uint32_t>(width);
//             const uint32_t addr = address << 2;

//             if (v->validRange(addr, size)) {

//                 retVal = 0;
//                 uint8_t array[] = {0, 0, 0, 0};
//                 v->read(addr, size, array);

//                 for (uint8_t i = 0; i < size; i++)
//                     retVal = retVal | ((uint32_t)array[i]) << (8 * i);

//                 return true;
//             }
//         }

//         return false;
//     }

//   private:
//     std::vector<Device*> banks;
// };