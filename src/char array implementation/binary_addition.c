#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "binary_addition.h"

int free_longbinary(LongBinary* long_binary) {
    if (long_binary == NULL) {
        fprintf(stderr, "The input for freeing longbinary is null\n");
        return 1;
    }
    LongBinaryChunk* chunk = long_binary->first_chunk;
    while (chunk != NULL) {
        LongBinaryChunk* next_chunk = chunk->next;
        free(chunk);
        chunk = next_chunk;
    }
    free(long_binary);
    return 0;
}

// Since allocated space of char* is limited by max value of size_t (as any allocated object in c),
// the only theoretical way to fully store strings of arbitrary size is using FILE*.
LongBinary* create_longbinary(FILE* input) {
    if (!input) {
        fprintf(stderr, "Input for creating long binary is null\n");
        return NULL;
    };
    LongBinary* long_binary = (LongBinary*) malloc(sizeof(LongBinary));
    if (!long_binary) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    LongBinaryChunk* previous_chunk = NULL;
    while (1) {
        LongBinaryChunk* chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
        if (chunk == NULL) {
            if (previous_chunk) previous_chunk->next = NULL;
            free_longbinary(long_binary);
            return NULL;
        }
        chunk->previous = previous_chunk;
        chunk->length = fread(chunk->data, 1, CHUNK_SIZE, input);
        for (int i = 0; i < chunk->length; i++) {
            if (chunk->data[i] != '0' && chunk->data[i] != '1') {
                fprintf(stderr, "Input doesn't consist of only ones and zeroes\n");
                if (previous_chunk) previous_chunk->next = NULL;
                free_longbinary(long_binary);
                free(chunk);
                return NULL;
            }
        }
        if (chunk->length == 0) {
            free(chunk);
            if (previous_chunk == NULL) {
                fprintf(stderr, "File is empty\n");
                free(long_binary);
                return NULL;
            }
            break;
        }
        if (previous_chunk != NULL) {
            previous_chunk->next = chunk;
        } else {
            long_binary->first_chunk = chunk;
        }
        previous_chunk = chunk;
        chunk->data[chunk->length] = '\0';
        if (chunk->length < CHUNK_SIZE) {
            break;
        }
    }
    if (previous_chunk) {
        long_binary->last_chunk = previous_chunk;
    }
    if (previous_chunk) previous_chunk->next = NULL;
    return long_binary;
}

// Strongly advised to use this function only when total length is much less than SIZE_MAX (at least SIZE_MAX/2),
// since there are problems with such a big allocation. Be ready to have malloc refusing to work.
char* long_binary_to_string(LongBinary* long_binary) {
    if (long_binary == NULL) return NULL;
    size_t length = 0; // assuming that size_t is enough to represent length (char* size is limited by size_t by being a single allocatable object)
    LongBinaryChunk* current_chunk = long_binary->first_chunk;
    while (current_chunk != NULL) {
        length += current_chunk->length;
        current_chunk = current_chunk->next;
    }
    char* binary_string = (char*) malloc(length + 1);
    if (binary_string == NULL) return NULL;
    binary_string[0] = '\0';
    current_chunk = long_binary->first_chunk;
    while (current_chunk != NULL) {
        strncat(binary_string, current_chunk->data, current_chunk->length);
        current_chunk = current_chunk->next;
    }
    char* trimmed_string = binary_string;
    while (*trimmed_string == '0' && *(trimmed_string + 1) != '\0') {
        trimmed_string++;
    }
    if (*trimmed_string == '\0' || (trimmed_string[0] == '0' && trimmed_string[1] == '\0')) {
        free(binary_string);
        return strdup("0");
    }
    char* result = strdup(trimmed_string);
    free(binary_string);
    return result;
}

LongBinary* binary_addition(LongBinary* long_binary1, LongBinary* long_binary2) {
    LongBinary* result = (LongBinary*) malloc(sizeof(LongBinary));
    if (result == NULL) return NULL;
    result->first_chunk = NULL;
    result->last_chunk = NULL;
    LongBinaryChunk* chunk1 = long_binary1->last_chunk;
    LongBinaryChunk* chunk2 = long_binary2->last_chunk;
    int len1 = chunk1->length;
    int len2 = chunk2->length;
    short carry = 0;
    LongBinaryChunk* chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
    if (chunk == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(result);
        return NULL;
    }
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
        int sum = (chunk1 ? chunk1->data[len1-1] - '0' : 0) + (chunk2 ? chunk2->data[len2-1] - '0' : 0) + carry;
        chunk->data[chunk_len-1] = sum % 2 + '0';
        carry = sum / 2;
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
                chunk->data[0] = '1';
                break;
            }
            if (carry || chunk1 || chunk2) {
                current_len = 0;
                chunk->length = last_size;
                last_size = CHUNK_SIZE;
                chunk->data[chunk->length] = '\0';
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
    chunk->data[last_size] = '\0';
    result->first_chunk->previous = NULL;
    result->last_chunk->next = NULL;
    return result;
}

int print_longbinary(LongBinary* long_binary) {
    if (!long_binary) {
        printf("The long binary to be printed is null\n");
        return 1;
    }
    LongBinaryChunk* current_chunk = long_binary->first_chunk;
    while (current_chunk != NULL) {
        printf("%s", current_chunk->data);
        current_chunk = current_chunk->next;
    }
    return 0;
}