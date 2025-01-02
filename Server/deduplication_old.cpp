// deduplication.cpp

#include "common.h"
#include <cstdlib>
#include <iostream>
#include <unordered_map>

using namespace std;

// Return -1 if the chunk is new; otherwise, return the existing chunk ID.
int64_t dedup(const std::string& sha_fingerprint, int64_t& chunk_id) {
    static unordered_map<string, int64_t> sha_chunk_id_map;

    auto it = sha_chunk_id_map.find(sha_fingerprint);

    if (it == sha_chunk_id_map.end()) {
        // New chunk
        sha_chunk_id_map[sha_fingerprint] = chunk_id;
        ++chunk_id;
        return -1;
    } else {
        // Duplicate chunk
        return it->second;
    }
}
