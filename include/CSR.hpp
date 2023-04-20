#pragma once
#include "RLatch.hpp"

enum class CSRInstructionType {
    RW = 0b01,
    RS = 0b10,
    RC = 0b11,

    CSRRW = 0b001,
    CSRRS = 0b010,
    CSRRC = 0b011,
    CSRRWI = 0b101,
    CSRRSI = 0b110,
    CSRRCI = 0b111,
};

enum class CSRMMode {
    misa = 0x301,
    mvendorid = 0xF11,
    marchid = 0xF12,
    mimpid = 0xF13,
    mhartid = 0xF14,
    mstatus = 0x300,
    mtvec = 0x305,
    mie = 0x304,
    mip = 0x344,
    mcause = 0x342,
    mepc = 0x341,
    mscratch = 0x340,
    mtval = 0x343,
};

// const MSTATUS_MASK = (1 << 3) | (1 << 7);

class CSR {
  public:
    CSR() = default;
    virtual ~CSR() = default;

    void step() { cycles++; }

    uint32_t read(uint32_t address) {
        switch (address) {
            // User Level
            case 0xc00:
                return (cycles & 0xffffffff);
            case 0xc01:
                return (cycles & 0xffffffff);
            case 0xc02:
                return (instret & 0xffffffff);
            case 0xc80:
                return (cycles >> 32);
            case 0xc81:
                return (cycles >> 32);
            case 0xc82:
                return (instret >> 32);

            // Machine Mode
            case 0x301:
                return misa;
            case 0xf11:
                return mvendorid;
            case 0xf12:
                return marchid;
            case 0xf13:
                return mimpid;
            case 0xf14:
                return mhartid;
            case 0x300:
                return mstatus;
            case 0x305:
                return mtvec;
            case 0x304:
                return mie;
            case 0x344:
                return mip;
            case 0x342:
                return mcause;
            case 0x341:
                return mepc;
            case 0x340:
                return mscratch;
            case 0x343:
                return mtval;
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
                mstatus = value & (1 << 3) | (1 << 7);
                return;
            }
            case CSRMMode::mie: {
                mie = value;
                return;
            }
            case CSRMMode::mip: {
                mip = value;
                return;
            }
            case CSRMMode::mcause: {
                mcause = value;
                return;
            }
            case CSRMMode::mepc: {
                mepc = value;
                return;
            }
            case CSRMMode::mscratch: {
                mscratch = value;
                return;
            }
            case CSRMMode::mtval: {
                mtval = value;
                return;
            }
            default:
                // TODO: implementar
                return;
        }
    }

  public: // FIXME em Trap
    uint64_t cycles = 0;
    uint64_t instret = 0;
    uint64_t mtimecmp = 0;
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
