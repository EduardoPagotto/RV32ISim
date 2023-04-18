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
    : crt(c), bus(bus), decode(d), regs(regs), state(PipelineState::Execute) {}

Execute::~Execute() {}

bool Execute::writeToFile(const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)&regs, 32 * sizeof(int));
    outfile.close();
    return true;
}

void Execute::loadRegister() {

    data.address = regs[rs1] + imm32;
    data.index = rd;

    switch (funct3) {
        case 0x0: // lb
            data.memSize = 1;
            data.valSigned = false;
            break;
        case 0x1: // lH
            data.memSize = 2;
            data.valSigned = false;
            break;
        case 0x2: // lW
            data.memSize = 4;
            data.valSigned = false;
            break;
        case 0x4: // lbu
            data.memSize = 1;
            data.valSigned = true;
            break;
        case 0x5: // lhu
            data.memSize = 2;
            data.valSigned = true;
            break;
    }
}

void Execute::ulai() {

    uint32_t vRS1 = regs[rs1];
    uint32_t vRD = 0;
    switch (funct3) {
        case 0x0: // ADDI
            vRD = vRS1 + imm32;
            break;

        case 0x1: // SLLI
            vRD = vRS1 << imm32;
            break;

        case 0x2: // SLTI
            vRD = (vRS1 < imm32) ? 1 : 0;
            break;

        case 0x3: // SLTIU
            vRD = (vRS1 < ((unsigned int)(imm32 & 0xfff))) ? 1 : 0;
            break;

        case 0x4: // XORI
            vRD = vRS1 ^ imm32;
            break;

        case 0x5: // SLRI / SRAI
            vRD = ((imm32 & 0xf00) == 0) ? vRS1 >> imm32 : vRS1 >> (imm32 & 0x1f);
            break;

        case 0x6: // ORI
            vRD = vRS1 | imm32;
            break;

        case 0x7: // ANDI
            vRD = vRS1 & imm32;
            break;
    }

    data.index = rd;
    data.address = vRD;
}

void Execute::auipc() {
    data.address = static_cast<uint32_t>(imm32);
    data.index = rd;
}

void Execute::saveRegister() {

    data.address = regs[rs1] + imm32;
    data.valueRS = regs[rs2];
    data.index = rs2;

    switch (funct3) {
        case 0x0:
            data.memSize = 1;
            break;
        case 0x1:
            data.memSize = 2;
            break;
        case 0x2:
            data.memSize = 4;
            break;
    }
}

void Execute::ula() {

    uint32_t vRS1 = regs[rs1];
    uint32_t vRS2 = regs[rs2];
    uint32_t vRD = 0;
    switch (funct3) {
        case 0x0:
            vRD = ((instr >> 25) == 0) ? vRS1 + vRS2 : vRS1 - vRS2;
            break;

        case 0x1:
            vRD = vRS1 << (vRS2 & 0x1f);
            break;

        case 0x2:
            vRD = (vRS1 < vRS2) ? 1 : 0;
            break;

        case 0x3:
            vRD = (vRS1 < (unsigned int)vRS2) ? 1 : 0;
            break;

        case 0x4:
            vRD = vRS1 ^ vRS2;
            break;

        case 0x5:
            vRD = ((instr >> 25) == 0) ? ((unsigned int)vRS1) >> (vRS2 & 0x1f) : vRS1 >> (vRS2 & 0x1f);
            break;

        case 0x6:
            vRD = vRS1 | vRS2;
            break;

        case 0x7:
            vRD = vRS1 & vRS2;
            break;
    }

    data.index = rd;
    data.address = vRD;
}

void Execute::lui() {
    data.index = rd;
    data.address = imm32;
}

void Execute::branchCase() {

    uint32_t const valRs1 = regs[rs1];
    uint32_t const valRs2 = regs[rs2];
    bool doBranch = false;

    switch (funct3) {
        case 0x0:
            if (valRs1 == valRs2)
                doBranch = true;
            break;

        case 0x1:
            if (valRs1 != valRs2)
                doBranch = true;
            break;

        case 0x4:
            if (valRs1 < valRs2)
                doBranch = true;
            break;

        case 0x5:
            if (valRs1 >= valRs2)
                doBranch = true;
            break;

        case 0x6:
            if (valRs1 < (unsigned)valRs2)
                doBranch = true;
            break;

        case 0x7:
            if (valRs1 >= (unsigned)valRs2)
                doBranch = true;
            break;
    }

    const uint32_t final = pc + imm32;
    if (doBranch) {
        std::cout << "pc = " << int_to_hex(final) << " ";
        crt->setBranchAddress(final);
    }
}

void Execute::jalr() {
    data.index = rd;
    data.address = pcPlus4;
    std::cout << "pc = " << int_to_hex(regs[rs1] + imm32) << " ";
    crt->setBranchAddress(regs[rs1] + imm32);
}

void Execute::jal() {
    data.index = rd;
    data.address = pcPlus4;
    std::cout << "pc = " << int_to_hex(pc + imm32) << " ";
    crt->setBranchAddress(pc + imm32);
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

        case OpCodeSetSystem::CSRRC:
        case OpCodeSetSystem::CSRRCI:
        case OpCodeSetSystem::CSRRS:
        case OpCodeSetSystem::CSRRSI:
        case OpCodeSetSystem::CSRRW:
        case OpCodeSetSystem::CSRRWI:
            data.address = imm32;
            data.index = rs1;
            data.valueRS = regs[rs1];
            data.valueRD = regs[rd];
            break;

        case OpCodeSetSystem::INVALID:
            break;
    }
}

void Execute::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        const DecodeData& d = decode->getData();

        std::cout << crt->debugCommandRegs(d);

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
