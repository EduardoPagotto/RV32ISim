#pragma once
#include "defs.hpp"

// const MSTATUS_MASK = (1 << 3) | (1 << 7);

struct CsrMem {
    CsrMem() = default;
    ~CsrMem() = default;
    CsrMem(const CsrMem& o) = default;
    uint64_t cycles = 0;
    uint64_t instret = 0;
    // uint64_t mtimecmp = 0;
    uint64_t misa = 0x40000100; // Encodes CPU capabilities, top 2 bits encode width (XLEN), bottom 26 encode extensions
    uint64_t mvendorid = 0;     // JEDEC manufacturer ID
    uint64_t marchid = 0;       // Microarchitecture ID
    uint64_t mimpid = 0;        // Processor version
    uint64_t mhartid = 0;       // Hart ID
    uint64_t mstatus = 0;       // Various specific flags and settings, including global interrupt enable
    uint64_t mtvec = 0x10000004 | 1; // Encodes the base trap vector address + mode (table or single handler)
    uint64_t mie = 0x00000888;       // Interrupt enable / disable
    uint64_t mip = 0;                // Interrupt-pending
    uint64_t mcause = 0;   // Trap cause. Top bit set = interrupt, reset = exception - reset indicates the type
    uint64_t mepc = 0;     // Exception Program Counter
    uint64_t mscratch = 0; // General use reg for M-Mode, mostly used to hold a pointer context space apparently
    uint64_t mtval = 0;    // Trap-value register, can hold the address of a faulting instruction
};

class CSR {
  public:
    CSR() = default;
    virtual ~CSR() = default;

    void step() { mem.cycles++; }

    void commit() {}

    uint32_t read(uint32_t address) {
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

    void write(uint32_t address, uint32_t value) {
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

  public: // FIXME em Trap
    CsrMem mem;
};
