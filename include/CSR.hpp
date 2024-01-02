#pragma once
#include <map>
//   const MSTATUS_MASK = (1 << 3) | (1 << 7);

// // PRIVILEGES
// #define PRIV_USER0 0x00
// #define PRIV_SUPER 0x04
// #define PRIV_HYPER 0x80
// #define PRIV_MACHI 0xC0
// MACHINE TRAP
#define CSR_MSTATUS 0x300    // MRW
#define CSR_MISA 0x301       // MRW
#define CSR_MEDELEG 0x302    // MRW
#define CSR_MIDELEG 0x303    // MRW
#define CSR_MIE 0x304        // MRW
#define CSR_MTVEC 0x305      // MRW
#define CSR_MCOUNTEREN 0x306 // MRW
#define CSR_MSTATUSH 0x310   // MRW
// MACHINE TRAP HANDLING
#define CSR_MSCRATCH 0x340 // MRW
#define CSR_MEPC 0x341     // MRW
#define CSR_MCAUSE 0x342   // MRW
#define CSR_MTVAL 0x343    // MRW
#define CSR_MIP 0x344      // MRW
#define CSR_MTINST 0x34A   // MRW
#define CSR_MTVAL2 0x34B   // MRW
// Unprivileged Conter/Timers
#define CSR_CYCLE 0xc00        // URO
#define CSR_TIME 0xc01         // URO
#define CSR_INSTRET 0xc02      // URO
#define CSR_HPMCONTER3 0xc03   // URO
#define CSR_HPMCONTER4 0xc03   // URO
#define CSR_HPMCONTER31 0xc1f  // URO
#define CSR_CYCLEH 0xc80       // URO
#define CSR_TIMEH 0xc81        // URO
#define CSR_INSTRETH 0xc82     // URO
#define CSR_HPMCONTER3H 0xc83  // URO
#define CSR_HPMCONTER4H 0xc83  // URO
#define CSR_HPMCONTER31H 0xc9f // URO
// MACHINE INFORMATION
#define CSR_MVENDORID 0xf11  // MRO
#define CSR_MARCHID 0xf12    // MRO
#define CSR_MIMIPID 0xf13    // MRO
#define CSR_MHARTID 0xf14    // MRO
#define CSR_MCONFIGPTR 0xf15 // MRO

class CSR {
  private:
    std::map<std::string, uint32_t> bank;

  public:
    CSR() {

        // 0x300  PRIV_MACHI
        // 0xC00  PRIV_USER0
        // 0xF00  PRIV_MACHI

        // Default values
        write(CSR_MISA, 0x40000100);
        write(CSR_MTVEC, 0x10000004 | 1);
        write(CSR_MIE, 0x00000888);
        write(CSR_MSTATUS, 0x00000008);
    }

    virtual ~CSR() { bank.clear(); };

    uint32_t read(const uint32_t& address) {

        const std::string key = Debug::int_to_hex(address, 4);

        auto it = bank.find(key);
        if (it == bank.end()) {
            bank[key] = 0;
            return 0;
        } else {
            return bank[key];
        }
    }

    void write(const uint32_t& address, const uint32_t& value, const bool& isRoot = false) {

        const std::string key = Debug::int_to_hex(address, 4);

        const uint32_t isReadOnly = address >> 10;
        const uint32_t permission = (address >> 8) & 0b11;

        if (!isRoot) {
            if (isReadOnly != 0) {
                throw std::string("CSR Write: Attempt to write a read-only register");
            }
        }

        // TODO: validar range e privilegios atuais
        bank[key] = value;
        // mem.mstatus = value & (1 << 3) | (1 << 7);
    }

    void increment64(const uint32_t& addl, const uint32_t& addh, const bool& isRoot = false) {
        uint32_t low = this->read(addl);
        low++;
        this->write(addl, low, isRoot);
        if (low == 0) {
            uint32_t hi = this->read(addh);
            hi++;
            this->write(addh, hi, isRoot);
        }
    }
};
