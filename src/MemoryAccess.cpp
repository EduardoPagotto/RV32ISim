#include "../include/MemoryAccess.hpp"

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

MemoryAccess::MemoryAccess(Controller* c, Bus* b, Execute* e, CSR* csr)
    : crt(c), bus(b), execute(e), csr(csr), state(PipelineState::MemoryAccess) {}

void MemoryAccess::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        const ExecuteData& d = execute->get();

        data.isValid = false;

        switch (d.opcode) {
            case OpCodeSet::LOAD:
                data.isValid = true;
                data.rd = d.index;
                bus->load(data.value, d.address, d.memSize, d.valSigned);

                std::cout << "(0x" << int_to_hex(d.address) << ") -> ";

                break;

            case OpCodeSet::SAVE:
                bus->store(d.valueRS2, d.address, d.memSize);

                std::cout << crt->alias[d.index] << " = " << int_to_hex(d.valueRS2) << " -> (0x"
                          << int_to_hex(d.address) << ")";

                break;

            case OpCodeSet::BRANCH:
                // branchCase();
                break;

            case OpCodeSet::ULA:
            case OpCodeSet::ULAI:
            case OpCodeSet::AUIPC:
            case OpCodeSet::LUI:
            case OpCodeSet::JALR:
            case OpCodeSet::JAL:
                data.isValid = true;
                data.rd = d.index;
                data.value = d.address;
                break;

            case OpCodeSet::FENCE:
                // TODO: implementar
                break;

            case OpCodeSet::SYSTEM:
                // setSystem();
                break;

            default:
                throw std::string("Opcode desconhecido");
                break;
        }
    }
}