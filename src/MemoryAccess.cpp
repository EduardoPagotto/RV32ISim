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
            case OpCodeSet::ULAI:

                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;

                // ulai();
                break;
            case OpCodeSet::AUIPC:
                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;
                // auipc();
                break;
            case OpCodeSet::SAVE:
                // saveRegister();
                bus->store(d.valueRS2, d.address, d.memSize);
                break;
            case OpCodeSet::ULA:
                // ula();
                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;

                break;
            case OpCodeSet::LUI:
                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;

                // lui();
                break;
            case OpCodeSet::BRANCH:
                // branchCase();
                break;
            case OpCodeSet::JALR:
                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;
                // jalr();
                break;
            case OpCodeSet::JAL:
                data.isValid = true;
                data.rd = d.indexRD;
                data.value = d.address;
                // jal();
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
        // regs[0] = 0; // jalr x0 xX is not supposed to cast exception, so this is
        //              // the easier way

        // returnFromTrap = false; // FIXME:
    }
}