#include "../include/Decode.hpp"

Decode::Decode(const int32_t& instr) {

    this->instr = instr;
    this->opcode = instr & 0x7f;
    this->rs1 = (instr >> 15) & 0x1f;
    this->rs2 = (instr >> 20) & 0x1f;
    this->rd = ((instr >> 7) & 0x1f);

    this->funct3 = (instr >> 12) & 0x7;
    this->funct7 = ((instr >> 25) & 0x7f);

    this->iImm = instr >> 20; // instr[11:0] [ 0xFFF => 2048 => -1024:1023 ]

    this->sImm = ((instr >> 25) << 5) // instr[11:5]  [ 0xFFF => 2048 => -1024:1023 ]
                 | this->rd;          // instr[4:0]

    this->uImm = ((this->instr >> 12) & 0xfffff) << 12; // instr[31:12] [ 0xFFFFFFFF => 4.294.967.295 ]

    this->jImm = ((this->instr >> 30) << 20) |          //  instr[20] [ 0xFFFFF => 1.048.575 =>  524.288:524.287 ]
                 (((this->instr >> 12) & 0xff) << 12) | //  instr[19:12]
                 (((this->instr >> 20) & 0x1) << 11) |  //  instr[11]
                 (((this->instr >> 21) & 0x3ff) << 1);  //  instr[10:1]

    // this->bImm = ((this->instr & 0x800) >> 11) | //  instr[12] // [0x1FFF => 8191 => -4096:4095]
    //              ((this->instr & 0x80) << 4) |   //  instr[11]
    //              ((this->instr >> 25) << 5) |    //  instr[10:5]
    //              ((this->instr >> 7) & 0x1e);    //  instr[4:1]

    this->bImm = ((this->instr >> 25) << 5) + ((this->instr >> 7) & 0x1f) - 1;
    if (this->bImm > 0) {
        this->bImm++;
    }
}

void Decode::compute() {

    // if (crt->resetSignal()) {
    //     this->reset();

    // } else if (!crt->shoulStall(state)) {

    //     uint32_t i = fetch->getInstruction();

    //     // primary fields
    //     this->opcode = instr & 0x7f;
    //     this->rs1 = (instr >> 15) & 0x1f;
    //     this->rs2 = (instr >> 20) & 0x1f;
    //     this->rd = ((instr >> 7) & 0x1f);

    //     this->funct3 = (instr >> 12) & 0x7;
    //     this->funct7 = ((instr >> 25) & 0x7f);

    //     // CSR fields
    //     csrAddress.set(i >> 20);
    //     const uint32_t zImm = irs1.next();
    //     const bool isIntegerCsr = (func3.next() & 0b100) == 0b100;
    //     const bool isCsrrw = (func3.next() & 0b11) == 0b01;

    //     csrSource.set(isIntegerCsr ? zImm : irs1.next()); // ??
    //     csrShouldWrite.set(isCsrrw || (!isCsrrw && irs1.next() != 0));
    //     csrShouldRead.set(!isCsrrw || (isCsrrw && ird.next() != 0));

    //     // Address set
    //     uint32_t iImm32 = i >> 20;
    //     uint32_t uImm32 = (i >> 12) << 12;
    //     uint32_t sImm32 = ((i & 0xfe000000) >> 20) | ((i >> 7) & 0x1f);
    //     uint32_t bImm32 = ((i & 0x80000000) >> 19) | ((i & 0x40) << 4) | ((i >> 20) & 0x1f) | ((i >> 7) & 0x1e);
    //     uint32_t jImm32 = ((i >> 11) & 0x80000) | (i & 0x7FE00) | ((i & 0x40000) >> 9) | ((i & 0x7FE00000) >> 20);

    //     type.set(static_cast<uint8_t>(OpcodeType::OTHER));
    //     switch (opcode.next()) {
    //         case 0x03:
    //         case 0x13:
    //         case 0x67:
    //             type.set(static_cast<uint8_t>(OpcodeType::I));
    //             imm32.set(iImm32);
    //             break;
    //         case 0x17:
    //         case 0x37:
    //             type.set(static_cast<uint8_t>(OpcodeType::U));
    //             imm32.set(uImm32);
    //             break;
    //         case 0x23:
    //             type.set(static_cast<uint8_t>(OpcodeType::S));
    //             imm32.set(sImm32);
    //             break;
    //         case 0x33:
    //             type.set(static_cast<uint8_t>(OpcodeType::R));
    //             break;
    //         case 0x63:
    //             type.set(static_cast<uint8_t>(OpcodeType::B));
    //             imm32.set(bImm32); // so par
    //             break;
    //         case 0x6f:
    //             type.set(static_cast<uint8_t>(OpcodeType::J));
    //             imm32.set(jImm32); // so par
    //             break;
    //         case 0x0f:
    //             imm32.set(iImm32); // ler dados posteriormente
    //             type.set(static_cast<uint8_t>(OpcodeType::FENCE));
    //             break;
    //         case 0x73:
    //             imm32.set(iImm32);
    //             returnFromTrap.set(iImm32 == 0x302);
    //             if ((i >> 7) == 0) {
    //                 type.set(static_cast<uint8_t>(OpcodeType::ECALL));

    //                 mepc.set(fetch->getPcPlus4());
    //                 mcause.set(static_cast<uint32_t>(MCause::EnvironmentCallFromMMode));
    //                 mtval.set(0);
    //                 trap.set(1);
    //                 return;

    //             } else {
    //                 type.set(static_cast<uint8_t>(OpcodeType::EBREAK));

    //                 // debugger;
    //                 mepc.set(fetch->getPcPlus4());

    //                 mcause.set(static_cast<uint32_t>(MCause::Breakpoint));
    //                 mtval.set(0);
    //                 trap.set(1);
    //                 return;
    //             }

    //         default:
    //             type.set(static_cast<uint8_t>(OpcodeType::OTHER));
    //             break;
    //     }
    // } else {
    //     returnFromTrap.set(false);
    // }
}