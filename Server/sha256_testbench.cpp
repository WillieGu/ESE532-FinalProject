#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring> // For memset
#include "sha256_hw.h" // Include the hardware-optimized SHA-256 header
#include "sha256.h"
#include "stopwatch.h"

void Randomize_data(unsigned char* data, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        data[i] = rand() % 256; // Random values between 0-255
    }
}

bool Compare_hashes(const unsigned char* hash1, const unsigned char* hash2, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        if (hash1[i] != hash2[i])
        {
            std::cout << "Mismatch at byte " << i << ": " 
                      << static_cast<int>(hash1[i]) << " != " 
                      << static_cast<int>(hash2[i]) << std::endl;
            return false;
        }
    }
    return true;
}

int main()
{
    const size_t input_size = 8 * 1024; // Testing with a single 64-byte block for simplicity
    unsigned char input_data[input_size];
    unsigned char hash_output_sw[SHA256_BLOCK_SIZE];
    unsigned char hash_output_hw[SHA256_BLOCK_SIZE];

    // Initialize and randomize the input data
    Randomize_data(input_data, input_size);

    // Run software SHA-256
    stopwatch time_sha_sw;
    time_sha_sw.start();
    SHA256_CTX ctx_sw;
    sha256_init(&ctx_sw);
    sha256_update(&ctx_sw, input_data, input_size);
    sha256_final(&ctx_sw, hash_output_sw);
    time_sha_sw.stop();

    // Run hardware SHA-256
    stopwatch time_sha_hw;
    time_sha_hw.start();
    SHA256_CTX_HW ctx_hw;
    sha256_init_hw(&ctx_hw);
    sha256_update_hw(&ctx_hw, input_data, input_size);
    sha256_final_hw(&ctx_hw, hash_output_hw);
    time_sha_hw.stop();

    // Compare the results
    bool Equal = Compare_hashes(hash_output_sw, hash_output_hw, SHA256_BLOCK_SIZE);

    std::cout << "TEST " << (Equal ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Total latency of sha256 (software) is: " << time_sha_sw.latency() << " ms." << std::endl;
    std::cout << "Total latency of sha256_hw (hardware) is: " << time_sha_hw.latency() << " ms." << std::endl;

    return Equal ? 0 : 1;
}
