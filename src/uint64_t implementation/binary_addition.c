#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "binary_addition.h"

int free_longbinary(LongBinary* long_binary) {
    if (long_binary == NULL) {
        fprintf(stderr, "No input for freeing longbinary\n");
        return 1;
    };
    LongBinaryChunk* chunk = long_binary->first_chunk;
    while (chunk != NULL) {
        LongBinaryChunk* next_chunk = chunk->next;
        free(chunk);
        chunk = next_chunk;
    }
    free(long_binary);
    return 0;
}

uint64_t convert_to_uint64_in_create_longbinary(char* str, int length) {
    uint64_t result = 0;
    for (int i = 0; i < length; i++) {
        result = (result << 1) | (str[i] - '0');
    }
    return result;
}

// Since allocated space of char* is limited by max value of size_t (as any allocated object in c),
// the only theoretical way to fully store strings of arbitrary size is using FILE*.
LongBinary* create_longbinary(FILE* input) {
    if (!input) {
        fprintf(stderr, "No input for creating longbinary\n");
        return NULL;
    }
    // The problem was that the file was being read as 111 111 1, which was interpreted as 111 111 001
    // Now that we reverse the file it is being read as 1 111 111 (001 111 111) - leading zeroes don't matter
    // The first one is ambiguous, e.g. 111 111 01 would mean the same thing. You now see how the second variant's better.
    // Am not willing to introduce serious crutches just for theoretical arbitrary size, but it could be possible and ugly.
    // One possible solution would be to keep track of the length of the last element, but...
    // It'd be really unmaintainable. So I'm cheating and introducing a variable to reverse the file input
    // Potentially one could write a function to join a file and a long binary together in one long binary type.
    // For arbitrary size in this moment you could use my char* variant, which is not the pinnacle of efficiency, but still something.
    fseeko(input, 0, SEEK_END);
    off_t file_size = ftello(input);
    if (file_size == 0) {
        fprintf(stderr, "File is empty\n");
        return NULL;
    }
    long read_size = file_size % POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW;
    if (!read_size) {
        read_size = POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW;
    }
    fseeko(input, 0, SEEK_SET);
    LongBinary* long_binary = (LongBinary*) malloc(sizeof(LongBinary));
    if (!long_binary) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    LongBinaryChunk* previous_chunk = NULL;
    int length_of_element = 0;
    while (1) {
        LongBinaryChunk* chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
        if (!chunk) {
            fprintf(stderr, "Memory allocation failed\n");
            free_longbinary(long_binary);
            return NULL;
        }
        long_binary->last_chunk = chunk;
        chunk->length = 0;
        chunk->previous = previous_chunk;
        for (int j = 0; j < CHUNK_SIZE; j++) {
            char chunk_data_str[POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW];
            length_of_element = fread(chunk_data_str, 1, read_size, input);
            if (length_of_element == 0) {
                break;
            }
            read_size = POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW;
            for (int i = 0; i < length_of_element; i++) {
                if (chunk_data_str[i] != '0' && chunk_data_str[i] != '1') {
                    fprintf(stderr, "Input doesn't consist of only ones and zeroes\n");
                    free_longbinary(long_binary);
                    free(chunk);
                    return NULL;
                }
            }
            chunk->data[j] = convert_to_uint64_in_create_longbinary(chunk_data_str, length_of_element);
            chunk->length++;
        }
        if (chunk->length == 0) {
            long_binary->last_chunk = previous_chunk;
            long_binary->last_chunk->next = NULL;
            free(chunk);
            break;
        }
        if (previous_chunk != NULL) {
            previous_chunk->next = chunk;
        } else {
            long_binary->first_chunk = chunk;
        }
        previous_chunk = chunk;
    }
    if (previous_chunk && previous_chunk->length != 0) {
        long_binary->last_chunk = previous_chunk;
        long_binary->last_chunk->next = NULL;
    } else {
        free(previous_chunk);
    }
    return long_binary;
}

