#include "include/WriteBack.hpp"

void WriteBack::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        const MemoryAccessData& m = memory->get();
        if (m.isValid) {
            if (m.rd != 0) {
                regs[m.rd] = m.value;
                csr->prt.printRegVal(m.rd, m.value); // TODO: Melhorar o print
            } else {
                regs[0] = 0;
            }
        }

        std::cout << '\n';
    }
}