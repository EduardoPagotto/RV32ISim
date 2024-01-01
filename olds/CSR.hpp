#pragma once
#include "PrintAs.hpp"
#include "defs.hpp"

// const MSTATUS_MASK = (1 << 3) | (1 << 7);

// PRIVILEGES
#define PRIV_USER0 0x00
#define PRIV_SUPER 0x04
#define PRIV_HYPER 0x80
#define PRIV_MACHI 0xC0
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

enum class MCause : uint32_t {
    // Interrupts
    UserSoftwareInterrupt = 0x80000000,
    SupervisorSoftwareInterrupt,
    Reserved0,
    MachineSoftwareInterrupt,
    UserTimerInterrupt,
    SupervisorTimerInterrupt,
    Reserved1,
    MachineTimerInterrupt,
    UserExternalInterrupt,
    SupervisorExternalInterrupt,
    Reserved2,
    MachineExternalInterrupt,
    // Exceptions
    InstructionAddressMisaligned = 0x00000000,
    InstructionAccessFault,
    IllegalInstruction,
    Breakpoint,
    LoadAddressMisaligned,
    LoadAccessFault,
    StoreAMOAddressMisaligned,
    StoreAMOAccessFault,
    EnvironmentCallFromUMode,
    EnvironmentCallFromSMode,
    Reserved3,
    EnvironmentCallFromMMode,
    InstructionPageFault,
    LoadPageFault,
    Reserved4,
    StoreAMOPageFault,
};

enum class TrapState { Idle, SetCSRJump, ReturnFromTrap, SetPc };

struct Trap {
    Trap() = default;
    Trap(const uint32_t& addr, MCause cause, uint32_t mtval)
        : trapState(TrapState::SetCSRJump), mepc(addr), mcause(cause), mtval(mtval), pcToSet(0) {}

    Trap(const Trap& t) = default;
    ~Trap() = default;

    void setPC(const uint32_t& mtvec) {
        const uint32_t mc = static_cast<uint32_t>(mcause);
        const uint32_t index = mc & 0x7fffffff;
        const bool isInterrupt = mc & 0x80000000;
        const uint32_t offset = isInterrupt ? 0 : 48;

        if ((mtvec & 01) == 01) {
            pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        } else {
            pcToSet = (mtvec & 0xfffffffc);
        }

        // pcToSet = (mtvec & 0xfffffffc) + offset + (index << 2);
        trapState = TrapState::SetPc;
    }

    TrapState trapState = TrapState::Idle;
    uint32_t mepc = 0;
    MCause mcause = MCause::IllegalInstruction;
    uint32_t mtval = 0;
    uint32_t pcToSet = 0;
};

#define MSTATUS_MIE_BIT 3
#define MSTATUS_MIE_MASK (1 << MSTATUS_MIE_BIT)
#define MSTATUS_MPIE_BIT 7
#define MSTATUS_MPIE_MASK (1 << MSTATUS_MPIE_BIT)

class CSR {
  public:
    CSR();
    virtual ~CSR() = default;

    void step();
    void commit() { this->nextState(); }

    uint32_t read(uint32_t address);
    void write(uint32_t address, uint32_t value);

    // Controller
    const bool resetSignal() const { return false; }

    const bool getBranchAddressValid() { return branchAddressValid; }

    const uint32_t getBranchAddress() {
        branchAddressValid = false;
        return branchAddress;
    }
    void setBranchAddress(const uint32_t& addr) {
        branchAddress = addr;
        branchAddressValid = true;

        prt.printPC(addr); // TODO: melhorar
    }

    bool shoulStall(PipelineState state) {
        return (this->cpuState == CPUState::Pipeline) ? !(state == this->pipelineState) : true;
    }

    void trapException(const Trap& t) {
        trap = t;
        this->cpuState = CPUState::Trap;
    }

    void trapReturn() { trap.trapState = TrapState::ReturnFromTrap; }
    //  bool beginTrapReturn() { return false; }

    void nextState();

  public: // FIXME em Trap
    PrintAs prt;

  private:
    void increment64(const uint32_t add, const uint32_t add2);

    Trap trap;
    CSRBus bus;

    // controller
    CPUState cpuState = CPUState::Pipeline;
    PipelineState pipelineState = PipelineState::Fetch;
    uint32_t branchAddress = 0;
    bool branchAddressValid = false;
};