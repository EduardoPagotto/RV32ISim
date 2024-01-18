#pragma once
#include "Memory.hpp"
#include "mmu/MMU.hpp"
#include <iostream>
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
    MMU mmu;

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

    void getVal(const std::tuple<int32_t, uint32_t> v) {
        std::cout << "Code: " << std::get<0>(v) << " Val: " << std::get<1>(v) << std::endl;
    }

    const std::optional<uint32_t> load(const uint32_t& address, const MemoryAccessWidth& width,
                                       const bool& signedVal = false) {

        getVal(mmu.createEntry(0x00000, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
        getVal(mmu.createEntry(0x40000, 0, MMU_ACC_READ | MMU_ACC_WRITE));
        getVal(mmu.createEntry(0x80000, 0, MMU_ACC_READ | MMU_ACC_WRITE | MMU_ACC_SUPER));
        getVal(mmu.createEntry(0xC0000, 0, MMU_ACC_EXECUTE));
        getVal(mmu.createEntry(0xF0000, 0, MMU_ACC_EXECUTE | MMU_ACC_READ | MMU_ACC_WRITE | MMU_ACC_SUPER));

        // ---
        getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_EXECUTE));
        getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));

        getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_READ));
        getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_WRITE));
        getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_EXECUTE));
        getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));

        getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_READ));
        getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_WRITE));
        getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));

        getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_EXECUTE));
        getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_READ));
        getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_WRITE));

        getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
        getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_EXECUTE));
        getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_READ));
        getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_WRITE));

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
