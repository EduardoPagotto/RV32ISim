#pragma once
#include <cstdint>
#include <optional>
#include <vector>

// Device access
#define DEV_OPENED 0x01  // status bitwise 0b00000001
#define DEV_CHANGED 0x02 // status bitwise 0b00000010
#define DEV_RW 0x04      // status bitwise 0b00000100

// PRIVILEGES
#define PRIV_USER0 0x00
#define PRIV_SUPER 0x04
#define PRIV_HYPER 0x80
#define PRIV_MACHI 0xC0

class Memory {
  private:
    uint8_t status;
    uint32_t start, top;
    std::vector<uint8_t> buffer;

  public:
    Memory(const uint32_t& start, const uint32_t& size, const uint8_t& status, const uint8_t& initval = 0xFC)
        : status(status), start(start), top(start + size) {
        this->buffer.reserve(size);
        for (uint32_t i = 0; i < size; i++)
            this->buffer.push_back(initval);
    }

    virtual ~Memory() { this->buffer.clear(); }

    inline void setRW(const bool& enable) {
        if (enable)
            status |= DEV_RW;
        else
            status &= (~DEV_RW);
    }

    // inline void open() { this->status |= DEV_OPENED; }

    // inline void close() { this->status &= (~DEV_OPENED); }

    inline const bool isOpen() const { return this->status & DEV_OPENED; }

    inline const bool isWritetable() const { return ((status & (DEV_RW | DEV_OPENED)) == 0x5); }

    // inline const bool isRW() const { return this->status & DEV_RW; }

    // inline const uint8_t& getStatus() const { return status; }

    // inline const uint32_t& getStart() const { return start; };

    // inline const uint32_t& getTop() const { return top; };

    inline std::vector<uint8_t>* getBank() { return &this->buffer; }

    inline const bool validRange(const uint32_t& address, const uint32_t& size) const {
        return isOpen() && (address >= start) && (address < top + size);
    }

    inline const bool validWrite(const uint32_t& address, const uint32_t& size) const {
        return (isWritetable() && (address >= start) && ((address + size) <= top));
    }

    const std::optional<uint32_t> read(const uint32_t& address, const uint32_t& size, const bool& signedVal = false) {

        if (validRange(address, size)) {
            uint32_t addrFinal{address - start};

            status &= ~DEV_CHANGED;
            uint32_t value{0};

            for (uint32_t i = 0; i < size; i++)
                value |= this->buffer[addrFinal + i] << (8 * i);

            if (signedVal) {
                const uint32_t lastval = addrFinal + size - 1;
                if ((this->buffer[lastval] >> 7) == 1) {
                    for (int i = 4; i > size; i--)
                        value = value | 0xff << (8 * (i - 1)); // fill FF MSB if negative
                }
            }

            return value;
        }
        return {};
    };

    const bool write(const uint32_t& address, const uint32_t& size, const uint32_t& val) {

        if (validWrite(address, size)) {
            uint32_t addrFinal{address - start};

            status |= DEV_CHANGED; // set changed

            for (uint32_t i = 0; i < size; i++)
                this->buffer[addrFinal + i] = static_cast<uint8_t>(val >> (8 * i)) & 0xff;

            return true;
        }
        return false;
    };
};