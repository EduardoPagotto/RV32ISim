/*
 * @file main.cpp
 * @author Eduardo Pagotto
 * RV32I simulator program
 */

#include "include/Memory.hpp"
#include "include/RV32ISim.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {

    // Check for input
    if (argc < 2) {
        std::cout << "Filepath to binary input file required" << '\n';
        return 0;
    }

    const char* filepath = argv[1];

    Bus bus;
    // uint32_t idRom = bus.add(new Device(0x0000, 0x1000, DEV_OPENED));          // ROM
    // uint32_t idRam = bus.add(new Device(0x1000, 0x1000, DEV_OPENED | DEV_RW)); // RAM
    uint32_t idMem = bus.add(new Memory(0x0, 0x1000000, DEV_OPENED | DEV_RW)); // RAM

    bus.loadFile(filepath, idMem);

    // Construct simulator object
    RV32ISim simulate(&bus);

    // Run the program
    try {
        simulate.play();
    } catch (std::string& v) { std::cout << v << '\n'; }

    // Perhaps get name of output file
    const char* outFile;
    if (argc != 2) {
        outFile = argv[2];
    } else {
        outFile = "out.res";
    }

    // Dump the register content to file
    // simulate.writeToFile(outFile);

    // std::cout << '\n' << "Register content dumped in file: " << outFile << '\n';

    return 0;
}
