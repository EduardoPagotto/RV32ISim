#include <include/Memory.hpp>

Memory::Memory(const uint32_t& start, const uint32_t& size, const uint8_t& status) : Device(status), start(start) {
    this->top = start + size;
    this->mem.reserve(size + 10);
    for (uint16_t i = 0; i < size; i++)
        this->mem.push_back(0xFC);
}

Memory::~Memory() { this->mem.clear(); }

bool Memory::store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes) {

    if (okWrite(address, bytes)) {

        uint32_t addrFinal = address - start;

        for (uint8_t i = 0; i < bytes; i++)
            this->mem[addrFinal + i] = (uint8_t)(reg >> (8 * i)) & 0xff;

        return true;
    }
    return false;
}

bool Memory::load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u) {

    if (okRead(address, bytes)) {

        retVal = 0;

        status &= ~DSTAT_CHANGED;
        uint32_t addrFinal = address - start;

        for (uint8_t i = 0; i < bytes; i++) {
            retVal = retVal | ((uint32_t)this->mem[addrFinal + i]) << (8 * i);
        }

        if (u != true) {
            if ((mem[addrFinal + bytes - 1] >> 7) == 1) { // For sign extension
                for (int i = 4; i > bytes; i--) {
                    retVal = retVal | 0xff << (8 * (i - 1));
                }
            }
        }
        return true;
    }

    return false;
}
