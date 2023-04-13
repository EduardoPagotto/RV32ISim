#include "../include/Execute.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T>
inline std::string int_to_hex(T val, size_t width = sizeof(T) * 2) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
    return ss.str();
}

Execute::Execute(Bus* bus, uint32_t regs[]) {
    cpu_pc = 0;
    ecall = false;
    this->bus = bus;
    this->regs = regs;

    // Init registers
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }

    alias.push_back("r0"); // 0  -> zero
    alias.push_back("ra"); // 1  -> return address
    alias.push_back("sp"); // 2  -> stackp pointer
    alias.push_back("gp"); // 3  -> global pointer
    alias.push_back("tp"); // 4  -> thread pointer
    alias.push_back("t0"); // 5  -> temporary / alternative link
    alias.push_back("t1"); // 6  -> temporary
    alias.push_back("t2"); // 7  -> temporary

    alias.push_back("s0"); // 8  -> saved / frame pointer
    alias.push_back("s1"); // 9  -> saved
    alias.push_back("a0"); // 10 -> func arg / return value
    alias.push_back("a1"); // 11 -> func arg / return value
    alias.push_back("a2"); // 12 -> func arg
    alias.push_back("a3"); // 13 -> func arg
    alias.push_back("a4"); // 14 -> func arg
    alias.push_back("a5"); // 15 -> func arg

    alias.push_back("a6"); // 16 -> func arg
    alias.push_back("a7"); // 17 -> func arg
    alias.push_back("s2"); // 18 -> saved
    alias.push_back("s3"); // 19 -> saved
    alias.push_back("s4"); // 20 -> saved
    alias.push_back("s5"); // 21 -> saved
    alias.push_back("s6"); // 22 -> saved
    alias.push_back("s7"); // 23 -> saved

    alias.push_back("s8");  // 24 -> saved
    alias.push_back("s9");  // 25 -> saved
    alias.push_back("s10"); // 26 -> saved
    alias.push_back("s11"); // 27 -> saved
    alias.push_back("t3");  // 28 -> temporary
    alias.push_back("t4");  // 29 -> temporary
    alias.push_back("t5");  // 30 -> temporary
    alias.push_back("t6");  // 31 -> temporary
}

Execute::~Execute() {}

bool Execute::writeToFile(const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)&regs, 32 * sizeof(int));
    outfile.close();
    return true;
}

std::string Execute::printIndexValue(const uint32_t& indice) {

    std::stringstream ss;

    if (indice == 2)
        ss << alias[indice] << " = " << int_to_hex(regs[indice]);
    else
        ss << alias[indice] << " = " << static_cast<int32_t>(regs[indice]);

    return ss.str();
}

std::string Execute::printValue(const uint32_t& indice, const uint32_t value) {

    std::stringstream ss;

    if (indice == 2)
        ss << alias[indice] << " = " << int_to_hex(value);
    else
        ss << alias[indice] << " = " << static_cast<int32_t>(value);

    return ss.str();
}

void Execute::loadRegister(const Decode& i) {
    switch (i.funct3) {
        case 0x0: // lb
            std::cout << printCommandRegs("lb    ", i);
            bus->load(regs[i.rd], regs[i.rs1] + i.iImm, 1);
            break;
        case 0x1: // lH
            std::cout << printCommandRegs("lh    ", i);
            bus->load(regs[i.rd], regs[i.rs1] + i.iImm, 2);
            break;
        case 0x2: // lW
            std::cout << printCommandRegs("lw    ", i);
            bus->load(regs[i.rd], regs[i.rs1] + i.iImm, 4);
            break;
        case 0x4: // lbu
            std::cout << printCommandRegs("lbu   ", i);
            bus->load(regs[i.rd], regs[i.rs1] + i.iImm, 1, true);
            break;
        case 0x5: // lhu
            std::cout << printCommandRegs("lhu   ", i);
            bus->load(regs[i.rd], regs[i.rs1] + i.iImm, 2, true);
            break;
    }

    std::cout << printIndexValue(i.rd) << " <- MEM[ " << int_to_hex(regs[i.rs1] + i.iImm) << " ]\n";
}

