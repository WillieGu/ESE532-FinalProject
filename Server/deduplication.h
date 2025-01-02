// deduplication.h

#ifndef DEDUPLICATION_H
#define DEDUPLICATION_H

#include <array>
#include <cstdint>
#include <unordered_map>

// Define the SHA256Hash type
using SHA256Hash = std::array<uint8_t, 32>;

// Custom hash function for SHA256Hash
struct SHA256HashHash {
    std::size_t operator()(const SHA256Hash& hash) const {
        std::size_t seed = 0;
        for (const auto& byte : hash) {
            seed = seed * 31 + byte;
        }
        return seed;
    }
};

// Deduplication function declaration
int64_t dedup(const SHA256Hash& sha_fingerprint,
              std::unordered_map<SHA256Hash, int64_t, SHA256HashHash>& chunk_hash_map,
              int64_t& lzw_chunk_index);

#endif // DEDUPLICATION_H




// #ifndef DEDUPLICATION_H
// #define DEDUPLICATION_H

// #include <string>
// #include <cstdint>
// #include <unordered_set>

// /**
//  * @brief Performs deduplication by checking if the given SHA-256 fingerprint already exists.
//  *
//  * This function maintains a static unordered map to track SHA-256 fingerprints and their associated chunk IDs.
//  * If the fingerprint is new, it stores it with a new chunk ID and returns -1.
//  * If the fingerprint already exists, it returns the existing chunk ID.
//  *
//  * @param sha_fingerprint The SHA-256 fingerprint of the chunk as a string.
//  * @param chunk_id Reference to the chunk ID counter, which is incremented when a new chunk is added.
//  * @return int64_t Returns -1 if the chunk is new; otherwise, returns the existing chunk ID.
//  */

// int64_t dedup(const std::string& sha_fingerprint,
//               std::unordered_map<std::string, int64_t>& chunk_hash_map,
//               int64_t& lzw_chunk_index);
// #endif
// deduplication.h

// #ifndef DEDUPLICATION_H
// #define DEDUPLICATION_H

// #include <array>
// #include <cstdint>
// #include <unordered_map>

// // Define the SHA256Hash type
// using SHA256Hash = std::array<uint8_t, 32>;

// // Custom hash function for SHA256Hash
// struct SHA256HashHash {
//     std::size_t operator()(const SHA256Hash& hash) const {
//         std::size_t seed = 0;
//         for (const auto& byte : hash) {
//             seed = seed * 31 + byte;
//         }
//         return seed;
//     }
// };

// // Deduplication function declaration
// int64_t dedup(const SHA256Hash& sha_fingerprint,
//               std::unordered_map<SHA256Hash, int64_t, SHA256HashHash>& chunk_hash_map,
//               int64_t& lzw_chunk_index);

// #endif // DEDUPLICATION_H
