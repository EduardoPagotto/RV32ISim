#pragma once
#include "Bus.hpp"
#include "Decode.hpp"

class Execute : public PipelineStage {
  public:
    Execute(CSR* c, Bus* bus, Decode* d, uint32_t regs[])
        : PipelineStage(PipelineState::Execute, c), bus(bus), decode(d), regs(regs) {}

    virtual ~Execute() = default;

    virtual void commit() override { this->done = this->data; }
    virtual void step() override;
    virtual void reset() override;

    void printRegisters();
    // void printProgram();

    inline const ExecuteData& get() const { return data; }

  private:
    // uint32_t cpu_pc; // Program counter
    uint32_t* regs; //
    Bus* bus = nullptr;
    Decode* decode;

    uint8_t funct3;
    uint8_t funct7;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    OpCodeSet opcode;
    OpCodeSetSystem opcodeSys;
    int32_t instr;
    int32_t imm32;

    uint32_t pc;
    uint32_t pcPlus4;

    void loadRegister();
    void ulai();
    void auipc();
    void saveRegister();
    void ula();
    void lui();
    void branchCase();
    void jalr();
    void jal();

    void setSystem();

    std::string printCommandRegs(const std::string& com, const int32_t& imm);

    // bool returnFromTrap = false;
    // bool csrShouldRead, csrShouldWrite;
    // int32_t mepc, trap, mtval, mcause;
    // uint32_t csrSource, csrAddress;

    ExecuteData data;
    ExecuteData done;
};
