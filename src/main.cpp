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

void loadFile(const std::string& file, std::vector<uint8_t>& buffer) {

    std::ifstream instream(file, std::ios::in | std::ios::binary);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());

    for (auto v : data)
        buffer.push_back(v);
}

int main(int argc, char** argv) {

    // Check for input
    if (argc < 2) {
        std::cout << "Filepath to binary input file required" << '\n';
        return 0;
    }

    const char* filepath = argv[1];

    Memory rom(0x0000, 0x1000, DEV_OPENED);
    Memory ram(0x1000, 0x1000, DEV_OPENED | DEV_RW);

    loadFile(filepath, rom.getBank());

    Riscv32 processor(0);
    processor.getBus().add(&rom);
    processor.getBus().add(&ram);
    processor.reset();

    while (true) {
        if (!processor.step())
            break;
    }

    // // Construct simulator object
    // RV32ISim simulate(&bus);

    // // Run the program
    // try {
    //     simulate.play();
    // } catch (std::string& v) { std::cout << v << '\n'; }

    // // Perhaps get name of output file
    // const char* outFile;
    // if (argc != 2) {
    //     outFile = argv[2];
    // } else {
    //     outFile = "out.res";
    // }

    // Dump the register content to file
    // simulate.writeToFile(outFile);

    // std::cout << '\n' << "Register content dumped in file: " << outFile << '\n';

    return 0;
}
