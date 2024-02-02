#!/bin/bash
mkdir -p ./build

# Compile
clang --target=riscv32 -march=rv32ia -nostdlib -mabi=ilp32 -static -c -o ./build/app01.o app01.s

# Link
clang --target=riscv32 -march=rv32ia -nostdlib -mabi=ilp32 -L. -Wl,-T,mem_cfg.ld -o ./build/app01 ./build/app01.o 

## Utils from ELF
llvm-objdump -D ./build/app01  > ./build/app01_dump.s # dissasembler

#llvm-objcopy -O ihex ./build/app01  ./build/app01.hex # hex file
llvm-objcopy -O binary ./build/app01  ./build/app01.bin # binary final !!!!!

llvm-objdump -t -r ./build/app01  # show type exec