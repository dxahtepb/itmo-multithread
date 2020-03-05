project(itmo-multithread)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SOURCES main.cpp util.h matrix.h matrix_multiplication.h)

add_executable(lab1 ${SOURCES})