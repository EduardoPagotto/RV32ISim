#include "include/MemoryAccess.hpp"

void MemoryAccess::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        const ExecuteData& d = execute->get();

        data = d;
        data.isValid = false;
        data.value = 0;

        switch (d.decode.opcode) {
            case OpCodeSet::LOAD:
                data.isValid = true;
                bus->load(data.value, d.address, d.width, d.valSigned);
                csr->prt.printAddress(d.address); // TODO: Melhorar o print
                break;

            case OpCodeSet::SAVE:
                bus->store(d.valueRS2, d.address, d.width);
                csr->prt.printRegtoMemory(d.decode.rs2, d.valueRS2, d.address); // TODO: Melhorar o print
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
                data.value = d.address;
                break;

            case OpCodeSet::FENCE:
                // TODO: implementar
                break;

            case OpCodeSet::SYSTEM:
                data.isValid = true;
                switch (d.decode.opcodeSys) {
                    case OpCodeSetSystem::INVALID:
                        break;

                    case OpCodeSetSystem::CSRRC:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, (data.value & (~d.valueRS1)));
                        break;

                    case OpCodeSetSystem::CSRRCI:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, (data.value & (~d.decode.rs1)));
                        break;

                    case OpCodeSetSystem::CSRRS:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, data.value | d.valueRS1);
                        break;

                    case OpCodeSetSystem::CSRRSI:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, data.value | d.decode.rs1);
                        break;

                    case OpCodeSetSystem::CSRRW:
                        data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                        this->csr->write(d.address, d.valueRS1);
                        break;

                    case OpCodeSetSystem::CSRRWI:
                        data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                        this->csr->write(d.address, d.decode.rs1);
                        break;
                    default:
                        break;
                }
                break;

            default:
                throw std::string("Opcode desconhecido");
                break;
        }
    }
}