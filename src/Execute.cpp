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

Execute::Execute(Controller* c, Bus* bus, Decode* d, uint32_t regs[])
    : crt(c), bus(bus), decode(d), regs(regs), state(PipelineState::Execute) {

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

void Execute::loadRegister() { //

    data.address = regs[rs1] + imm32;
    data.indexRD = rd;

    switch (funct3) {
        case 0x0: // lb
            std::cout << printCommandRegs("lb    ");
            data.memSize = 1;       // 1; 2; 4
            data.valSigned = false; // true so no lbu, lhu
            // bus->load(regs[rd], regs[rs1] + imm32, 1);

            break;
        case 0x1: // lH
            std::cout << printCommandRegs("lh    ");
            data.memSize = 2;       // 1; 2; 4
            data.valSigned = false; // true so no lbu, lhu
            // bus->load(regs[rd], regs[rs1] + imm32, 2);

            break;
        case 0x2: // lW
            std::cout << printCommandRegs("lw    ");
            data.memSize = 4;       // 1; 2; 4
            data.valSigned = false; // true so no lbu, lhu
            // bus->load(regs[rd], regs[rs1] + imm32, 4);

            break;
        case 0x4: // lbu
            std::cout << printCommandRegs("lbu   ");
            data.memSize = 1;      // 1; 2; 4
            data.valSigned = true; // true so no lbu, lhu
            // bus->load(regs[rd], regs[rs1] + imm32, 1, true);

            break;
        case 0x5: // lhu
            std::cout << printCommandRegs("lhu   ");
            data.memSize = 2;      // 1; 2; 4
            data.valSigned = true; // true so no lbu, lhu
            // bus->load(regs[rd], regs[rs1] + imm32, 2, true);

            break;
    }

    std::cout << printIndexValue(rd) << " <- MEM[ " << int_to_hex(regs[rs1] + imm32) << " ]\n";
}

void Execute::ulai() {

    uint32_t vRS1 = regs[rs1];
    uint32_t vRD = 0;
    switch (funct3) {
        case 0x0: // ADDI
            vRD = vRS1 + imm32;
            std::cout << printCommandRegs("addi  ");
            break;

        case 0x1: // SLLI
            vRD = vRS1 << imm32;
            std::cout << printCommandRegs("slli  ");
            break;

        case 0x2: // SLTI
            vRD = (vRS1 < imm32) ? 1 : 0;
            std::cout << printCommandRegs("slti  ");
            break;

        case 0x3: // SLTIU
            vRD = (vRS1 < ((unsigned int)(imm32 & 0xfff))) ? 1 : 0;
            std::cout << printCommandRegs("sltiu ");
            break;

        case 0x4: // XORI
            vRD = vRS1 ^ imm32;
            std::cout << printCommandRegs("xori  ");
            break;

        case 0x5: // SLRI / SRAI  TODO: check!
            if ((imm32 & 0xf00) == 0) {
                vRD = vRS1 >> imm32;
                std::cout << printCommandRegs("slri  ");
            } else {
                vRD = vRS1 >> (imm32 & 0x1f);
                std::cout << printCommandRegs("srai  ");
            }
            break;
        case 0x6: // ORI
            vRD = vRS1 | imm32;
            std::cout << printCommandRegs("ori   ");

            break;
        case 0x7: // ANDI
            vRD = vRS1 & imm32;
            std::cout << printCommandRegs("andi  ");

            break;
    }

    data.indexRD = rd;
    data.address = vRD;

    // regs[rd] = vRD;
    std::cout << printValue(rd, vRD) << "; " << printValue(rs1, vRS1) << '\n';
}

void Execute::auipc() {

    data.address = static_cast<uint32_t>(imm32);
    data.indexRD = rd;

    // regs[rd] = static_cast<uint32_t>(imm32);
    std::cout << printCommandRegs("auipc ");
    std::cout << printValue(data.indexRD, data.address) << '\n';
}

void Execute::saveRegister() {

    data.address = regs[rs1] + imm32;
    data.valueRS2 = regs[rs2];

    switch (funct3) {
        case 0x0:
            std::cout << printCommandRegs("sb    ");
            data.memSize = 1;
            // bus->store(regs[rs2], regs[rs1] + imm32, 1);
            break;
        case 0x1:
            std::cout << printCommandRegs("sh    ");
            data.memSize = 2;
            // bus->store(regs[rs2], regs[rs1] + imm32, 2);
            break;
        case 0x2:
            std::cout << printCommandRegs("sw    ");
            data.memSize = 4;
            // bus->store(regs[rs2], regs[rs1] + imm32, 4);
            break;
    }

    std::cout << printValue(rs2, data.valueRS2) << " -> MEM[ " << int_to_hex(data.address) << " ]\n";
}

void Execute::ula() {

    uint32_t vRS1 = regs[rs1];
    uint32_t vRS2 = regs[rs2];
    uint32_t vRD = 0;
    switch (funct3) {
        case 0x0:
            if ((instr >> 25) == 0) {
                std::cout << printCommandRegs("add   ");
                vRD = vRS1 + vRS2;
            } else {
                std::cout << printCommandRegs("sub   ");
                vRD = vRS1 - vRS2;
            }
            break;
        case 0x1:
            std::cout << printCommandRegs("sll   ");
            vRD = vRS1 << (vRS2 & 0x1f);
            break;
        case 0x2:
            std::cout << printCommandRegs("slt   ");
            if (vRS1 < vRS2) {
                vRD = 1;
            } else {
                vRD = 0;
            }
            break;
        case 0x3:
            std::cout << printCommandRegs("sltu  ");
            if (vRS1 < (unsigned int)vRS2) {
                vRD = 1;
            } else {
                vRD = 0;
            }
            break;
        case 0x4:
            std::cout << printCommandRegs("xor   ");
            vRD = vRS1 ^ vRS2;
            break;
        case 0x5:
            if ((instr >> 25) == 0) {
                std::cout << printCommandRegs("srl   ");
                vRD = ((unsigned int)vRS1) >> (vRS2 & 0x1f);

            } else {
                std::cout << printCommandRegs("sra   ");
                vRD = vRS1 >> (vRS2 & 0x1f);
            }
            break;
        case 0x6:
            std::cout << printCommandRegs("OR    ");
            vRD = vRS1 | vRS2;
            break;
        case 0x7:
            std::cout << printCommandRegs("and   ");
            vRD = vRS1 & vRS2;
            break;
    }

    // regs[rd] = vRD;

    data.indexRD = rd;
    data.address = vRD;

    std::cout << printIndexValue(rd) << "; ";
    std::cout << printValue(rs1, vRS1) << "; ";
    std::cout << printValue(rs2, vRS2) << '\n';
}

void Execute::lui() {
    // regs[rd] = imm32;
    data.indexRD = rd;
    data.address = imm32;

    std::cout << printCommandRegs("lui   ");
    std::cout << printValue(rd, data.address) << '\n';
}

void Execute::branchCase() {

    uint32_t valRs1 = regs[rs1];
    uint32_t valRs2 = regs[rs2];
    std::string opp;
    bool doBranch = false;

    switch (funct3) {
        case 0x0:
            opp = "beq   ";
            if (valRs1 == valRs2)
                doBranch = true;
            break;

        case 0x1:
            opp = "bne   ";
            if (valRs1 != valRs2)
                doBranch = true;
            break;

        case 0x4:
            opp = "blt   ";
            if (valRs1 < valRs2)
                doBranch = true;
            break;

        case 0x5:
            opp = "bge   ";
            if (valRs1 >= valRs2)
                doBranch = true;
            break;

        case 0x6:
            opp = "bltu  ";
            if (valRs1 < (unsigned)valRs2)
                doBranch = true;
            break;

        case 0x7:
            opp = "bgeu  ";
            if (valRs1 >= (unsigned)valRs2)
                doBranch = true;
            break;
    }

    std::cout << printCommandRegs(opp);
    std::cout << printValue(rs1, valRs1) << "; ";
    std::cout << printValue(rs2, valRs2) << "; ";

    const uint32_t final = pc + imm32;
    if (doBranch) {
        crt->setBranchAddress(final);
        std::cout << " PC -> " << int_to_hex(final) << '\n';
    } else {
        std::cout << " PC -> " << int_to_hex(pcPlus4) << '\n';
    }
}

void Execute::jalr() {

    std::cout << printCommandRegs("jalr  ");

    // regs[rd] = pcPlus4;
    data.indexRD = rd;
    data.address = pcPlus4;

    const uint32_t vFinal = regs[rs1] + imm32;
    crt->setBranchAddress(vFinal);

    if (rd != 0)
        std::cout << printValue(data.indexRD, data.address) << " ; PC -> " << int_to_hex(vFinal) << '\n';
    else
        std::cout << printIndexValue(rs1) << " ; PC -> " << int_to_hex(vFinal) << '\n';
}

void Execute::jal() {

    std::cout << printCommandRegs("jal ");

    // regs[rd] = pcPlus4;
    data.indexRD = rd;
    data.address = pcPlus4;

    const uint32_t vFinal = pc + imm32;
    crt->setBranchAddress(vFinal); // cpu_pc = cpu_pc + (imm32 >> 2) - 1; // Because of inc after switch

    std::cout << printValue(data.indexRD, data.address) << " ; PC -> " << int_to_hex(vFinal) << '\n';
}

void Execute::reset() {}

void Execute::setSystem() {

    // returnFromTrap = imm32 == 0x302;
    switch (opcodeSys) {
        case OpCodeSetSystem::EBREAK:
            // FIXME: mover para execute ???
            // mepc = fetch->getPcPlus4();
            // mcause = static_cast<uint32_t>(MCause::Breakpoint);
            // mtval = 0;
            // trap = 1;
            break;
        case OpCodeSetSystem::ECALL:
            std::cout << "Ecall - Exiting program" << '\n';
            crt->ecall = true;
            // mepc = fetch->getPcPlus4();
            // mcause = static_cast<uint32_t>(MCause::EnvironmentCallFromMMode);
            // mtval = 0;
            // trap = 1;
            break;
        default:

            // // CSR fields
            // csrAddress = this->instr >> 20;
            // const uint32_t zImm = rs1;
            // const bool isIntegerCsr = (funct3 & 0b100) == 0b100;
            // const bool isCsrrw = (funct3 & 0b11) == 0b01;

            // csrSource = isIntegerCsr ? zImm : rs1;
            // csrShouldWrite = isCsrrw || (!isCsrrw && rs1 != 0);
            // csrShouldRead = !isCsrrw || (isCsrrw && rd != 0);

            break;
    }
}

void Execute::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        const DecodeData& d = decode->getData();

        pc = d.pc;
        pcPlus4 = d.pcPlus4;

        funct3 = d.funct3;
        funct7 = d.funct7;
        rd = d.rd;
        rs1 = d.rs1;
        rs2 = d.rs2;
        opcode = d.opcode;
        opcodeSys = d.opcodeSys;
        instr = d.instr;
        imm32 = d.imm32;

        data.opcode = opcode;
        data.opcodeSys = d.opcodeSys;
        data.funct3 = funct3;

        switch (opcode) {
            case OpCodeSet::LOAD:
                loadRegister();
                break;
            case OpCodeSet::ULAI:
                ulai();
                break;
            case OpCodeSet::AUIPC:
                auipc();
                break;
            case OpCodeSet::SAVE:
                saveRegister();
                break;
            case OpCodeSet::ULA:
                ula();
                break;
            case OpCodeSet::LUI:
                lui();
                break;
            case OpCodeSet::BRANCH:
                branchCase();
                break;
            case OpCodeSet::JALR:
                jalr();
                break;
            case OpCodeSet::JAL:
                jal();
                break;
            case OpCodeSet::FENCE:
                // TODO: implementar
                break;
            case OpCodeSet::SYSTEM:
                setSystem();
                break;
            default:
                throw std::string("Opcode desconhecido");
                break;
        }
        regs[0] = 0; // jalr x0 xX is not supposed to cast exception, so this is
                     // the easier way

        // returnFromTrap = false; // FIXME:
    }
}

