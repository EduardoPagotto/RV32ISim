cmake_minimum_required(VERSION 3.27.7)

# # gcc 
# set(RISCV_TOOL_PATH "/opt/riscv" CACHE PATH "RISC-V tool path") # usando clang default da maquina
# set(RISCV_TOOL_PATH_BIN_DIR "${RISCV_TOOL_PATH}/bin")
# set(RISCV_TOOLCHAIN_PREFIX "riscv32-unknown-linux-gnu-")
# set(RISCV_TOOLCHAIN_ROOT "${RISCV_TOOL_PATH}" CACHE PATH "RISC-V compiler path") # default da maquina

# clang / llvm
set(RISCV_TOOL_PATH "/usr" CACHE PATH "RISC-V tool path") # usando clang default da maquina
set(RISCV_TOOL_PATH_BIN_DIR "${RISCV_TOOL_PATH}/bin")
set(RISCV_TOOLCHAIN_PREFIX "llvm")
set(RISCV_TOOLCHAIN_ROOT "${RISCV_TOOL_PATH}" CACHE PATH "RISC-V compiler path") # default da maquina

#message(STATUS "Toolchain file in: ${RISCV_TOOLCHAIN_ROOT}.")

set(CMAKE_C_COMPILER "${RISCV_TOOLCHAIN_ROOT}/bin/clang")
set(CMAKE_C_COMPILER_ABI "ilp32")

set(CMAKE_CXX_COMPILER "${RISCV_TOOLCHAIN_ROOT}/bin/clang++")
set(CMAKE_AR "${RISCV_TOOLCHAIN_ROOT}/bin/llvm-ar")
set(CMAKE_RANLIB "${RISCV_TOOLCHAIN_ROOT}/bin/llvm-ranlib")
set(CMAKE_STRIP "${RISCV_TOOLCHAIN_ROOT}/bin/llvm-strip")

set(RISCV_SIZE "${RISCV_TOOL_PATH_BIN_DIR}/${RISCV_TOOLCHAIN_PREFIX}-size")
set(RISCV_OBJDUMP "${RISCV_TOOL_PATH_BIN_DIR}/${RISCV_TOOLCHAIN_PREFIX}-objdump")
set(RISCV_OBJCOPY "${RISCV_TOOL_PATH_BIN_DIR}/${RISCV_TOOLCHAIN_PREFIX}-objcopy")
