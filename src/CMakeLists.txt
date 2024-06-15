PROJECT(sim)

#define os Lib's a serem usados
set (LibsSim -lpthread)

#set(EXECUTABLE ${PROJECT_NAME})

#define os includes
set (IncludesSim ${CMAKE_SOURCE_DIR})

add_executable (${PROJECT_NAME} Debug.cpp
                                main.cpp)

target_link_libraries(${PROJECT_NAME} ${LibsSim})
target_include_directories (${PROJECT_NAME} PRIVATE ${IncludesSim})

#INSTALL(FILES ${Sim_BINARY_DIR}/sim DESTINATION bin)