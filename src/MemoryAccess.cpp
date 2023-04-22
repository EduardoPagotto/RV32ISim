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
        data.value = 0;

        switch (d.opcode) {
            case OpCodeSet::LOAD:
                data.isValid = true;
                data.rd = d.index;
                bus->load(data.value, d.address, d.memSize, d.valSigned);

                std::cout << "(0x" << int_to_hex(d.address) << ") -> ";

                break;

            case OpCodeSet::SAVE:
                bus->store(d.valueRS, d.address, d.memSize);

                // std::cout << crt->alias[d.index] << " = " << int_to_hex(d.valueRS) << " -> (0x" <<
                // int_to_hex(d.address)
                //           << ")";

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
                        crt->ecall = true;
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