void Execute::ulai(const Decode& i) {

    uint32_t val_rs = regs[i.rs1];
    switch (i.funct3) {
        case 0x0: // ADDI
            regs[i.rd] = regs[i.rs1] + i.iImm;
            std::cout << printCommandRegs("addi  ", i);
            break;
        case 0x1: // SLLI
            regs[i.rd] = regs[i.rs1] << i.iImm;
            std::cout << printCommandRegs("slli  ", i);
            break;
        case 0x2: // SLTI

            if (regs[i.rs1] < i.iImm) { // TODO: Trocar por ternario
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }

            std::cout << printCommandRegs("slti  ", i);

            break;
        case 0x3: // SLTIU

            if (regs[i.rs1] < ((unsigned int)(i.iImm & 0xfff))) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }

            std::cout << printCommandRegs("sltiu ", i);

            break;
        case 0x4: // XORI

            regs[i.rd] = regs[i.rs1] ^ i.iImm;

            std::cout << printCommandRegs("xori  ", i);

            break;
        case 0x5: // SLRI / SRAI  TODO: check!
            if ((i.iImm & 0xf00) == 0) {
                regs[i.rd] = regs[i.rs1] >> i.iImm;
                std::cout << printCommandRegs("slri  ", i);
            } else {
                regs[i.rd] = regs[i.rs1] >> (i.iImm & 0x1f);
                std::cout << printCommandRegs("srai  ", i);
            }
            break;
        case 0x6: // ORI
            regs[i.rd] = regs[i.rs1] | i.iImm;
            std::cout << printCommandRegs("ori   ", i);

            break;
        case 0x7: // ANDI
            regs[i.rd] = regs[i.rs1] & i.iImm;
            std::cout << printCommandRegs("andi  ", i);

            break;
    }

    std::cout << printIndexValue(i.rd) << "; " << printValue(i.rs1, val_rs) << '\n';
}

void Execute::auipc(const Decode& i) {
    regs[i.rd] = cpu_pc * 4 + static_cast<uint32_t>(i.uImm);
    std::cout << printCommandRegs("auipc ", i);
    std::cout << printIndexValue(i.rd) << '\n';
}

void Execute::saveRegister(const Decode& i) {
    switch (i.funct3) {
        case 0x0:
            std::cout << printCommandRegs("sb    ", i);
            bus->store(regs[i.rs2], regs[i.rs1] + i.sImm, 1);
            break;
        case 0x1:
            std::cout << printCommandRegs("sh    ", i);
            bus->store(regs[i.rs2], regs[i.rs1] + i.sImm, 2);
            break;
        case 0x2:
            std::cout << printCommandRegs("sw    ", i);
            bus->store(regs[i.rs2], regs[i.rs1] + i.sImm, 4);
            break;
    }

    std::cout << printIndexValue(i.rs2) << " -> MEM[ " << int_to_hex(regs[i.rs1] + i.sImm) << " ]\n";
}

