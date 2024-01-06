
## deps 
```bash
sudo apt install qemu-system-misc
```

## Compiling
```bash
clang --target=riscv64 -march=rv64i -nostdlib  -static -Wall -Wextra -c -mcmodel=medany -c -o kernel.o kernel.c
#riscv64-elf-gcc -Wall -Wextra -c -mcmodel=medany .c -o kernel.o -ffreestanding

clang --target=riscv64 -march=rv64i -nostdlib  -static -c -o entry.o entry.s
# riscv64-elf-as -c entry.S -o entry.o

clang --target=riscv64 -march=rv64i -nostdlib  -static -T linker.ld -nostdlib kernel.o entry.o -o kernel.elf
#riscv64-elf-ld -T linker.ld -lgcc -nostdlib kernel.o entry.o -o kernel.elf
```

## Runing
```bash
qemu-system-riscv64 -machine virt -bios none -kernel kernel.elf -serial mon:stdio
```

refs:
- https://wiki.osdev.org/RISC-V_Bare_Bones
- https://llvm.org/docs/RISCVUsage.html
- https://github.com/riscv-collab/riscv-gnu-toolchain/tree/master