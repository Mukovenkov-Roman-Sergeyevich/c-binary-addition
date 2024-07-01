cmake_minimum_required(VERSION 3.25)
project(c_binary_addition C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(c_binary_addition
        src/binary_addition.c
        src/binary_addition.h)
