cmake_minimum_required(VERSION 3.16.3)
set(CMAKE_TOOLCHAIN_FILE ./cmake/riscv.toolchain.cmake) 


PROJECT(hello)

#find_package(toolchain_arm REQUIRED)

message(STATUS "Toolchain file in: ${CMAKE_TOOLCHAIN_FILE}.")
enable_language(ASM)
#set(CMAKE_C_STANDARD 99)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

set(GENERATED_FILES teste.s)
set(EXECUTABLE ${PROJECT_NAME}.elf)

add_executable(${EXECUTABLE} ${GENERATED_FILES})

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/teste.ld
        #-mcpu=arm926ej-s
        # -g
        # -mthumb
        # #-mfpu=fpv4-sp-d16
        # -mfloat-abi=hard
        # -specs=nano.specs
        # -lc
        # -lm
        # -lnosys
        # #-Wl,-Map=${PROJECT_NAME}.map,--cref
        # -Wl,--gc-sections
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE} 
        POST_BUILD
        COMMAND ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size ${EXECUTABLE})

# Create aux files
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objdump -D ${EXECUTABLE} > ${PROJECT_NAME}.list
        COMMAND ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
        COMMAND ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objdump -d ${EXECUTABLE} -l > dissasembler.s
)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
