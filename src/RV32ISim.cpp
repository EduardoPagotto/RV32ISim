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

    fetch = new Fetch(&csr, bus, 0x0);
    decode = new Decode(&csr, fetch);
    execute = new Execute(&csr, bus, decode, regs);
    memory = new MemoryAccess(&csr, bus, execute);
    writeBack = new WriteBack(&csr, bus, memory, regs);
    trap = new Trap(&csr);
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

        writeBack->step();
        writeBack->commit();

        csr.step();
        csr.commit();

        trap->step();
        trap->commit();
    }
}
