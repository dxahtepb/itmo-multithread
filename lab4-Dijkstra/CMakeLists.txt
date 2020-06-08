project(lab4)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCES cpp/main.cpp cpp/util.h cpp/command_line.h cpp/dijkstra.h cpp/my_defs.h)

add_executable(lab4 ${SOURCES})