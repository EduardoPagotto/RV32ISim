/*
 * @file main.cpp
 * @author Eduardo Pagotto
 * RV32I simulator program
 */

#include "include/Riscv32.hpp"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void loadFile(const std::string& file, std::vector<uint8_t>* buffer) {

    std::ifstream instream(file, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());

    for (uint32_t i{0}; i < data.size(); i++) {
        (*buffer)[i] = data[i];
    }
}

int main(int argc, char** argv) {

    Debug::init();

    // Check for input
    if (argc < 2) {
        std::cout << "Filepath to binary input file required" << '\n';
        return 0;
    }

    const char* filepath = argv[1];

    Memory rom(0x0000, 0x100, DEV_OPENED);
    Memory ram(0x100, 0x100, DEV_OPENED | DEV_RW);

    loadFile(filepath, rom.getBank());

    Riscv32 processor(0);
    processor.getBus().add(&rom);
    processor.getBus().add(&ram);
    processor.reset();

    try {
        while (true) {
            if (!processor.step())
                break;
        }
    } catch (std::string e) { std::cout << '\n' << "Erro: " << e << '\n'; }

    return 0;
}
