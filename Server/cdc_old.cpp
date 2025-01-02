#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "cdc.h"

// Precomputed powers of 3 (up to WIN_SIZE+...)
int expo_table[] = {1, 3, 9, 27, 81, 243, 729, 2187, 19683, 59049, 177147, 531441};
uint64_t last_hash = 0;

// Rolling hash function
uint64_t hash_func(unsigned char* input, unsigned int pos) {
    uint64_t hash = 0;

    if (last_hash == 0) {
        // Initial hash calculation for the first window
        for (int i = 0; i < WIN_SIZE; i++) {
            hash += static_cast<uint64_t>(input[pos + WIN_SIZE - 1 - i]) * expo_table[i + 1];
        }
    } else {
        // Rolling hash for subsequent positions
        hash = last_hash * PRIME 
               - static_cast<uint64_t>(input[pos - 1]) * expo_table[WIN_SIZE + 1] 
               + static_cast<uint64_t>(input[pos + WIN_SIZE - 1]) * PRIME;
    }

    last_hash = hash;
    return hash;
}

// CDC function to find chunk positions
void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions) {
    chunk_positions.push_back(0);  // Start of the first chunk

    for (unsigned int i = WIN_SIZE; i < buff_size - WIN_SIZE; i++) {
        uint64_t hash = hash_func(buffer, i);

        if ((hash % MODULUS) == TARGET) {
            chunk_positions.push_back(i + 1);  // End of the current chunk, start of the next
        }
    }

    chunk_positions.push_back(buff_size);  // End of the last chunk
    last_hash = 0;  // Reset hash for next call
}

// Main function
// int main() {
//     // Open the file in binary mode
//     std::ifstream file("LittlePrince.txt", std::ios::binary);
//     if (!file) {
//         std::cerr << "Failed to open LittlePrince.txt" << std::endl;
//         return 1;
//     }

//     // Read the file contents into a buffer
//     std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//     file.close();

//     // Check if buffer is non-empty
//     if (buffer.empty()) {
//         std::cerr << "File is empty." << std::endl;
//         return 1;
//     }

//     // Perform Content-Defined Chunking (CDC)
//     std::vector<size_t> chunk_positions;
//     content_defined_chunking(buffer.data(), buffer.size(), chunk_positions);

//     // Process and print each chunk
//     std::vector<uint8_t> output_data; // Buffer to store output data
//     for (size_t i = 0; i < chunk_positions.size() - 1; ++i) {
//         size_t start = chunk_positions[i];
//         size_t end = chunk_positions[i + 1];
//         size_t chunk_size = end - start;

//         // Get the chunk data
//         unsigned char* chunk_data = buffer.data() + start;

//         // Append the chunk data to output_data for potential deduplication/compression
//         output_data.insert(output_data.end(), chunk_data, chunk_data + chunk_size);

//         // Print chunk information and content
//         std::cout << "Chunk " << i << ": Start=" << start << ", End=" << end
//                 << ", Size=" << chunk_size << " bytes, Data=\""
//                 << std::string(chunk_data, chunk_data + chunk_size) << "\"" << std::endl;
//     }

//     return 0;
// }

// int main() {
//     // Open the file in binary mode
//     std::ifstream file("../LittlePrince.txt", std::ios::binary);
//     if (!file) {
//         std::cerr << "Failed to open LittlePrince.txt" << std::endl;
//         return 1;
//     }

//     // Read the file contents into a buffer
//     std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//     file.close();

//     // Check if buffer is non-empty
//     if (buffer.empty()) {
//         std::cerr << "File is empty." << std::endl;
//         return 1;
//     }

//     // Prepare a vector to hold chunk positions
//     std::vector<size_t> chunk_positions;

//     // Perform content-defined chunking
//     content_defined_chunking(buffer.data(), buffer.size(), chunk_positions);

//     // Output the chunk positions
//     std::cout << "Chunk positions:" << std::endl;
//     for (size_t pos : chunk_positions) {
//         std::cout << pos << " ";
//     }
//     std::cout << std::endl;

//     return 0;
// }