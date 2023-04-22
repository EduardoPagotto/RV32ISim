#include "../include/MemoryAccess.hpp"

void MemoryAccess::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        const ExecuteData& d = execute->get();

        data.isValid = false;
        data.value = 0;

        switch (d.opcode) {
            case OpCodeSet::LOAD:
                data.isValid = true;
                data.rd = d.index;
                bus->load(data.value, d.address, d.memSize, d.valSigned);

                csr->prt.printAddress(d.address); // TODO: Melhorar o print

                break;

            case OpCodeSet::SAVE:
                bus->store(d.valueRS, d.address, d.memSize);

                csr->prt.printRegtoMemory(d.index, d.valueRS, d.address); // TODO: Melhorar o print

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
                data.isValid = true;
                switch (d.opcodeSys) {
                    case OpCodeSetSystem::EBREAK:
                        // FIXME: mover para execute ???
                        // mepc = fetch->getPcPlus4();
                        // mcause = static_cast<uint32_t>(MCause::Breakpoint);
                        // mtval = 0;
                        // trap = 1;
                        break;
                    case OpCodeSetSystem::ECALL:
                        std::cout << "Ecall - Exiting program" << '\n';
                        csr->ecall = true;
                        // mepc = fetch->getPcPlus4();
                        // mcause = static_cast<uint32_t>(MCause::EnvironmentCallFromMMode);
                        // mtval = 0;
                        // trap = 1;
                        break;

                    case OpCodeSetSystem::INVALID:
                        break;

                    case OpCodeSetSystem::CSRRC:
                        data.value = this->csr->read(d.address);
                        if (d.index != 0)
                            this->csr->write(d.address, (data.value & (~d.valueRS)));
                        break;

                    case OpCodeSetSystem::CSRRCI:
                        data.value = this->csr->read(d.address);
                        if (d.index != 0)
                            this->csr->write(d.address, (data.value & (~d.index)));
                        break;

                    case OpCodeSetSystem::CSRRS:
                        data.value = this->csr->read(d.address);
                        if (d.index != 0)
                            this->csr->write(d.address, data.value | d.valueRS);
                        break;

                    case OpCodeSetSystem::CSRRSI:
                        data.value = this->csr->read(d.address);
                        if (d.index != 0)
                            this->csr->write(d.address, data.value | d.index);
                        break;

                    case OpCodeSetSystem::CSRRW:
                        data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                        this->csr->write(d.address, d.valueRS);
                        break;

                    case OpCodeSetSystem::CSRRWI:
                        data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                        this->csr->write(d.address, d.index);
                        break;
                }

                break;

            default:
                throw std::string("Opcode desconhecido");
                break;
        }
    }
}