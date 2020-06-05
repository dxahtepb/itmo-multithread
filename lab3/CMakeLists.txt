project(lab3)
cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})
set(SOURCES cpp/main.cpp cpp/command_line.h)

add_executable(lab3 ${SOURCES})
target_link_libraries(lab3 ${MPI_LIBRARIES})

if(MPI_COMPILE_FLAGS)
    set_target_properties(lab3 PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(lab3 PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()