#include "../include/Decode.hpp"
#include "../include/Trap.hpp"

Decode::Decode(Controller* c, Fetch* f) {
    this->crt = c;
    this->fetch = f;
    this->state = PipelineState::Decode;
}

void Decode::reset() {}

void Decode::step() {

    if (crt->resetSignal()) {
        this->reset();

    } else if (!crt->shoulStall(state)) {

        this->pc = fetch->getPc();
        this->pcPlus4 = fetch->getPcPlus4();

        this->instr = fetch->getInstruction();
        this->opcode = instr & 0x7f;
        this->rs1 = (instr >> 15) & 0x1f;
        this->rs2 = (instr >> 20) & 0x1f;
        this->rd = ((instr >> 7) & 0x1f);

        this->funct3 = (instr >> 12) & 0x7;
        this->funct7 = ((instr >> 25) & 0x7f);

        int32_t iImm = instr >> 20; // instr[11:0] [ 0xFFF => 2048 => -1024:1023 ]

        int32_t sImm = ((instr >> 25) << 5) // instr[11:5]  [ 0xFFF => 2048 => -1024:1023 ]
                       | this->rd;          // instr[4:0]

        int32_t uImm = ((this->instr >> 12) & 0xfffff) << 12; // instr[31:12] [ 0xFFFFFFFF => 4.294.967.295 ]

        int32_t jImm = ((this->instr >> 30) << 20) |          //  instr[20] [ 0xFFFFF => 1.048.575 =>  524.288:524.287 ]
                       (((this->instr >> 12) & 0xff) << 12) | //  instr[19:12]
                       (((this->instr >> 20) & 0x1) << 11) |  //  instr[11]
                       (((this->instr >> 21) & 0x3ff) << 1);  //  instr[10:1]

        // int32_t bImm = ((this->instr & 0x800) >> 11) | //  instr[12] // [0x1FFF => 8191 => -4096:4095]
        //                ((this->instr & 0x80) << 4) |   //  instr[11]
        //                ((this->instr >> 25) << 5) |    //  instr[10:5]
        //                ((this->instr >> 7) & 0x1e);    //  instr[4:1]

        int32_t bImm = ((this->instr >> 25) << 5) + ((this->instr >> 7) & 0x1f) - 1;
        if (bImm > 0) {
            bImm++;
        }

        switch (opcode) {
            case 0x03:
            case 0x13:
            case 0x67:
                imm32 = iImm;
                break;
            case 0x17:
            case 0x37:
                imm32 = uImm;
                break;
            case 0x23:
                imm32 = sImm;
                break;
            case 0x33:
                imm32 = iImm;
                break;
            case 0x63:
                imm32 = bImm;
                break;
            case 0x6f:
                imm32 = jImm;
                break;
            case 0x0f:
                imm32 = iImm; // ler dados posteriormente
                // type.set(static_cast<uint8_t>(OpcodeType::FENCE));
                break;
            case 0x73:
                imm32 = iImm;
                returnFromTrap = imm32 == 0x302;
                if ((this->instr >> 7) == 0) {
                    // ECALL
                    crt->ecall = true;

                    mepc = fetch->getPcPlus4();
                    mcause = static_cast<uint32_t>(MCause::EnvironmentCallFromMMode);
                    mtval = 0;
                    trap = 1;
                    return;

                } else {
                    // EBREAK;
                    mepc = fetch->getPcPlus4();
                    mcause = static_cast<uint32_t>(MCause::Breakpoint);
                    mtval = 0;
                    trap = 1;
                    return;
                }

            default:
                break;
        }

        // CSR fields
        csrAddress = this->instr >> 20;
        const uint32_t zImm = rs1;
        const bool isIntegerCsr = (funct3 & 0b100) == 0b100;
        const bool isCsrrw = (funct3 & 0b11) == 0b01;

        csrSource = isIntegerCsr ? zImm : rs1;
        csrShouldWrite = isCsrrw || (!isCsrrw && rs1 != 0);
        csrShouldRead = !isCsrrw || (isCsrrw && rd != 0);

    } else {
        returnFromTrap = false;
    }
}
