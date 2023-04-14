/**
 * @file RV32ISim.cpp
 * @author  Eduardo Pagotto
 * @brief  Implementation for the RV32ISim class
 * @version 0.1
 * @date 2023-03-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "include/RV32ISim.h"
#include <fstream>
#include <iomanip>
#include <iostream>

RV32ISim::RV32ISim(Bus* bus) {

    this->bus = bus;

    // Init registers
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }

    fetch = new Fetch(&crt, bus, 0x0);
    decode = new Decode(&crt, fetch);
    execute = new Execute(&crt, bus, decode, regs);
    memory = new MemoryAccess(&crt, bus, execute, &csr);
}

RV32ISim::~RV32ISim() {}

void RV32ISim::play() {

    while (fetch->hasNext()) {

        fetch->step();
        fetch->commit();

        decode->step();
        decode->commit();

        execute->step();
        execute->commit();

        memory->step();
        memory->commit();

        csr.step();
    }
}

void RV32ISim::printRegisters() {
    std::cout << "Value of registers: \n" << std::endl;
    for (int i = 0; i < 32; i++) {
        std::cout << "x" << i << " " << regs[i] << std::endl;
    }
}
