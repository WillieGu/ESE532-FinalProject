// #ifndef CDC_H
// #define CDC_H

// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include "common.h"

// // Constants for CDC
// // #define WIN_SIZE 4
// // #define PRIME 3
// // #define MODULUS 7
// // #define TARGET 3

// // Function declaration for the chunking process
// uint64_t hash_func(unsigned char* input, unsigned int pos);
// void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions);

// #endif
#ifndef CDC_H
#define CDC_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "common.h"

// Function declaration for the initialization and chunking process
void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions);

#endif

