#include "../include/WriteBack.hpp"

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

WriteBack::WriteBack(Controller* c, Bus* b, MemoryAccess* m, uint32_t* regs)
    : crt(c), bus(b), memory(m), regs(regs), state(PipelineState::WriteBack) {}

void WriteBack::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        const MemoryAccessData& m = memory->get();
        if (m.isValid) {
            if (m.rd != 0) {
                regs[m.rd] = m.value;
                std::cout << crt->alias[m.rd] << " = " << int_to_hex(m.value);
            } else {
                regs[0] = 0;
            }
        }

        std::cout << '\n';
    }
}