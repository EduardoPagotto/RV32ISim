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
}

RV32ISim::~RV32ISim() {}

void RV32ISim::step() {
    fetch->step();
    decode->step();
    execute->step();
    memory->step();
    writeBack->step();

    csr.step();
}

void RV32ISim::commit() {
    fetch->commit();
    decode->commit();
    execute->commit();
    memory->commit();
    writeBack->commit();

    csr.commit();
}

void RV32ISim::play() {

    while (true) {
        this->step();
        this->commit();
    }
}
