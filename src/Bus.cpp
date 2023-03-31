#include <fstream>
#include <include/Bus.hpp>

uint32_t Bus::add(Device* dev) {
    size_t index = vDevs.size();
    vDevs.push_back(dev);
    return index;
}

bool Bus::hasData(const uint32_t& address) const {
    for (auto& v : vDevs) {
        if (v->isValid(address, 0))
            return true;
    }
    return false;
}

bool Bus::store(const uint32_t& reg, const uint32_t& address, const uint8_t& bytes) {

    for (auto& v : vDevs) {
        if (v->store(reg, address, bytes))
            return true;
    }

    return false;
}

bool Bus::load(uint32_t& retVal, const uint32_t& address, const uint8_t& bytes, const bool& u) {

    for (auto& v : vDevs) {
        if (v->load(retVal, address, bytes, u))
            return true;
    }

    return false;
}

void Bus::loadFile(const std::string& file, const uint32_t& index) {

    std::ifstream instream(file, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());

    Device* dev = vDevs[index];
    for (int i = 0; i < data.size(); i++)
        dev->getRaw()[i] = data[i];
}