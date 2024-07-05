#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef USE_UINT64_T_IMPLEMENTATION
#include "../src/uint64_t implementation/binary_addition.h"
#else
#include "../src/char array implementation/binary_addition.h"
#endif

int test_create_longbinary() {
    FILE* file = fopen("../tests/test_strings/test_input1.txt", "r");
    LongBinary *long_binary = create_longbinary(file);
    fclose(file);

    if (!long_binary) return 0;

    char* binary_string = long_binary_to_string(long_binary);
    if (!binary_string) return 0;

    int result = strcmp(binary_string, "111111111111111111111111111111111111111111111111111111111111111") == 0;

    free(binary_string);
    free_longbinary(long_binary);

    return result;
}

int test_binary_addition() {
    FILE* file1 = fopen("../tests/test_strings/test_input1.txt", "r");
    FILE* file2 = fopen("../tests/test_strings/test_input2.txt", "r");
    LongBinary *long_binary1 = create_longbinary(file1);
    LongBinary *long_binary2 = create_longbinary(file2);
    fclose(file1);
    fclose(file2);

    if (!long_binary1 || !long_binary2) return 0;

    LongBinary *result = binary_addition(long_binary1, long_binary2);
    if (!result) return 0;

    char* result_string = long_binary_to_string(result);
    if (!result_string) return 0;

    int test_result = strcmp(result_string, "1111111111111111111111111111111111111111111111111111111111111110") == 0;

    free(result_string);
    free_longbinary(long_binary1);
    free_longbinary(long_binary2);
    free_longbinary(result);

    return test_result;
}

int test_triple_binary_addition() {
    FILE* file1 = fopen("../tests/test_strings/test_input1.txt", "r");
    FILE* file2 = fopen("../tests/test_strings/test_input2.txt", "r");
    LongBinary *long_binary1 = create_longbinary(file1);
    LongBinary *long_binary2 = create_longbinary(file2);
    fclose(file1);
    fclose(file2);

    if (!long_binary1 || !long_binary2) return 0;

    LongBinary *result1 = binary_addition(long_binary1, long_binary2);
    if (!result1) return 0;

    LongBinary *result2 = binary_addition(result1, long_binary1);
    if (!result2) return 0;

    LongBinary *result3 = binary_addition(result2, result1);
    if (!result3) return 0;

    char *result_string = long_binary_to_string(result3);
    if (!result_string) return 0;

    int test_result = strcmp(result_string, "100111111111111111111111111111111111111111111111111111111111111011") == 0;

    free(result_string);
    free_longbinary(long_binary1);
    free_longbinary(long_binary2);
    free_longbinary(result1);
    free_longbinary(result2);
    free_longbinary(result3);

    return test_result;
}

int test_file_is_null() {
    LongBinary* long_binary = create_longbinary(NULL);
    return long_binary == NULL;
}

int test_string_zeroes_number() {
    FILE* file = fopen("../tests/test_strings/test_input3.txt", "r");
    LongBinary* long_binary = create_longbinary(file);
    fclose(file);
    if (!long_binary) return 0;
    char* result_string = long_binary_to_string(long_binary);
    if (!result_string) return 0;
    int test_result = strcmp(result_string, "0") == 0;
    free(result_string);
    free_longbinary(long_binary);
    return test_result;
}

int test_file_is_empty() {
    FILE* file = fopen("../tests/test_strings/test_input4.txt", "r");
    LongBinary* long_binary = create_longbinary(file);
    fclose(file);
    return !long_binary;
}

int test_large_binary_addition() {
    FILE* file1 = fopen("../tests/test_strings/test_large_input1.txt", "r");
    FILE* file2 = fopen("../tests/test_strings/test_large_input2.txt", "r");
    LongBinary *long_binary1 = create_longbinary(file1);
    LongBinary *long_binary2 = create_longbinary(file2);
    fclose(file1);
    fclose(file2);

    if (!long_binary1 || !long_binary2) return 0;

    LongBinary *result = binary_addition(long_binary1, long_binary2);
    if (!result) return 0;

    char* result_string = long_binary_to_string(result);
    if (!result_string) return 0;

    int test_result = strcmp(result_string, "1010000101100000110000111010101110101111111001010101111110111010001100010000111011011001001011011001000001100010110110011001011011010010110110011110111010011111100111111010000010011010000100101010011100011100011100100010101100100110100111101101000111011011011000010101000101010001101001") == 0;

    free(result_string);
    free_longbinary(long_binary1);
    free_longbinary(long_binary2);
    free_longbinary(result);

    return test_result;
}

int main() {
    int result = 1;

    result &= test_create_longbinary();
    result &= test_binary_addition();
    result &= test_triple_binary_addition();
    result &= test_string_zeroes_number();
    result &= test_file_is_empty();
    result &= test_large_binary_addition();

    if (result) {
        printf("All tests passed.\n");
    } else {
        printf("Some tests failed.\n");
    }

    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}
