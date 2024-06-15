cmake_minimum_required(VERSION 3.28)

PROJECT(RV32ISim)

#set(CMAKE_C_COMPILER "/usr/bin/clang")
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} -g -ftest-coverage -fprofile-arcs -fstandalone-debug")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(src)

# clean
add_custom_target (clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/*
                             COMMAND rm -rf ${CMAKE_BINARY_DIR}/.*
                             COMMAND rm -rf ${CMAKE_SOURCE_DIR}/bin/*)
