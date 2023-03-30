/*
 * @file main.cpp
 * @author Eduardo Pagotto
 * RV32I simulator program
 */

#include "include/Bus.hpp"
#include "include/Memory.hpp"
#include "include/RV32ISim.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {

    // Check for input
    if (argc < 2) {
        std::cout << "Filepath to binary input file required" << '\n';
        return 0;
    }

    // Get filepath for input file
    const char* filepath = argv[1];

    Bus bus;                                                             //= new Bus();
    uint32_t idRom = bus.add(new Memory(0x0000, 0x1000, DSTAT_ENABLED)); // ROM
    uint32_t idRam = bus.add(new Memory(0x1000, 0x1000, DSTAT_ENABLED | DSTAT_READWRITE));

    bus.loadFile(filepath, idRom);

    // Construct simulator object
    RV32ISim simulate;

    // bool read = simulate.readFromFile(filepath); // Read the input file
    // if (!read) {
    //     std::cout << "File not found :(" << '\n';
    //     return 0;
    // }

    // Run the program
    while (simulate.hasNext()) {
        simulate.step();
    }

    // Perhaps get name of output file
    const char* outFile;
    if (argc != 2) {
        outFile = argv[2];
    } else {
        outFile = "out.res";
    }

    // Dump the register content to file
    simulate.writeToFile(outFile);

    std::cout << '\n' << "Register content dumped in file: " << outFile << '\n';

    return 0;
}
