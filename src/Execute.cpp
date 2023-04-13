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

Execute::Execute(Controller* c, Bus* bus, Decode* d, uint32_t regs[]) {
    cpu_pc = 0;
    ecall = false;
    this->crt = c;
    this->bus = bus;
    this->decode = d;
    this->regs = regs;

    state = PipelineState::Execute;

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

void Execute::loadRegister() {
    switch (funct3) {
        case 0x0: // lb
            std::cout << printCommandRegs("lb    ");
            bus->load(regs[rd], regs[rs1] + imm32, 1);
            break;
        case 0x1: // lH
            std::cout << printCommandRegs("lh    ");
            bus->load(regs[rd], regs[rs1] + imm32, 2);
            break;
        case 0x2: // lW
            std::cout << printCommandRegs("lw    ");
            bus->load(regs[rd], regs[rs1] + imm32, 4);
            break;
        case 0x4: // lbu
            std::cout << printCommandRegs("lbu   ");
            bus->load(regs[rd], regs[rs1] + imm32, 1, true);
            break;
        case 0x5: // lhu
            std::cout << printCommandRegs("lhu   ");
            bus->load(regs[rd], regs[rs1] + imm32, 2, true);
            break;
    }

    std::cout << printIndexValue(rd) << " <- MEM[ " << int_to_hex(regs[rs1] + imm32) << " ]\n";
}

void Execute::ulai() {

    uint32_t val_rs = regs[rs1];
    switch (funct3) {
        case 0x0: // ADDI
            regs[rd] = regs[rs1] + imm32;
            std::cout << printCommandRegs("addi  ");
            break;
        case 0x1: // SLLI
            regs[rd] = regs[rs1] << imm32;
            std::cout << printCommandRegs("slli  ");
            break;
        case 0x2: // SLTI

            if (regs[rs1] < imm32) { // TODO: Trocar por ternario
                regs[rd] = 1;
            } else {
                regs[rd] = 0;
            }

            std::cout << printCommandRegs("slti  ");

            break;
        case 0x3: // SLTIU

            if (regs[rs1] < ((unsigned int)(imm32 & 0xfff))) {
                regs[rd] = 1;
            } else {
                regs[rd] = 0;
            }

            std::cout << printCommandRegs("sltiu ");

            break;
        case 0x4: // XORI

            regs[rd] = regs[rs1] ^ imm32;

            std::cout << printCommandRegs("xori  ");

            break;
        case 0x5: // SLRI / SRAI  TODO: check!
            if ((imm32 & 0xf00) == 0) {
                regs[rd] = regs[rs1] >> imm32;
                std::cout << printCommandRegs("slri  ");
            } else {
                regs[rd] = regs[rs1] >> (imm32 & 0x1f);
                std::cout << printCommandRegs("srai  ");
            }
            break;
        case 0x6: // ORI
            regs[rd] = regs[rs1] | imm32;
            std::cout << printCommandRegs("ori   ");

            break;
        case 0x7: // ANDI
            regs[rd] = regs[rs1] & imm32;
            std::cout << printCommandRegs("andi  ");

            break;
    }

    std::cout << printIndexValue(rd) << "; " << printValue(rs1, val_rs) << '\n';
}

void Execute::auipc() {
    regs[rd] = cpu_pc * 4 + static_cast<uint32_t>(imm32);
    std::cout << printCommandRegs("auipc ");
    std::cout << printIndexValue(rd) << '\n';
}

void Execute::saveRegister() {
    switch (funct3) {
        case 0x0:
            std::cout << printCommandRegs("sb    ");
            bus->store(regs[rs2], regs[rs1] + imm32, 1);
            break;
        case 0x1:
            std::cout << printCommandRegs("sh    ");
            bus->store(regs[rs2], regs[rs1] + imm32, 2);
            break;
        case 0x2:
            std::cout << printCommandRegs("sw    ");
            bus->store(regs[rs2], regs[rs1] + imm32, 4);
            break;
    }

    std::cout << printIndexValue(rs2) << " -> MEM[ " << int_to_hex(regs[rs1] + imm32) << " ]\n";
}

void Execute::ula() {

    uint32_t val_rs1 = regs[rs1];
    uint32_t val_rs2 = regs[rs2];
    switch (funct3) {
        case 0x0:
            if ((instr >> 25) == 0) {
                std::cout << printCommandRegs("add   ");
                regs[rd] = regs[rs1] + regs[rs2];
            } else {
                std::cout << printCommandRegs("sub   ");
                regs[rd] = regs[rs1] - regs[rs2];
            }
            break;
        case 0x1:
            std::cout << printCommandRegs("sll   ");
            regs[rd] = regs[rs1] << (regs[rs2] & 0x1f);
            break;
        case 0x2:
            std::cout << printCommandRegs("slt   ");
            if (regs[rs1] < regs[rs2]) {
                regs[rd] = 1;
            } else {
                regs[rd] = 0;
            }
            break;
        case 0x3:
            std::cout << printCommandRegs("sltu  ");
            if (regs[rs1] < (unsigned int)regs[rs2]) {
                regs[rd] = 1;
            } else {
                regs[rd] = 0;
            }
            break;
        case 0x4:
            std::cout << printCommandRegs("xor   ");
            regs[rd] = regs[rs1] ^ regs[rs2];
            break;
        case 0x5:
            if ((instr >> 25) == 0) {
                std::cout << printCommandRegs("srl   ");
                regs[rd] = ((unsigned int)regs[rs1]) >> (regs[rs2] & 0x1f);

            } else {
                std::cout << printCommandRegs("sra   ");
                regs[rd] = regs[rs1] >> (regs[rs2] & 0x1f);
            }
            break;
        case 0x6:
            std::cout << printCommandRegs("OR    ");
            regs[rd] = regs[rs1] | regs[rs2];
            break;
        case 0x7:
            std::cout << printCommandRegs("and   ");
            regs[rd] = regs[rs1] & regs[rs2];
            break;
    }

    std::cout << printIndexValue(rd) << "; ";
    std::cout << printValue(rs1, val_rs1) << "; ";
    std::cout << printValue(rs2, val_rs2) << '\n';
}

void Execute::lui() {
    regs[rd] = imm32;
    std::cout << printCommandRegs("lui   ");
    std::cout << printIndexValue(rd) << '\n';
}

void Execute::branchCase() {

    uint32_t valRs1 = regs[rs1];
    uint32_t valRs2 = regs[rs2];

    switch (funct3) {
        case 0x0:
            std::cout << "beq   " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 == valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
        case 0x1:
            std::cout << "bne   " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 != valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
        case 0x4:
            std::cout << "blt   " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 < valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
        case 0x5:
            std::cout << "bge   " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 >= valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
        case 0x6:
            std::cout << "bltu  " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 < (unsigned)valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
        case 0x7:
            std::cout << "bgeu  " << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            if (valRs1 >= (unsigned)valRs2) {
                cpu_pc = cpu_pc + (imm32 / 4) - 1;
            }
            break;
    }

    std::cout << printValue(rs1, valRs1) << "; ";
    std::cout << printValue(rs2, valRs2) << "; ";
    std::cout << " PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::jalr() {

    std::cout << printCommandRegs("jalr  ");

    regs[rd] = (cpu_pc + 1) << 2;
    cpu_pc = (regs[rs1] + imm32) >> 2;
    cpu_pc = cpu_pc - 1;

    std::cout << printIndexValue(rd) << " ; PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::jal() {

    std::cout << printCommandRegs("jal ");

    regs[rd] = (cpu_pc + 1) << 2;
    cpu_pc = cpu_pc + (imm32 >> 2) - 1; // Because of inc after switch

    std::cout << printIndexValue(rd) << " ; PC -> " << int_to_hex((cpu_pc * 4) + 4) << '\n';
}

void Execute::reset() {}

void Execute::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        funct3 = decode->getFunct3();
        funct7 = decode->getFunct7();
        rd = decode->getRD();
        rs1 = decode->getRS1();
        rs2 = decode->getRS2();
        opcode = decode->getOpCode();
        instr = decode->getInstruction();
        imm32 = decode->getImm32();

        switch (opcode) {
            case 0x03:
                loadRegister(); // load Instructions
                break;
            case 0x13:
                ulai(); // ULAi
                break;
            case 0x17:
                auipc(); // auipc
                break;
            case 0x23:
                saveRegister(); // Save Instructions
                break;
            case 0x33:
                ula(); // ULA
                break;
            case 0x37:
                lui(); // LUI
                break;
            case 0x63:
                branchCase(); // Branch case
                break;
            case 0x67:
                jalr(); // jalr
                break;
            case 0x6f:
                jal(); // jal
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
}

std::string Execute::printCommandRegs(const std::string& com) {
    std::stringstream ss;

    switch (opcode) {
        case 0x03:
            ss << com << alias[rd] << " " << imm32 << "(" << alias[rs1] << ") \t\t# ";
            break;

        case 0x13:
        case 0x67:
            ss << com << alias[rd] << " " << alias[rs1] << " " << imm32 << " \t\t# ";
            break;

        case 0x17:
        case 0x37:
            ss << com << alias[rd] << " " << static_cast<uint32_t>(imm32) << " \t\t# ";
            break;

        case 0x23:
            ss << com << alias[rs2] << " " << imm32 << "(" << alias[rs1] << ") \t\t# ";
            break;

        case 0x33:
            ss << com << alias[rd] << " " << alias[rs1] << " " << alias[rs2 & 0x1f] << " \t\t# ";
            break;

        case 0x6f:
            ss << com << alias[rd] << " " << imm32 << " \t\t# ";
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
