#include "../include/MemoryAccess.hpp"

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
                data.rd = d.indexRD;
                bus->load(data.value, d.address, d.memSize, d.valSigned);
                break;

            case OpCodeSet::SAVE:
                bus->store(d.valueRS2, d.address, d.memSize);
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
                data.rd = d.indexRD;
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