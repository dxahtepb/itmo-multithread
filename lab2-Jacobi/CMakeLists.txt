project(itmo-multithread-lab-2)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES cpp/main.cpp cpp/command_line.h)

add_executable(lab2 ${SOURCES})