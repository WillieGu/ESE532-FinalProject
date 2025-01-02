// common.h

#ifndef COMMON_H
#define COMMON_H

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "server.h"
#include "stopwatch.h"

#include "sha256.h"
#include "lzw.h"
#include "deduplication.h"
#include "cdc.h"

#include <bitset>  // Include bitset for binary printing
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstring> 

// Constants
#define MAX_CHUNK_SIZE 1024 * 5//8192
#define MIN_CHUNK_SIZE 0
#define TARGET_CHUNK_SIZE 4096
#define CHUNK_SIZE 4096
#define PIPELINE_BUFFER_LEN 16384
#define MAX_OUTPUT_BUF_SIZE 40960
#define MAX_LZW_CHUNKS 20
#define WIN_SIZE 12
// #define CODE_LENGTH 12
#define MODULUS 64
#define MODULUS_MASK (MODULUS - 1)
#define TARGET 3
#define PRIME 3

// Macro for checking memory allocation
#define CHECK_MALLOC(ptr, msg) \
    if (ptr == NULL) { \
        printf("%s\n", msg); \
        exit(EXIT_FAILURE); \
    }

// Function prototypes

// CDC (Content-Defined Chunking)
void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions);

// Deduplication function prototype
int64_t dedup(const std::string& sha_fingerprint, std::unordered_map<std::string, int64_t>& chunk_hash_map, int64_t& lzw_chunk_index);

// LZW Compression
// Function prototype for LZW compression
void lzw_encode(const unsigned char* input_data, size_t input_size, std::vector<uint8_t>& output_data);

#endif // COMMON_H

// common.h

// #ifndef COMMON_H
// #define COMMON_H


// #include <errno.h>
// #include <fcntl.h>
// #include <pthread.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/mman.h>
// #include <unistd.h>
// #include "server.h"
// #include "stopwatch.h"

// #include <bitset>  // Include bitset for binary printing
// #include <fstream>
// #include <iostream>
// #include <string>
// #include <unordered_map>
// #include <unordered_set>
// #include <vector>
// #include <sstream>
// #include <iomanip>
// #include <cstring> 

// #include "sha256.h"
// #include "lzw.h"
// #include "deduplication.h"

// // Constants
// #define MAX_CHUNK_SIZE 8192
// #define MIN_CHUNK_SIZE 1024
// #define TARGET_CHUNK_SIZE 4096
// #define CHUNK_SIZE 4096
// #define PIPELINE_BUFFER_LEN 16384
// #define MAX_OUTPUT_BUF_SIZE 40960
// #define MAX_LZW_CHUNKS 20
// #define WIN_SIZE 12
// #define CODE_LENGTH 12
// #define MODULUS 17
// #define MODULUS_MASK (MODULUS - 1)
// #define TARGET 3
// #define PRIME 3

// // Macro for checking memory allocation
// #define CHECK_MALLOC(ptr, msg) \
//     if (ptr == NULL) { \
//         printf("%s\n", msg); \
//         exit(EXIT_FAILURE); \
//     }

// // Function prototypes

// // CDC (Content-Defined Chunking)
// void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions);

// // Deduplication function prototype
// int64_t dedup(const std::string& sha_fingerprint, std::unordered_map<std::string, int64_t>& chunk_hash_map, int64_t& lzw_chunk_index);

// // LZW Compression
// // Function prototype for LZW compression
// #define MAX_OUTPUT_SIZE 40960   // Define maximum output size as per your requirements

// #endif // COMMON_H