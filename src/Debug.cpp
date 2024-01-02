#include "include/Debug.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>

void Debug::init() {
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

std::string Debug::printValue(const uint32_t& indice, const uint32_t value) {

    std::stringstream ss;

    if (indice == 2)
        ss << alias[indice] << " = " << int_to_hex(value);
    else
        ss << alias[indice] << " = " << static_cast<int32_t>(value);

    return ss.str();
}

void Debug::printAsHex(const uint32_t& addr, const uint32_t& instr) {
    int res = 0;

    std::cout << int_to_hex(addr) << " ";

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

void Debug::newline() { std::cout << '\n'; };

void Debug::printPC(const uint32_t& val) { std::cout << "pc < " << int_to_hex(val) << " "; }

void Debug::printRegtoMemory(const uint8_t& index, const uint32_t& valRS, const uint32_t& address) {
    std::cout << alias[index] << " = " << int_to_hex(valRS) << " > (0x" << int_to_hex(address) << ")";
}

void Debug::printRegVal(const uint8_t& index, const uint32_t& value) {
    std::cout << "\t# " << alias[index] << " <- " << int_to_hex(value);
}

void Debug::printAddress(const uint32_t& addr) { std::cout << "\t (0x" << int_to_hex(addr) << ") > "; }

void Debug::printRegisters(uint32_t* regs) {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << ", " << regs[i] << std::endl;
    }
}

bool Debug::writeToFile(uint32_t* regs, const char* filepath) {
    std::ofstream outfile(filepath, std::ios::binary);
    outfile.write((char*)regs, 32 * sizeof(int));
    outfile.close();
    return true;
}
