#ifndef C_BINARY_ADDITION_BINARY_ADDITION_H
#define C_BINARY_ADDITION_BINARY_ADDITION_H

#define CHUNK_SIZE (1024) // Can be tweaked

// Linked list node for storing chunks of binary data
typedef struct LongBinaryChunk {
    short length; // Length of the data in this chunk
    char data[CHUNK_SIZE]; // Pointer to the chunk's data
    struct LongBinaryChunk* next; // Pointer to the next chunk
    struct LongBinaryChunk* previous; // Pointer to the previous chunk
} LongBinaryChunk;

// Linked list structure for storing a long binary number
typedef struct LongBinary {
    LongBinaryChunk* first_chunk; // Pointer to the first chunk
    LongBinaryChunk* last_chunk; // Pointer to the last chunk
} LongBinary;

// Function to create a LongBinary from a file containing binary data
LongBinary* create_longbinary(FILE* input);

// Function to convert a LongBinary to a null-terminated string
char* long_binary_to_string(LongBinary* long_binary);

// Function to add two LongBinary numbers and return the resulting LongBinary number
LongBinary* binary_addition(LongBinary* long_binary1, LongBinary* long_binary2);

// Function to print a LongBinary
int print_longbinary(LongBinary* long_binary);

// Function to free the memory allocated for a LongBinary
int free_longbinary(LongBinary* long_binary);

#endif //C_BINARY_ADDITION_BINARY_ADDITION_H
