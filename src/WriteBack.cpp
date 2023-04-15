#include "../include/WriteBack.hpp"

WriteBack::WriteBack(Controller* c, Bus* b, MemoryAccess* m, uint32_t* regs)
    : crt(c), bus(b), memory(m), regs(regs), state(PipelineState::WriteBack) {}

void WriteBack::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        const MemoryAccessData& m = memory->get();
        if (m.isValid) {
            regs[m.rd] = m.value;
        }
    }
}