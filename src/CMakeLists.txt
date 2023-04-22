cmake_minimum_required(VERSION 3.18.2)

PROJECT(sim)

#define os Lib's a serem usados
set (LibsSim -lpthread)

#define os includes
set (IncludesSim ${CMAKE_SOURCE_DIR})

add_executable (sim RV32ISim.cpp
                    PrintAs.cpp
                    Decode.cpp
                    Execute.cpp
                    MemoryAccess.cpp
                    WriteBack.cpp
                    Bus.cpp
                    Device.cpp
                    main.cpp)

target_link_libraries(sim ${LibsSim})
target_include_directories (sim PRIVATE ${IncludesSim})

#INSTALL(FILES ${Sim_BINARY_DIR}/sim DESTINATION bin)