void Execute::ula(const Decode& i) {

    uint32_t val_rs1 = regs[i.rs1];
    uint32_t val_rs2 = regs[i.rs2];
    switch (i.funct3) {
        case 0x0:
            if ((i.instr >> 25) == 0) {
                std::cout << printCommandRegs("add   ", i);
                regs[i.rd] = regs[i.rs1] + regs[i.rs2];
            } else {
                std::cout << printCommandRegs("sub   ", i);
                regs[i.rd] = regs[i.rs1] - regs[i.rs2];
            }
            break;
        case 0x1:
            std::cout << printCommandRegs("sll   ", i);
            regs[i.rd] = regs[i.rs1] << (regs[i.rs2] & 0x1f);
            break;
        case 0x2:
            std::cout << printCommandRegs("slt   ", i);
            if (regs[i.rs1] < regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x3:
            std::cout << printCommandRegs("sltu  ", i);
            if (regs[i.rs1] < (unsigned int)regs[i.rs2]) {
                regs[i.rd] = 1;
            } else {
                regs[i.rd] = 0;
            }
            break;
        case 0x4:
            std::cout << printCommandRegs("xor   ", i);
            regs[i.rd] = regs[i.rs1] ^ regs[i.rs2];
            break;
        case 0x5:
            if ((i.instr >> 25) == 0) {
                std::cout << printCommandRegs("srl   ", i);
                regs[i.rd] = ((unsigned int)regs[i.rs1]) >> (regs[i.rs2] & 0x1f);

            } else {
                std::cout << printCommandRegs("sra   ", i);
                regs[i.rd] = regs[i.rs1] >> (regs[i.rs2] & 0x1f);
            }
            break;
        case 0x6:
            std::cout << printCommandRegs("OR    ", i);
            regs[i.rd] = regs[i.rs1] | regs[i.rs2];
            break;
        case 0x7:
            std::cout << printCommandRegs("and   ", i);
            regs[i.rd] = regs[i.rs1] & regs[i.rs2];
            break;
    }

    std::cout << printIndexValue(i.rd) << "; ";
    std::cout << printValue(i.rs1, val_rs1) << "; ";
    std::cout << printValue(i.rs2, val_rs2) << '\n';
}

void Execute::lui(const Decode& i) {
    regs[i.rd] = i.uImm;
    std::cout << printCommandRegs("lui   ", i);
    std::cout << printIndexValue(i.rd) << '\n';
}

void Execute::branchCase(const Decode& i) {

    uint32_t valRs1 = regs[i.rs1];
    uint32_t valRs2 = regs[i.rs2];

    switch (i.funct3) {
        case 0x0:
            std::cout << "beq   " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 == valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
        case 0x1:
            std::cout << "bne   " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 != valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
        case 0x4:
            std::cout << "blt   " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 < valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
        case 0x5:
            std::cout << "bge   " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 >= valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
        case 0x6:
            std::cout << "bltu  " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 < (unsigned)valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
        case 0x7:
            std::cout << "bgeu  " << alias[i.rs1] << " " << alias[i.rs2] << " " << i.bImm << " \t\t# ";
            if (valRs1 >= (unsigned)valRs2) {
                cpu_pc = cpu_pc + (i.bImm / 4) - 1;
            }
            break;
    }

    std::cout << printValue(i.rs1, valRs1) << "; ";
    std::cout << printValue(i.rs2, valRs2) << "; ";
    std::cout << " PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::jalr(const Decode& i) {

    std::cout << printCommandRegs("jalr  ", i);

    regs[i.rd] = (cpu_pc + 1) << 2;
    cpu_pc = (regs[i.rs1] + i.iImm) >> 2;
    cpu_pc = cpu_pc - 1;

    std::cout << printIndexValue(i.rd) << " ; PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::jal(const Decode& i) {

    std::cout << printCommandRegs("jal ", i);

    regs[i.rd] = (cpu_pc + 1) << 2;
    cpu_pc = cpu_pc + (i.jImm >> 2) - 1; // Because of inc after switch

    std::cout << printIndexValue(i.rd) << " ; PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::step() {

    // Get fields
    uint32_t instr = 0;
    bus->load(instr, 4 * cpu_pc, 4);
    printAsHex(instr); // REMOVE

    Decode i(static_cast<int32_t>(instr));

    switch (i.opcode) {
        case 0x03:
            loadRegister(i); // load Instructions
            break;
        case 0x13:
            ulai(i); // ULAi
            break;
        case 0x17:
            auipc(i); // auipc
            break;
        case 0x23:
            saveRegister(i); // Save Instructions
            break;
        case 0x33:
            ula(i); // ULA
            break;
        case 0x37:
            lui(i); // LUI
            break;
        case 0x63:
            branchCase(i); // Branch case
            break;
        case 0x67:
            jalr(i); // jalr
            break;
        case 0x6f:
            jal(i); // jal
            break;
        case 0x73:
            std::cout << "Ecall - Exiting program" << '\n';
            ecall = true;
            break;
    }
    regs[0] = 0; // jalr x0 xX is not supposed to cast exception, so this is
                 // the easier way
    cpu_pc++;    // Increment program counter
}

std::string Execute::printCommandRegs(const std::string& com, const Decode& i) {
    std::stringstream ss;

    switch (i.opcode) {
        case 0x03:
            ss << com << alias[i.rd] << " " << i.iImm << "(" << alias[i.rs1] << ") \t\t# ";
            break;

        case 0x13:
        case 0x67:
            ss << com << alias[i.rd] << " " << alias[i.rs1] << " " << i.iImm << " \t\t# ";
            break;

        case 0x17:
        case 0x37:
            ss << com << alias[i.rd] << " " << static_cast<uint32_t>(i.uImm) << " \t\t# ";
            break;

        case 0x23:
            ss << com << alias[i.rs2] << " " << i.sImm << "(" << alias[i.rs1] << ") \t\t# ";
            break;

        case 0x33:
            ss << com << alias[i.rd] << " " << alias[i.rs1] << " " << alias[i.rs2 & 0x1f] << " \t\t# ";
            break;

        case 0x6f:
            ss << com << alias[i.rd] << " " << i.jImm << " \t\t# ";
            break;

        default:
            break;
    }
    return ss.str();
}

void Execute::printRegisters() {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << " " << regs[i] << std::endl;
    }
}

void Execute::printProgram() {
    std::cout << "Program" << '\n' << std::endl;

    uint32_t addr = 0;
    do {

        uint32_t instr;
        bus->load(instr, 4 * addr, 4);
        printAsHex(instr);
        std::cout << "" << '\n';

        addr++;

    } while (bus->hasData(addr));
}

void Execute::printAsHex(unsigned int instr) {
    int res = 0;

    std::cout << int_to_hex(cpu_pc * 4) << " ";

    for (int i = 0; i < 8; i++) {
        res = (instr >> (4 * (7 - i))) & 0xf;
        switch (res) {
            case 10:
                std::cout << "a";
                break;
            case 11:
                std::cout << "b";
                break;
            case 12:
                std::cout << "c";
                break;
            case 13:
                std::cout << "d";
                break;
            case 14:
                std::cout << "e";
                break;
            case 15:
                std::cout << "f";
                break;
            default:
                std::cout << res;
                break;
        }
    }
    std::cout << "    ";
}
