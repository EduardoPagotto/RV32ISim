#include "include/Decode.hpp"
#include "include/Trap.hpp"

void Decode::step() {

    if (csr->resetSignal()) {
        this->reset();

    } else if (!csr->shoulStall(state)) {

        const FetchData& f = fetch->get();
        data.pc = f.pc;
        data.pcPlus4 = f.pcPlus4;
        data.instr = f.instruction;

        data.opcode = static_cast<OpCodeSet>(data.instr & 0x7f);
        data.rs1 = (data.instr >> 15) & 0x1f;
        data.rs2 = (data.instr >> 20) & 0x1f;
        data.rd = ((data.instr >> 7) & 0x1f);

        data.funct3 = (data.instr >> 12) & 0x7;
        data.funct7 = ((data.instr >> 25) & 0x7f);

        int32_t iImm = data.instr >> 20; // instr[11:0] [ 0xFFF => 2048 => -1024:1023 ]

        int32_t sImm = ((data.instr >> 25) << 5) // instr[11:5]  [ 0xFFF => 2048 => -1024:1023 ]
                       | data.rd;                // instr[4:0]

        int32_t uImm = ((data.instr >> 12) & 0xfffff) << 12; // instr[31:12] [ 0xFFFFFFFF => 4.294.967.295 ]

        int32_t jImm = ((data.instr >> 30) << 20) |          //  instr[20] [ 0xFFFFF => 1.048.575 =>  524.288:524.287 ]
                       (((data.instr >> 12) & 0xff) << 12) | //  instr[19:12]
                       (((data.instr >> 20) & 0x1) << 11) |  //  instr[11]
                       (((data.instr >> 21) & 0x3ff) << 1);  //  instr[10:1]

        // int32_t bImm = ((data.instr & 0x800) >> 11) | //  instr[12] // [0x1FFF => 8191 => -4096:4095]
        //                ((data.instr & 0x80) << 4) |   //  instr[11]
        //                ((data.instr >> 25) << 5) |    //  instr[10:5]
        //                ((data.instr >> 7) & 0x1e);    //  instr[4:1]

        int32_t bImm = ((data.instr >> 25) << 5) + ((data.instr >> 7) & 0x1f) - 1;
        if (bImm > 0) {
            bImm++;
        }

        data.opcodeSys = OpCodeSetSystem::INVALID;

        switch (data.opcode) {
            case OpCodeSet::LOAD:
            case OpCodeSet::ULAI:
            case OpCodeSet::JALR:
                data.imm32 = iImm;
                break;
            case OpCodeSet::AUIPC:
            case OpCodeSet::LUI:
                data.imm32 = uImm;
                break;
            case OpCodeSet::SAVE:
                data.imm32 = sImm;
                break;
            case OpCodeSet::ULA:
                data.imm32 = iImm;
                break;
            case OpCodeSet::BRANCH:
                data.imm32 = bImm;
                break;
            case OpCodeSet::JAL:
                data.imm32 = jImm;
                break;
            case OpCodeSet::FENCE:
                data.imm32 = iImm;
                // TODO
                break;
            case OpCodeSet::SYSTEM:
                data.imm32 = iImm;

                if (data.funct3 == 0) {
                    switch (data.imm32) {
                        case 0x0:
                            data.opcodeSys = OpCodeSetSystem::ECALL;
                            break;
                        case 0x1:
                            data.opcodeSys = OpCodeSetSystem::EBREAK;
                            break;
                        case 0x102:
                            data.opcodeSys = OpCodeSetSystem::SRET;
                            break;
                        case 0x302:
                            data.opcodeSys = OpCodeSetSystem::MRET;
                            break;
                        case 0x105:
                            data.opcodeSys = OpCodeSetSystem::WFI;
                            break;
                        default:
                            throw std::string("Opcode desconhecido");
                            break;
                    }
                } else {
                    data.opcodeSys = static_cast<OpCodeSetSystem>(data.funct3); // CSR's
                }
                break;
            default:
                throw std::string("Opcode desconhecido");
                break;
        }
    }
}
