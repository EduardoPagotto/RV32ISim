#include "include/MemoryAccess.hpp"

void MemoryAccess::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        const ExecuteData& d = execute->get();

        data = d;
        data.isValid = false;
        data.value = 0;

        const bool isUnaligned = ((d.width == MemoryAccessWidth::Word && d.address & 0b11) ||
                                  (d.width == MemoryAccessWidth::HalfWord && d.address & 0b01));

        switch (d.decode.opcode) {
            case OpCode::LOAD:

                if (isUnaligned) {
                    csr->trapException(Trap(d.address, MCause::LoadAddressMisaligned, d.decode.fetch.instr));
                    return;
                }

                data.isValid = true;
                bus->load(data.value, d.address, d.width, d.valSigned);
                csr->prt.printAddress(d.address); // TODO: Melhorar o print
                break;

            case OpCode::SAVE:

                if (isUnaligned) {
                    csr->trapException(Trap(d.address, MCause::StoreAMOAddressMisaligned, d.decode.fetch.instr));
                    return;
                }

                bus->store(d.valueRS2, d.address, d.width);
                csr->prt.printRegtoMemory(d.decode.rs2, d.valueRS2, d.address); // TODO: Melhorar o print
                break;

            case OpCode::BRANCH:
                // branchCase();
                break;

            case OpCode::ULA:
            case OpCode::ULAI:
            case OpCode::AUIPC:
            case OpCode::LUI:
            case OpCode::JALR:
            case OpCode::JAL:
                data.isValid = true;
                data.value = d.address;
                break;

            case OpCode::FENCE:
                // TODO: implementar
                break;

            case OpCode::SYSTEM:
                data.isValid = true;
                switch (d.decode.opcodeSys) {
                    case OpCodeSystem::INVALID:
                        break;

                    case OpCodeSystem::CSRRC:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, (data.value & (~d.valueRS1)));
                        break;

                    case OpCodeSystem::CSRRCI:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, (data.value & (~d.decode.rs1)));
                        break;

                    case OpCodeSystem::CSRRS:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, data.value | d.valueRS1);
                        break;

                    case OpCodeSystem::CSRRSI:
                        data.value = this->csr->read(d.address);
                        if (d.decode.rs1 != 0)
                            this->csr->write(d.address, data.value | d.decode.rs1);
                        break;

                    case OpCodeSystem::CSRRW:
                        data.value = (d.valueRD != 0) ? this->csr->read(d.address) : 0;
                        this->csr->write(d.address, d.valueRS1);
                        break;

                    case OpCodeSystem::CSRRWI:
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