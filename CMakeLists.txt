cmake_minimum_required(VERSION 3.24)
project(ossim_student)

set(CMAKE_CXX_STANDARD 11)

add_executable(ossim_student main.cpp disk.cpp OS.cpp PCB.cpp)
