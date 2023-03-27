cmake_minimum_required(VERSION 3.18)
project(IR)
add_executable(ir main.cpp lirc_payload.cpp lirc_serializer.cpp lirc_payload.h lirc_serializer.h)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_COMPLIER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")