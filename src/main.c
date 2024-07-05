#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// !!! Comment/uncomment this to switch realization !!!
// The default state for CMakeLists.txt is to be uncommented, so please revert it back to uncommented.
// #define USE_UINT64_T_IMPLEMENTATION

#ifdef USE_UINT64_T_IMPLEMENTATION
#include "uint64_t implementation/binary_addition.h"
#else
#include "char array implementation/binary_addition.h"
#endif

int main() {
    FILE* file1 = fopen("../strings/input1.txt", "r");
    FILE* file2 = fopen("../strings/input2.txt", "r");
    LongBinary* long_binary1 = create_longbinary(file1);
    LongBinary* long_binary2 = create_longbinary(file2);
    fclose(file1);
    fclose(file2);
    LongBinary* result;
    if (long_binary1 != NULL && long_binary2 != NULL) {
        result = binary_addition(long_binary1, long_binary2);
    } else {
        return 1;
    }
    char* a = long_binary_to_string(result);
    printf("HEY!! the string ---> %s\n", a);
    free(a);
    print_longbinary(long_binary1);
    printf(" + ");
    print_longbinary(long_binary2);
    printf(" = ");
    print_longbinary(result);
    printf("\n");
    LongBinary* result2;
    if (long_binary1 != NULL && result != NULL) {
        result2 = binary_addition(long_binary1, result);
    } else {
        return 2;
    }
    print_longbinary(long_binary1);
    printf(" + ");
    print_longbinary(result);
    printf(" = ");
    print_longbinary(result2);
    printf("\n");
    free_longbinary(long_binary1);
    free_longbinary(long_binary2);
    free_longbinary(result);
    if (result2) free_longbinary(result2);
    return 0;
}