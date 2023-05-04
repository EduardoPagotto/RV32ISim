#include "include/CSR.hpp"
#include "include/Memory.hpp"

CSR::CSR() {
    bus.add(new Memory(0x300 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_MACHI), 0)); // 0x0300 << 2
    bus.add(new Memory(0xC00 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_USER0), 0)); // 0x0300 << 2
    bus.add(new Memory(0xF00 << 2, (256 * 4), (DEV_OPENED | DEV_RW | PRIV_MACHI), 0)); // 0x0F00 << 2

    bus.store(0x40000100, CSR_MISA);
    bus.store(0x10000004 | 1, CSR_MTVEC);
    bus.store(0x00000888, CSR_MIE);
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

void CSR::step() { increment64(CSR_CYCLE, CSR_CYCLEH); }