std::string Execute::printCommandRegs(const std::string& com) {
    std::stringstream ss;

    switch (opcode) {
        case OpCodeSet::LOAD:
            ss << com << alias[rd] << " " << imm32 << "(" << alias[rs1] << ") \t\t# ";
            break;

        case OpCodeSet::ULAI:
        case OpCodeSet::JALR:
            ss << com << alias[rd] << " " << alias[rs1] << " " << imm32 << " \t\t# ";
            break;

        case OpCodeSet::AUIPC:
        case OpCodeSet::LUI:
            ss << com << alias[rd] << " " << static_cast<uint32_t>(imm32) << " \t\t# ";
            break;

        case OpCodeSet::SAVE:
            ss << com << alias[rs2] << " " << imm32 << "(" << alias[rs1] << ") \t\t# ";
            break;

        case OpCodeSet::ULA:
            ss << com << alias[rd] << " " << alias[rs1] << " " << alias[rs2 & 0x1f] << " \t\t# ";
            break;

        case OpCodeSet::BRANCH:
            ss << com << alias[rs1] << " " << alias[rs2] << " " << imm32 << " \t\t# ";
            break;

        case OpCodeSet::JAL:
            ss << com << alias[rd] << " " << imm32 << " \t\t# ";
            break;

        default:
            break;
    }
    return ss.str();
}
