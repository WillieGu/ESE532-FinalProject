#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstring> // For memset

#include "stopwatch.h"

#include "lzw.h"     // Include the header file for the LZW software version
#include "lzw_hw.h"  // Include the header file for the hardware-optimized version

// Function to create dynamic data buffer
unsigned char* Create_data(size_t size) {
    unsigned char* data = static_cast<unsigned char*>(malloc(size * sizeof(unsigned char)));
    if (data == nullptr) {
        std::cerr << "Could not allocate data buffer." << std::endl;
        exit(EXIT_FAILURE);
    }
    return data;
}

// Function to free dynamic data buffer
void Destroy_data(unsigned char* data) { free(data); }

// Function to initialize data with a repeating sequence (0-255)
void Initialize_data(unsigned char* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<unsigned char>(i % 256); // Repeating sequence
    }
}

// Function to compare two data buffers
bool Compare_data(const uint8_t* data1, size_t size1, const uint8_t* data2, size_t size2) {
    bool equal = true;
    if (size1 != size2) {
        std::cout << "Size mismatch: " << size1 << " != " << size2 << std::endl;
        equal = false;
    }

    for (size_t i = 0; i < size1; ++i) {
        if (data1[i] != data2[i]) {
            std::cout << "Mismatch at index " << i << ": " << static_cast<int>(data1[i])
                      << " != " << static_cast<int>(data2[i]) << std::endl;
            equal = false;
        }
    }
    return equal;
}

int main() {
    // Define test parameters
    const size_t input_size = 1024; 
    const size_t output_size = OUTPUT_SIZE; // Ensure OUTPUT_SIZE is defined appropriately

    // Allocate and initialize input data
    unsigned char* input_data = Create_data(input_size);
    Initialize_data(input_data, input_size);

    // Allocate output buffers and initialize to zero
    uint8_t compressed_data_sw[OUTPUT_SIZE];
    uint32_t compressed_size_sw = 0;
    memset(compressed_data_sw, 0, sizeof(compressed_data_sw));

    uint8_t compressed_data_hw[OUTPUT_SIZE];
    uint32_t compressed_size_hw = 0;
    memset(compressed_data_hw, 0, sizeof(compressed_data_hw));

    // Initialize stopwatch instances
    stopwatch time_lzw_sw, time_lzw_hw;

    // Execute software LZW compression
    time_lzw_sw.start();
    lzw_encode(input_data, static_cast<uint16_t>(input_size), compressed_data_sw, &compressed_size_sw);
    time_lzw_sw.stop();

    // Execute hardware-optimized LZW compression
    time_lzw_hw.start();
    lzw_encode_hw(input_data, static_cast<uint16_t>(input_size), compressed_data_hw, &compressed_size_hw);
    time_lzw_hw.stop();

    // Compare compressed outputs
    bool Equal = Compare_data(compressed_data_sw, compressed_size_sw, compressed_data_hw, compressed_size_hw);

    // Clean up allocated memory
    Destroy_data(input_data);

    // Report test results
    std::cout << "TEST " << (Equal ? "PASSED" : "FAILED") << std::endl;
    std::cout << "Total latency of lzw_encode (software) is: " << time_lzw_sw.latency() << " ms." << std::endl;
    std::cout << "Total latency of lzw_encode_hw (hardware) is: " << time_lzw_hw.latency() << " ms." << std::endl;

    return Equal ? 0 : 1;
}
