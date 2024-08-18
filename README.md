# 32 bit RISCV simulator
Simulador simples de riscv conjunto rv32i

Original em https://github.com/schoeberl/cae-lab.
Diretorio "test" baixados de la.

## Compiling the C Simulator programs
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

## Compile llvm
```bash
# CMake
sudo dnf install cmake cmake-data cmake-rpm-macros
# Install deps CLANG
sudo dnf install clang clang-tools-extra 
# Install deps os LLVM
#sudo apt install llvm lld 
sudo dnf install llvm lld libstdc++-static llvm-static llvm-devel llvm-test

$ llvm-objdump --version | grep riscv
riscv32    - 32-bit RISC-V
riscv64    - 64-bit RISC-V


# Just compile
clang --target=riscv32 -march=rv32i -nostdlib -mabi=ilp32 -static -c -o pgm.o teste.s

# Linker
clang --target=riscv32 -march=rv32i -nostdlib -mabi=ilp32 -L. -Wl,-T,teste.ld pgm.o  

# Compile and  Linker
clang --target=riscv32 -march=rv32i -nostdlib -mabi=ilp32 -static -L. -Wl,-T,teste.ld,-Map=pgm.map -o pgm.o teste.s

## Conver from ELF
llvm-objdump -D pgm.o > pgm.s # dissasembler
llvm-objdump -t -r pgm.o # show type exec
llvm-objcopy -O ihex pgm.o pgm.hex # hex file
llvm-objcopy -O binary pgm.o pgm.bin # binary final !!!!!

```
