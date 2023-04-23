#include "include/CSR.hpp"

uint32_t CSR::read(uint32_t address) {
    switch (address) {
        // User Level
        case 0xc00:
            return (mem.cycles & 0xffffffff);
        case 0xc01:
            return (mem.cycles & 0xffffffff);
        case 0xc02:
            return (mem.instret & 0xffffffff);
        case 0xc80:
            return (mem.cycles >> 32);
        case 0xc81:
            return (mem.cycles >> 32);
        case 0xc82:
            return (mem.instret >> 32);

        // Machine Mode
        case 0x301:
            return mem.misa;
        case 0xf11:
            return mem.mvendorid;
        case 0xf12:
            return mem.marchid;
        case 0xf13:
            return mem.mimpid;
        case 0xf14:
            return mem.mhartid;
        case 0x300:
            return mem.mstatus;
        case 0x305:
            return mem.mtvec;
        case 0x304:
            return mem.mie;
        case 0x344:
            return mem.mip;
        case 0x342:
            return mem.mcause;
        case 0x341:
            return mem.mepc;
        case 0x340:
            return mem.mscratch;
        case 0x343:
            return mem.mtval;
    }

    // FIXME:
    return 0;
    //  throw new Error(`Unknown CSR : 0x $ { toHexString(address, 3) }`);
}

void CSR::write(uint32_t address, uint32_t value) {
    const uint32_t isReadOnly = address >> 10;
    const uint32_t permission = (address >> 8) & 0b11;

    if (isReadOnly != 0) {
        // FIXME:
        // throw new Error('CSR Write: Attempt to write a read-only register');
    }

    switch (static_cast<CSRMMode>(address)) { // FIXME:
        case CSRMMode::mstatus: {
            mem.mstatus = value & (1 << 3) | (1 << 7);
            return;
        }
        case CSRMMode::mie: {
            mem.mie = value;
            return;
        }
        case CSRMMode::mip: {
            mem.mip = value;
            return;
        }
        case CSRMMode::mcause: {
            mem.mcause = value;
            return;
        }
        case CSRMMode::mepc: {
            mem.mepc = value;
            return;
        }
        case CSRMMode::mscratch: {
            mem.mscratch = value;
            return;
        }
        case CSRMMode::mtval: {
            mem.mtval = value;
            return;
        }
        default:
            // TODO: implementar
            return;
    }
}