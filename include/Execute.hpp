#pragma once

#include "Bus.hpp"
#include "Decode.hpp"

class Execute {
  public:
    Execute(Bus* bus, uint32_t regs[]);
    virtual ~Execute();

    /**
     * @brief Dumps the registry content at current state to file
     *
     * @param filepath where to place file
     * @return true indication if write was successful \
     * @return false indication fail to write
     */
    bool writeToFile(const char* filepath);

    /**
     * @brief Step through the program by decoding and executing next instruction pointed
     * to by cpu_pc. This function contain the implementation of the RISC-V 32RVI
     * instruction subset For different subset/functionality this function must be
     * overwritten.
     */
    void step();

    /**
     * @brief indication if program is ended
     *
     * @return true \
     * @return false
     */
    inline bool hasNext() const { return ((bus->hasData(cpu_pc)) & !ecall); } // TODO: AQI!!!! ver esta variaveisZZ

    void printRegisters();
    void printProgram();

  private:
    uint32_t cpu_pc; // Program counter
    uint32_t* regs;  //
    bool ecall;      // For program termination
    Bus* bus = nullptr;

    /**
     * @brief Test function used to print content of memory
     *
     * @param instr instruction
     */
    void printAsHex(unsigned int instr);

    void loadRegister(const Decode& i);
    void ulai(const Decode& i);
    void auipc(const Decode& i);
    void saveRegister(const Decode& i);
    void ula(const Decode& i);
    void lui(const Decode& i);
    void branchCase(const Decode& i);
    void jalr(const Decode& i);
    void jal(const Decode& i);

    std::string printCommandRegs(const std::string& com, const Decode& i);

    std::vector<std::string> alias;
    std::string printIndexValue(const uint32_t& indice);
    std::string printValue(const uint32_t& indice, const uint32_t value);
};
