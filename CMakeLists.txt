cmake_minimum_required(VERSION 3.18)

project(IR VERSION 0.1)

add_subdirectory(${PROJECT_SOURCE_DIR}/src/common)
list(APPEND EXTRA_LIBS common)
list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/src/common)


add_executable(ir_cli src/cli/main.cpp)

configure_file(ir.h.in ir.h)

target_link_libraries(ir_cli PUBLIC ${EXTRA_LIBS})
target_include_directories(ir_cli PUBLIC "${PROJECT_BINARY_DIR}" ${EXTRA_INCLUDES})

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_COMPLIER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")