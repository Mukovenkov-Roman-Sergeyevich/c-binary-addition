# c-binary-addition
Two C functions with different implementations that take in two arbitrarily long strings of zeroes and ones and return their sum.

The main idea is a linked list of arrays. The first implementation uses char* for an array, whereas the second uint64_t*. 

You can specify the size of the arrays. If you use CHUNK_SIZE 1, they become arrays of one element, basically storing one element. Pretty much your standard linked list now without any arrays.

Inputting of strings is done via files. You can use some other functions to turn char* into FILE* or write your own function. 

You can also specify your power of two (uint64_t implementation) to your liking. Though 63 is the most efficient, since it still uses uint64_t regardless.

Testing and build provided by CMake build system.
