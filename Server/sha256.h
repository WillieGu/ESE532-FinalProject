// #ifndef SHA256_H
// #define SHA256_H

// #include <cstddef>
// #include <cstdint>
// #include <vector>
// #include <arm_neon.h>
// #include <cstring>
// #include <thread>
// #include <sstream>

// // Type Definitions
// typedef unsigned char BYTE;    // 8-bit byte
// typedef uint32_t WORD;   // 32-bit word

// #define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32-byte digest

// // Adjust based on the number of available cores
// #define NUM_THREADS 4

// // SHA256 Context Structure
// typedef struct {
//     uint32x4_t state[2]; // state[0]: a, b, c, d; state[1]: e, f, g, h
// } SHA256_CTX_NEON;

// // Function Declarations

// /**
//  * @brief Transforms the SHA256 context using NEON intrinsics.
//  *
//  * @param ctx Pointer to the SHA256 context.
//  * @param data Input data block (64 bytes).
//  */
// void sha256_transform_neon(SHA256_CTX_NEON *ctx, const BYTE data[]);

// /**
//  * @brief Initializes the SHA256 context.
//  *
//  * @param ctx Pointer to the SHA256 context.
//  */
// void sha256_init_neon(SHA256_CTX_NEON *ctx);

// /**
//  * @brief Computes the SHA256 hash for a single data chunk using NEON optimization.
//  *
//  * @param data The input data.
//  * @param len The length of the input data.
//  * @param hash_value The output hash (32 bytes).
//  */
// void sha256_hash_chunk_neon(const BYTE* data, size_t len, BYTE* hash_value);

// /**
//  * @brief Computes the SHA256 hash of the data using multiple threads and tree hashing.
//  *
//  * @param data The input data.
//  * @param total_len The total length of the input data.
//  * @param hash_value The output hash (32 bytes).
//  */
// void sha256_compute_parallel(const BYTE* data, size_t total_len, BYTE* hash_value);

// // helper function
// std::string bytes_to_hex_string(const BYTE* bytes, size_t length);


// // SHA-256 context structure
// typedef struct {
//     BYTE data[64];
//     WORD datalen;
//     uint64_t bitlen;
//     WORD state[8];
// } SHA256_CTX;

// // Function prototypes

// void sha256_init(SHA256_CTX *ctx);
// void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
// void sha256_final(SHA256_CTX *ctx, BYTE hash[]);

// // Helper functions
// void sha256_hash(const BYTE data[], size_t length, BYTE hash_output[]);
// void sha256_hash_range(const BYTE data[], uint32_t start_idx, uint32_t end_idx, BYTE hash_output[]);
// std::string sha256_hash_string(const BYTE data[], size_t length);
// #endif // SHA256_H

#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE;             // 8-bit byte
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	BYTE data[64];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);
void sha256_hash(SHA256_CTX* ctx, const BYTE data[], BYTE hash[], size_t runLen);
void sha256_hash_file(SHA256_CTX* ctx, const BYTE data[], BYTE hash[], size_t dataLen);

#endif