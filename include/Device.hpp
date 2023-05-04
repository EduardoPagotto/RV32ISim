#pragma once
#include <cstdint>
#include <vector>

#define DEV_OPENED 0x01  // status bitwise 0b00000001
#define DEV_CHANGED 0x02 // status bitwise 0b00000010
#define DEV_RW 0x04      // status bitwise 0b00000100

class Device {
  public:
    Device(const uint8_t& status) : status(status) {}
    virtual ~Device() = default;

    virtual bool read(const uint32_t& address, const uint32_t& size, uint8_t* valueRet) = 0;
    virtual bool write(const uint32_t& address, const uint32_t& size, uint8_t* value) = 0;
    virtual bool validRange(const uint32_t& address, const uint32_t& size) const = 0;
    virtual bool validWrite(const uint32_t& address, const uint32_t& size) const = 0;
    virtual const uint32_t getStart() const = 0;
    virtual const uint32_t getTop() const = 0;

    inline void open() { this->status |= DEV_OPENED; }
    inline void close() { this->status &= (~DEV_OPENED); }

    inline void setRW(const bool& enable) {
        if (enable)
            status |= DEV_RW;
        else
            status &= (~DEV_RW);
    }
    inline const bool isRW() const { return this->status & DEV_RW; }

    inline const bool isOpen() const { return this->status & DEV_OPENED; }
    inline const uint8_t& getStatus() const { return status; }
    inline const bool isWritetable() const { return ((status & (DEV_RW | DEV_OPENED)) == 0x5); }

  protected:
    uint8_t status;
};