#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "binary_addition.h"

#define CHUNK_SIZE (20480)

// Linked list implementation. Is not limited whatsoever.
typedef struct LongBinaryChunk {
    char* chunk;
    size_t length;
    struct LongBinaryChunk* next;
    struct LongBinaryChunk* previous;
} LongBinaryChunk;

typedef struct LongBinary {
    LongBinaryChunk* first_chunk;
    LongBinaryChunk* last_chunk;
} LongBinary;

LongBinary* create_longbinary(FILE* input) {
    if (input == NULL) return NULL;
    LongBinary* long_binary = (LongBinary*) malloc(sizeof(long_binary));
    if (long_binary == NULL) return NULL;
    long_binary->first_chunk = NULL;
    long_binary->last_chunk = NULL;

    LongBinaryChunk* previous_chunk = NULL;
    while (1) {
        LongBinaryChunk* chunk = (LongBinaryChunk*) malloc(sizeof(LongBinaryChunk));
        if (chunk == NULL) {
            free(long_binary);
            return NULL;
        }
        chunk->chunk = (char*) malloc(CHUNK_SIZE);
        if (chunk->chunk == NULL) return NULL;
        chunk->length = fread(chunk->chunk, 1, CHUNK_SIZE, input);
        if (chunk->length == 0) {
            free(chunk->chunk);
            free(chunk);
            break;
        }

        chunk->previous = previous_chunk;
        if (previous_chunk != NULL) {
            previous_chunk->next = chunk;
        } else {
            long_binary->first_chunk = chunk;
        }
        previous_chunk = chunk;
        if (chunk->length < CHUNK_SIZE) {
            break;
        }
    }
    long_binary->last_chunk = previous_chunk;
    previous_chunk->next = NULL;
    return long_binary;
}

void free_longbinary(LongBinary* long_binary) {
    if (long_binary == NULL) return;
    LongBinaryChunk* chunk = long_binary->first_chunk;
    while (chunk != NULL) {
        LongBinaryChunk* next_chunk = chunk->next;
        free(chunk->chunk);
        free(chunk);
        chunk = next_chunk;
    }
    free(long_binary);
}

int main() {
    LongBinary* long_binary = create_longbinary(stdin);
    if (long_binary != NULL) {
        free_longbinary(long_binary);
    }
    return 0;
}