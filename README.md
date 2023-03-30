# 32 bit RISCV simulator
Simulador simples de riscv conjunto rv32i

Original em https://github.com/schoeberl/cae-lab.
Diretorio "test" baixados de la.

## Compiling the programs
```bash
cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Debug -D CMAKE_CXX_COMPILER="/bin/clang++" -D CMAKE_C_COMPILER="/bin/clang"
# cmake -S . -B build/ -D CMAKE_BUILD_TYPE=Release -D CMAKE_CXX_COMPILER="/bin/clang++" -D CMAKE_C_COMPILER="/bin/clang"
cd ./build
make
```

## Entrada
Binary deve ser compilado como RISCV 32 ISA code.
```bash
./sim tests/addlarge.bin
```