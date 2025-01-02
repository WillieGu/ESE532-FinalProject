// // deduplication.cpp

// #include "common.h"
// #include "deduplication.h"


// /**
//  * @brief Deduplicates chunks based on their SHA-256 hash.
//  * 
//  * @param sha_fingerprint The SHA-256 hash of the chunk as a hexadecimal string.
//  * @param chunk_hash_map Reference to the map maintaining hash to chunk index.
//  * @param lzw_chunk_index Reference to the current LZW chunk index.
//  * @return int64_t Returns the index of the duplicate chunk if found; otherwise, returns -1.
//  */
// int64_t dedup(const std::string& sha_fingerprint,
//               std::unordered_map<std::string, int64_t>& chunk_hash_map,
//               int64_t& lzw_chunk_index) {
//     auto it = chunk_hash_map.find(sha_fingerprint);

//     if (it == chunk_hash_map.end()) {
//         // New chunk
//         chunk_hash_map[sha_fingerprint] = lzw_chunk_index;
//         ++lzw_chunk_index;
//         return -1;
//     } else {
//         // Duplicate chunk
//         return it->second;
//     }
// }

// deduplication.cpp

// #include "deduplication.h"

// int64_t dedup(const SHA256Hash& sha_fingerprint,
//               std::unordered_map<SHA256Hash, int64_t, SHA256HashHash>& chunk_hash_map,
//               int64_t& lzw_chunk_index) {
//     auto it = chunk_hash_map.find(sha_fingerprint);

//     if (it == chunk_hash_map.end()) {
//         // New chunk
//         chunk_hash_map.emplace(sha_fingerprint, lzw_chunk_index);
//         ++lzw_chunk_index;
//         return -1;
//     } else {
//         // Duplicate chunk
//         return it->second;
//     }
// }

// deduplication.cpp

// deduplication.cpp

// deduplication.cpp

#include "deduplication.h"

int64_t dedup(const SHA256Hash& sha_fingerprint,
              std::unordered_map<SHA256Hash, int64_t, SHA256HashHash>& chunk_hash_map,
              int64_t& lzw_chunk_index) {
    auto it = chunk_hash_map.find(sha_fingerprint);

    if (it == chunk_hash_map.end()) {
        // New chunk
        chunk_hash_map.emplace(sha_fingerprint, lzw_chunk_index);
        ++lzw_chunk_index;
        return -1;
    } else {
        // Duplicate chunk
        return it->second;
    }
}




