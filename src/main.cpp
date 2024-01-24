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

void getVal(const std::tuple<int32_t, uint32_t> v) {
    std::cout << "Code: " << std::get<0>(v) << " Val: " << std::get<1>(v) << std::endl;
}

int main(int argc, char** argv) {

    Debug::init();

    // Check for input
    if (argc < 2) {
        std::cout << "Filepath to binary input file required" << '\n';
        return 0;
    }

    const char* filepath = argv[1];

    Bus bus;
    MMU mmu;

    getVal(mmu.createEntry(0x00000, 0,
                           MMU_ACC_EXECUTE | MMU_ACC_READ | MMU_ACC_WRITE | MMU_ACC_SUPER)); // FIXME: corrigir!!!
    // getVal(mmu.createEntry(0x00000, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER)); // FIXME: corrigir!!!
    // getVal(mmu.createEntry(0x40000, 0, MMU_ACC_READ | MMU_ACC_WRITE));
    // getVal(mmu.createEntry(0x80000, 0, MMU_ACC_READ | MMU_ACC_WRITE | MMU_ACC_SUPER));
    // getVal(mmu.createEntry(0xC0000, 0, MMU_ACC_EXECUTE));
    // getVal(mmu.createEntry(0xF0000, 0, MMU_ACC_EXECUTE | MMU_ACC_READ | MMU_ACC_WRITE | MMU_ACC_SUPER));

    // // ---
    // getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_EXECUTE));
    // getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0x0002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));

    // getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_READ));
    // getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_WRITE));
    // getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_EXECUTE));
    // getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0x4002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));

    // getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_READ));
    // getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_WRITE));
    // getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_READ | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_WRITE | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0x8002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));

    // getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_EXECUTE));
    // getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_READ));
    // getVal(mmu.getPhysicalAddress(0xC002, 0, MMU_ACC_WRITE));

    // getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_EXECUTE | MMU_ACC_SUPER));
    // getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_EXECUTE));
    // getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_READ));
    // getVal(mmu.getPhysicalAddress(0xF000, 0, MMU_ACC_WRITE));

    Memory rom(0x000, 0x200, DEV_OPENED);
    Memory ram(0x200, 0x200, DEV_OPENED | DEV_RW);

    bus.add(&rom);
    bus.add(&ram);

    loadFile(filepath, rom.getBank());

    Riscv32 processor(0);
    processor.reset();

    try {
        while (true) {
            if (!processor.step(bus, mmu))
                break;
        }
    } catch (std::string e) { std::cout << '\n' << "Erro: " << e << '\n'; }

    return 0;
}
