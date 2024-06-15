cmake_minimum_required(VERSION 3.28)

PROJECT(kernel)

enable_language(ASM)
enable_language(C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)     # Force the usage of _ISOC11_SOURCE

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(GENERATED_FILES src/kernel.c 
                    src/entry.s)

set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${GENERATED_FILES})

#set (SfsInclude ${CMAKE_SOURCE_DIR}/include) # Raiz do projeto
#target_include_directories (${EXECUTABLE} PUBLIC ${SfsInclude})

target_compile_options(${EXECUTABLE} PRIVATE    
                        --target=riscv64
                        -march=rv64i
                        -nostdlib
                        -mabi=lp64d
                        -static
                        -Wall
                        -Wextra
                        -mcmodel=medany
                        -c
)

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/src/linker.ld
        --target=riscv64
        -march=rv64i
        -nostdlib
        -mabi=lp64d
        -L.
        -Wl
        #-Map=${PROJECT_NAME}.map
        # --cref
        #--gc-sections
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

# clean
add_custom_target (clean-all 
        COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
        COMMAND rm -rf ${CMAKE_SOURCE_DIR}/bin/*)