#ifndef SHA256_HW_H
#define SHA256_HW_H

#include <cstdint>
#include <cstddef>
#include <string>

typedef uint8_t BYTE;  // 8-bit byte
typedef uint32_t WORD; // 32-bit word

#define SHA256_BLOCK_SIZE 32 // SHA256 outputs a 32-byte hash

// Context structure for SHA-256
struct SHA256_CTX_HW {
    BYTE data[64];
    WORD datalen;
    uint64_t bitlen;
    WORD state[8];
};

// Function prototypes
void sha256_transform_hw(SHA256_CTX_HW *ctx, const BYTE data[64]);
void sha256_init_hw(SHA256_CTX_HW *ctx);
void sha256_update_hw(SHA256_CTX_HW *ctx, const BYTE data[], size_t len);
void sha256_final_hw(SHA256_CTX_HW *ctx, BYTE hash[SHA256_BLOCK_SIZE]);

// Helper functions
void sha256_hash_hw(const BYTE data[], size_t length, BYTE hash_output[]);
void sha256_hash_range_hw(const BYTE data[], uint32_t start_idx, uint32_t end_idx, BYTE hash_output[]);
std::string sha256_hash_string_hw(const BYTE data[], size_t length);
std::string bytes_to_hex_string_hw(const BYTE* bytes, size_t length);

#endif // SHA256_HW_H
