#pragma once
#include "Device.hpp"

class Memory : public Device {
  public:
    Memory(const uint32_t& start, const uint32_t& size, const uint8_t& status)
        : Device(status), start(start), top(start + size) {
        this->mem.reserve(size);
        for (uint32_t i = 0; i < size; i++)
            this->mem.push_back(0xFC);
    }

    virtual ~Memory() { this->mem.clear(); }

    virtual bool read(const uint32_t& address, const uint32_t& size, uint8_t* valueRet) override {

        if (validRange(address, size)) {
            uint32_t addrFinal = address - start;

            status &= ~DEV_CHANGED;

            for (uint32_t i = 0; i < size; i++)
                valueRet[i] = this->mem[addrFinal + i];

            return true;
        }
        return false;
    };

    virtual bool write(const uint32_t& address, const uint32_t& size, uint8_t* value) override {

        if (validWrite(address, size)) {
            uint32_t addrFinal = address - start;

            status |= DEV_CHANGED; // set changed

            for (uint32_t i = 0; i < size; i++)
                this->mem[addrFinal + i] = value[i];

            return true;
        }
        return false;
    };

    inline virtual bool validRange(const uint32_t& address, const uint32_t& size) const override {
        return isOpen() && (address >= start) && (address < top + size);
    }

    inline virtual bool validWrite(const uint32_t& address, const uint32_t& size) const override {
        return isWritetable() && (address >= start) && ((address + size) < top);
    }

    inline virtual const uint32_t getStart() const override { return start; };
    inline virtual const uint32_t getTop() const override { return top; };

  protected:
    uint32_t start, top;
    std::vector<uint8_t> mem;
};