LongBinary* binary_addition(LongBinary* long_binary1, LongBinary* long_binary2) {
    LongBinary* result = (LongBinary*) malloc(sizeof(LongBinary));
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    result->first_chunk = NULL;
    result->last_chunk = NULL;
    LongBinaryChunk* chunk1 = long_binary1->last_chunk;
    LongBinaryChunk* chunk2 = long_binary2->last_chunk;
    int len1 = chunk1->length;
    int len2 = chunk2->length;
    uint64_t carry = 0;
    LongBinaryChunk* chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
    if (chunk == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(result);
        return NULL;
    }
    chunk->length = CHUNK_SIZE;
    result->first_chunk = chunk;
    result->last_chunk = chunk;
    int current_len = 0;
    int chunk_len = CHUNK_SIZE;
    int last_size = CHUNK_SIZE;
    while (carry || chunk1 || chunk2) {
        if (chunk1 && chunk1->previous == NULL && chunk2 && chunk2->previous == NULL && (chunk_len > len1 && chunk_len > len2)) {
            chunk_len = (len1 < len2 ? len2 : len1);
            last_size = chunk_len;
            chunk->length = last_size;
        } else if (chunk1 && chunk1->previous == NULL && !chunk2 && chunk_len > len1) {
            chunk_len = len1;
            last_size = chunk_len;
            chunk->length = last_size;
        } else if (!chunk1 && chunk2 && chunk2->previous == NULL && chunk_len > len2) {
            chunk_len = len2;
            last_size = chunk_len;
            chunk->length = last_size;
        } else if (!chunk1 && !chunk2) {
            chunk_len = ++current_len;
            last_size = chunk_len;
            chunk->length = last_size;
        }
        // Avoiding overflowing (maximum value possible - 2^63-1 + 2^63 - 1 + 2^63 - 2, exceeds 2^64-1)
        uint64_t carry_temp = carry;
        uint64_t sum = ((chunk1) ? chunk1->data[len1-1] : 0) +  ((chunk2) ? chunk2->data[len2-1] : 0);
        carry = sum / POWER_OF_TWO_EVALUATED;
        sum %= POWER_OF_TWO_EVALUATED;
        sum += carry_temp;
        carry += sum / POWER_OF_TWO_EVALUATED;
        sum %= POWER_OF_TWO_EVALUATED;
        chunk->data[chunk_len-1] = sum;
        chunk_len--;
        current_len++;
        if (chunk1) len1--;
        if (chunk2) len2--;
        if (len1 == 0) {
            chunk1 = chunk1 ? chunk1->previous : NULL;
            len1 = chunk1 ? chunk1->length : -1;
        }
        if (len2 == 0) {
            chunk2 = chunk2 ? chunk2->previous : NULL;
            len2 = chunk2 ? chunk2->length : -1;
        }
        if (chunk_len == 0) {
            if (carry && !chunk1 && !chunk2 && last_size < CHUNK_SIZE) {
                chunk->length = ++last_size;
                for (int i = last_size-1; i > 0; i--) {
                    chunk->data[i] = chunk->data[i-1];
                }
                chunk->data[0] = carry;
                break;
            }
            if (carry || chunk1 || chunk2) {
                current_len = 0;
                chunk->length = last_size;
                last_size = CHUNK_SIZE;
                chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
                if (chunk == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    free_longbinary(result);
                    return NULL;
                }
                chunk->next = result->first_chunk;
                chunk->length = CHUNK_SIZE;
                result->first_chunk->previous = chunk;
                result->first_chunk = chunk;
                chunk_len = CHUNK_SIZE;
            }
        }
    }
    result->first_chunk->previous = NULL;
    result->last_chunk->next = NULL;
    return result;
}

int print_longbinary(LongBinary* long_binary) {
    if (!long_binary) {
        fprintf(stderr, "The long binary to be printed is null\n");
        return 1;
    }
    LongBinaryChunk* chunk = long_binary->first_chunk;
    short met_one = 0; // Skip leading zeroes

    while (chunk) {
        for (int j = 0; j < chunk->length; j++) {
            for (int i = POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW-1; i >= 0; i--) {
                uint64_t mask = 1ull << i;
                if (chunk->data[j] & mask) {
                    printf("1");
                    met_one = 1;
                } else if (met_one) {
                    printf("0");
                }
            }
        }
        chunk = chunk->next;
    }
    if (!met_one) {
        printf("0");
    }
    return 0;
}

char* long_binary_to_string(LongBinary* long_binary) {
    if (!long_binary) {
        fprintf(stderr, "Input for long_binary to string is null\n");
        return NULL;
    }
    size_t total_length = 0;
    LongBinaryChunk* chunk = long_binary->first_chunk;
    while (chunk) {
        total_length += chunk->length * 64;
        chunk = chunk->next;
    }
    char* binary_string = (char*)malloc((total_length + 1) * sizeof(char));
    if (!binary_string) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    int index = 0;
    int met_one = 0; // skip leading zeroes

    chunk = long_binary->first_chunk;
    while (chunk) {
        for (int j = 0; j < chunk->length; j++) {
            for (int i = POWER_OF_TWO_WHICH_ALLOWS_ADDITION_WITHOUT_OVERFLOW-1; i >= 0; i--) {
                uint64_t mask = 1ull << i;
                if (chunk->data[j] & mask) {
                    binary_string[index++] = '1';
                    met_one = 1;
                } else if (met_one) {
                    binary_string[index++] = '0';
                }
            }
        }
        chunk = chunk->next;
    }

    binary_string[index] = '\0';

    if (!met_one) {
        free(binary_string);
        binary_string = (char*)malloc(2 * sizeof(char));
        if (binary_string == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }
        binary_string[0] = '0';
        binary_string[1] = '\0';
    }

    return binary_string;
}
