cmake_minimum_required(VERSION 3.27.7)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/../toolchains/riscvclang.toolchain.cmake) 

PROJECT(simplles01)

enable_language(ASM)
enable_language(C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)     # Force the usage of _ISOC11_SOURCE

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(GENERATED_FILES app01.s)
set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${GENERATED_FILES})

#set (SfsInclude ${CMAKE_SOURCE_DIR}/include) # Raiz do projeto
#target_include_directories (${EXECUTABLE} PUBLIC ${SfsInclude})

target_compile_options(${EXECUTABLE} PRIVATE    
                        --target=riscv32
                        -march=rv32ia
                        -nostdlib
                        -mabi=ilp32
                        -static
                        -c
)

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/mem_cfg.ld
        --target=riscv32
        -march=rv32ia
        -nostdlib
        -mabi=ilp32
        -L.
        -Wl
        # #-Wl,-Map=${PROJECT_NAME}.map,--cref
        # -Wl,--gc-sections
)


# Create aux files
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${RISCV_SIZE} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE} # Print executable size
        COMMAND ${RISCV_OBJDUMP} -D ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE} > ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${PROJECT_NAME}.list
        COMMAND ${RISCV_OBJCOPY} -O ihex ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${PROJECT_NAME}.hex
        COMMAND ${RISCV_OBJCOPY} -O binary ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${PROJECT_NAME}.bin
        COMMAND ${RISCV_OBJDUMP} -d ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE} -l > ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/dissasembler.s
)
