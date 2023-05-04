#include <fstream>
#include <include/Bus.hpp>

uint32_t Bus::add(Device* dev) {
    size_t index = vDevs.size();
    vDevs.push_back(dev);
    return index;
}

bool Bus::hasData(const uint32_t& address) const {
    for (auto& v : vDevs) {
        if (v->validRange(address, 0))
            return true;
    }
    return false;
}

bool Bus::store(const uint32_t& reg, const uint32_t& address, const MemoryAccessWidth& width) {

    for (auto& v : vDevs) {

        const uint32_t size = static_cast<uint32_t>(width);

        if (v->validRange(address, size)) {

            uint8_t array[] = {0, 0, 0, 0};
            for (uint8_t i = 0; i < size; i++)
                array[i] = (uint8_t)(reg >> (8 * i)) & 0xff;

            v->write(address, size, array);
            return true;
        }
    }

    return false;
}

bool Bus::load(uint32_t& retVal, const uint32_t& address, const MemoryAccessWidth& width, const bool& u) {

    for (auto& v : vDevs) {

        const uint32_t size = static_cast<uint32_t>(width);

        if (v->validRange(address, size)) {

            retVal = 0;
            uint8_t array[] = {0, 0, 0, 0};
            v->read(address, size, array);

            for (uint8_t i = 0; i < size; i++)
                retVal = retVal | ((uint32_t)array[i]) << (8 * i);

            if (u != true) {
                if ((array[size - 1] >> 7) == 1) { // For sign extension
                    for (int i = 4; i > size; i--) {
                        retVal = retVal | 0xff << (8 * (i - 1));
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void Bus::loadFile(const std::string& file, const uint32_t& index) {

    std::ifstream instream(file, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());

    if (index < vDevs.size()) {
        Device* dev = vDevs[index];
        dev->write(dev->getStart(), data.size(), &data[0]);
    }
}