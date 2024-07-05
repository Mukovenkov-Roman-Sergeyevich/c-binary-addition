cmake_minimum_required(VERSION 3.25)
project(c_binary_addition C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(c_binary_addition_char_array
        "src/main.c"
        "src/char array implementation/binary_addition.c"
        "src/char array implementation/binary_addition.h"
)

add_executable(c_binary_addition_uint64_t
        "src/main.c"
        "src/uint64_t implementation/binary_addition.c"
        "src/uint64_t implementation/binary_addition.h"
)

add_executable(test_binary_addition_uint64_t
        "tests/test_binary_addition.c"
        "src/uint64_t implementation/binary_addition.c"
        "src/uint64_t implementation/binary_addition.h"
)

add_executable(test_binary_addition_char_array
        "tests/test_binary_addition.c"
        "src/char array implementation/binary_addition.c"
        "src/char array implementation/binary_addition.h"
)

target_compile_definitions(test_binary_addition_uint64_t PRIVATE USE_UINT64_T_IMPLEMENTATION)
target_compile_definitions(c_binary_addition_uint64_t PRIVATE USE_UINT64_T_IMPLEMENTATION)

enable_testing()

add_test(NAME uint64_t_TestOfAddition COMMAND test_binary_addition_uint64_t)
add_test(NAME char*_TestOfAddition COMMAND test_binary_addition_char_array)
