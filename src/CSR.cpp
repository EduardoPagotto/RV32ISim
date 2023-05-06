#include "include/CSR.hpp"
#include "include/Memory.hpp"

CSR::CSR() {
    bus.add(new Memory(0x300 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_MACHI), 0)); // 0x0300 << 2
    bus.add(new Memory(0xC00 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_USER0), 0)); // 0x0300 << 2
    bus.add(new Memory(0xF00 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_MACHI), 0)); // 0x0F00 << 2

    bus.store(0x40000100, CSR_MISA);
    bus.store(0x10000004 | 1, CSR_MTVEC);
    bus.store(0x00000888, CSR_MIE);
    bus.store(0x00000008, CSR_MSTATUS);
}

uint32_t CSR::read(uint32_t address) {

    uint32_t val = 0;

    if (!bus.load(val, address))
        throw std::string("Registro invalido");

    return val;
}

void CSR::write(uint32_t address, uint32_t value) {
    const uint32_t isReadOnly = address >> 10;
    const uint32_t permission = (address >> 8) & 0b11;

    if (isReadOnly != 0) {
        // FIXME:
        // throw new Error('CSR Write: Attempt to write a read-only register');
    }

    if (!bus.store(value, address))
        throw std::string("Registro invalido");

    // mem.mstatus = value & (1 << 3) | (1 << 7);
}

void CSR::nextState() {
    if (cpuState == CPUState::Pipeline) {
        switch (pipelineState) {
            case PipelineState::Fetch:
                pipelineState = PipelineState::Decode;
                break;
            case PipelineState::Decode:
                pipelineState = PipelineState::Execute;
                break;
            case PipelineState::Execute:
                pipelineState = PipelineState::MemoryAccess;
                break;
            case PipelineState::MemoryAccess:
                pipelineState = PipelineState::WriteBack;
                break;
            case PipelineState::WriteBack:
                pipelineState = PipelineState::Fetch;
                increment64(CSR_INSTRET, CSR_INSTRETH);
                break;
        }
    }
}

void CSR::increment64(const uint32_t add, const uint32_t add2) {

    uint32_t low = 0;
    bus.load(low, add);
    low++;
    bus.store(low, add);
    if (low == 0) {
        uint32_t hight = 0;
        bus.load(hight, add2);
        hight++;
        bus.store(hight, add2);
    }
}

void CSR::step() {

    switch (trap.trapState) {
        case TrapState::Idle: {
            return;
        }

        case TrapState::SetCSRJump: {

            this->write(CSR_MEPC, trap.mepc);
            this->write(CSR_MCAUSE, static_cast<uint32_t>(trap.mcause));
            this->write(CSR_MTVAL, trap.mtval);

            uint32_t mstatus = this->read(CSR_MSTATUS);
            const uint32_t mie = (mstatus & MSTATUS_MIE_MASK) >> MSTATUS_MIE_BIT;
            // Unset MPIE bit
            mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>
            // Save MIE to MPIE
            mstatus = (mstatus | (mie << MSTATUS_MPIE_BIT)) >> 0; // >>>
            // Unset mie
            mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>

            this->write(CSR_MSTATUS, mstatus);

            trap.setPC(this->read(CSR_MTVEC));

            return;
        }

        case TrapState::SetPc: {
            this->setBranchAddress(trap.pcToSet);
            this->cpuState = CPUState::Pipeline;
            this->pipelineState = PipelineState::WriteBack; // PipelineState::Fetch;
            this->trap.trapState = TrapState::Idle;
            return;
        }

        case TrapState::ReturnFromTrap: {

            // TODO
            trap.pcToSet = this->read(CSR_MEPC) + 4;
            trap.trapState = TrapState::SetPc;

            uint32_t mstatus = this->read(CSR_MSTATUS);
            const uint32_t mpie = (mstatus & MSTATUS_MPIE_MASK) >> MSTATUS_MPIE_BIT;
            // Unset MIE bit
            mstatus = (mstatus & ~MSTATUS_MIE_MASK) >> 0; // >>>
            // Save MPIE to MIE
            mstatus = (mstatus | (mpie << MSTATUS_MIE_BIT)) >> 0; // >>>
            // Unset mpie
            mstatus = (mstatus & ~MSTATUS_MPIE_MASK) >> 0; // >>>

            this->write(CSR_MSTATUS, mstatus);

            return;
        }
    }

    increment64(CSR_CYCLE, CSR_CYCLEH);
}