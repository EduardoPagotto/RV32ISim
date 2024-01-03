
## Compile llvm
```bash
# Compile
clang --target=riscv32 -march=rv32i -nostdlib -mabi=ilp32 -static -c -o app01.o app01.s

# Link
clang --target=riscv32 -march=rv32i -nostdlib -mabi=ilp32 -L. -Wl,-T,mem_cfg.ld -o app01.o app01.o 

## Utils from ELF
llvm-objdump -D app01.o  > app01_dump.s # dissasembler
llvm-objdump -t -r app01.o  # show type exec
llvm-objcopy -O ihex app01.o  app01.hex # hex file
llvm-objcopy -O binary app01.o  app01.bin # binary final !!!!!

```