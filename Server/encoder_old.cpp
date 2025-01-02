// encoder.cpp

#include "encoder.h"
#include "server.h"
#include "common.h"

#include <cmath>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>
#include "stopwatch.h"

// Constants
#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

#define NUM_ELEMENTS 16384
#define BLOCKSIZE_ENC 8192//2048
#define HEADER 2 // Assuming HEADER is defined as 2; adjust as needed

// Global Variables
int offset = 0;
unsigned char* file_buffer;

// Helper Functions for Byte Order Handling

/**
 * @brief Appends a uint32_t to a byte buffer in Little Endian order.
 * 
 * @param buffer The byte buffer to append to.
 * @param value The uint32_t value to append.
 */
void append_uint32_le(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

/**
 * @brief Appends a uint16_t to a byte buffer in Little Endian order.
 * 
 * @param buffer The byte buffer to append to.
 * @param value The uint16_t value to append.
 */
void append_uint16_le(std::vector<uint8_t>& buffer, uint16_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
}

/**
 * @brief Saves binary data to a file.
 * 
 * @param data Pointer to the data buffer.
 * @param data_size Size of the data in bytes.
 * @param output_filename Name of the output file.
 */
void save_to_text_file(const unsigned char* data, size_t data_size, const std::string& output_filename) {
    // Open the output file in binary mode
    std::ofstream output_file(output_filename, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Could not open file " << output_filename << " for writing." << std::endl;
        return;
    }

    // Write the data to the file
    output_file.write(reinterpret_cast<const char*>(data), data_size);
    output_file.close();

    if (output_file) {
        std::cout << "Successfully wrote " << data_size << " bytes to " << output_filename << std::endl;
    } else {
        std::cerr << "Error: Failed to write data to " << output_filename << std::endl;
    }
}

/**
 * @brief Handles command-line input to set the block size.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @param blocksize Pointer to the block size variable.
 */
void handle_input(int argc, char* argv[], int* blocksize) {
    int option;
    while ((option = getopt(argc, argv, ":b:")) != -1) {
        switch (option) {
            case 'b':
                *blocksize = atoi(optarg);
                printf("Blocksize is set to %d\n", *blocksize);
                break;
            case ':':
                fprintf(stderr, "Option -%c requires a parameter\n", optopt);
                break;
            default:
                fprintf(stderr, "Usage: %s [-b blocksize]\n", argv[0]);
        }
    }
}

/**
 * @brief Processes the received file buffer by performing Content-Defined Chunking (CDC),
 *        deduplication, and LZW compression.
 * 
 * @param file The file buffer containing the received data.
 * @param offset The current offset in the file buffer.
 * @param lzw_cache A map for caching LZW compressed data based on hash values.
 * @param hash_table A set for tracking unique chunk hashes for deduplication.
 * @return true if processing is successful, false otherwise.
 */
bool process_packet(unsigned char* file, int& offset, 
                    std::unordered_map<std::string, std::vector<unsigned char>>& lzw_cache,//std::vector<int>>& lzw_cache,
                    std::unordered_set<std::string>& hash_table) {
    // Initial size before processing
    size_t original_size = offset;
    
    // Step 1: Perform Content-Defined Chunking
    std::vector<size_t> chunk_positions;
    content_defined_chunking(file, offset, chunk_positions);
    size_t chunk_count = (chunk_positions.empty()) ? 0 : chunk_positions.size() - 1;
    std::cout << "CDC function called: chunk_count = " << chunk_count << std::endl;
    
    size_t cdc_size = offset; // Size remains the same as CDC just marks chunk positions

    // Step 2: SHA-256 -> Deduplication or LZW for each chunk
    for (size_t i = 0; i < chunk_count; i++) {
        uint32_t start = chunk_positions[i];
        uint32_t end = chunk_positions[i + 1];
        uint32_t chunk_size = end - start;
        unsigned char* chunk = file + start;

        // Compute SHA-256 hash of the chunk
        std::string hash_value = sha256_hash_string(chunk, chunk_size);

        if (dedup(hash_table, hash_value)) {
            dedup_before += chunk_size;
            // Hash found in table, reuse cached LZW data
            const std::vector<unsigned char>& cached_data = lzw_cache[hash_value];
            //std::cout << "Hash found in lzw_cache with size" << lzw_cache[hash_value].size() << "\n";
            uint32_t cached_size = cached_data.size();
            //std::cout << "Duplicate chunk found. Reusing cached compressed data, size = " << cached_data.size() << "bytes \n";
            dedup_size += cached_size;  // Add size of reused chunk to dedup size
            // Check total byte size of cached_data
            size_t cached_byte_size = cached_data.size() * sizeof(unsigned char);
            //std::cout << "Cached data byte size: " << cached_byte_size << " bytes\n";
            // Ensure buffer does not overflow
            if (offset + cached_size < 70000000) {
                memcpy(&file[offset], cached_data.data(), cached_size);
                offset += cached_size;
            } else {
                std::cerr << "File buffer overflow!" << std::endl;
                return false;
            }
        } else {
            // New chunk, perform LZW compression
            uint32_t lzw_codes[16384]; // Adjust size as needed
            uint32_t code_length = 0;
            uint8_t failure = 0;
            unsigned int associative_mem = 0;
            lzw_before += chunk_size;
            // New chunk, perform LZW compression
            lzw(chunk, 0, chunk_size, lzw_codes, &code_length, &failure, &associative_mem);

            std::vector<uint8_t> compressed_data;
            for (size_t j = 0; j < code_length; ++j) {
                uint16_t code = static_cast<uint16_t>(lzw_codes[j]);
                compressed_data.push_back((code >> 8) & 0xFF);
                compressed_data.push_back(code & 0xFF);
            }
            uint32_t compressed_size = compressed_data.size();  
            std::cout << "Compressed chunk with size = " << compressed_data.size() << " bytes \n";
            lzw_compressed_size += compressed_size;  // Track the LZW compressed size   
            lzw_cache[hash_value] = compressed_data;
        }
    }

    std::cout << "Compression ratio (compared to orginal file size): " << (lzw_before / (static_cast<float>(original_size))) << std::endl;

    return true;
}

int main(int argc, char* argv[]) {
    if (argc <= 2) {
        std::cerr << "Using " << argv[1] << " as output file name." << std::endl;
    }
    const char* output_filename = argv[1]; // Take the output file name from the command line

    stopwatch ethernet_timer;
    unsigned char* input_buffers[NUM_PACKETS];
    int writer = 0;
    int done = 0;
    int length = 0;
    int count = 0; // If 'count' is unused, consider removing or using it
    int blocksize = BLOCKSIZE_ENC; // Fixed macro name
    ESE532_Server server;

    // Handle command-line input
    handle_input(argc, argv, &blocksize);

    // Allocate memory for the file buffer
    file_buffer = (unsigned char*) malloc(sizeof(unsigned char) * 70000000);
    if (file_buffer == NULL) {
        std::cerr << "Failed to allocate memory for file buffer.\n";
        return 1;
    }

    // Allocate memory for input buffers
    for (int i = 0; i < NUM_PACKETS; i++) {
        input_buffers[i] = (unsigned char*) malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
        if (input_buffers[i] == NULL) {
            std::cerr << "Failed to allocate memory for input buffer " << i << std::endl;
            // Free previously allocated buffers
            for (int j = 0; j < i; j++) {
                free(input_buffers[j]);
            }
            free(file_buffer);
            return 1;
        }
    }

    // Setup the server
    server.setup_server(blocksize);
	writer = pipe_depth;
	server.get_packet(input_buffers[writer]);
	count++;

	// get packet
	unsigned char* buffer = input_buffers[writer];

	// decode
	done = buffer[1] & DONE_BIT_L;
	length = buffer[0] | (buffer[1] << 8);
	length &= ~DONE_BIT_H;
	// printing takes time so be weary of transfer rate
	//printf("length: %d offset %d\n",length,offset);

	// we are just memcpy'ing here, but you should call your
	// top function here.
	memcpy(&file_buffer[offset], &buffer[HEADER], length);

	offset += length;
	writer++;

    // Receive premaining ackets
    while (!done) {
        if (writer == NUM_PACKETS) {
            writer = 0;
        }

        ethernet_timer.start();
        server.get_packet(input_buffers[writer]);
        ethernet_timer.stop();

        unsigned char* buffer = input_buffers[writer];

        // Decode packet
        done = buffer[1] & DONE_BIT_L;
        length = buffer[0] | (buffer[1] << 8);
        length &= ~DONE_BIT_H;

        //printf("Packet %d: Length=%d, Writer Index=%d, Offset=%d\n", count + 1, length, writer, offset);

        // Ensure buffer does not overflow
        if (offset + length >= 70000000) {
            std::cerr << "File buffer overflow! Skipping packet." << std::endl;
            writer++;
            count++;
            continue;
        }

        // Copy received data into 'file_buffer'
        memcpy(&file_buffer[offset], &buffer[HEADER], length);
        offset += length;
        count++;
        writer++;
    }
    // Optionally save the raw received data to a text file
    save_to_text_file(file_buffer, offset, "received_output.txt");

    // Initialize caches for deduplication and compression
    std::unordered_map<std::string, std::vector<unsigned char>> lzw_cache;
    std::unordered_set<std::string> hash_table;

    // Process the received file buffer
    if (!process_packet(file_buffer, offset, lzw_cache, hash_table)) {
        std::cerr << "Error processing packet!" << std::endl;
    }

    // Save the processed data to a binary file
    FILE *outfd = fopen(output_filename, "wb");
    if (outfd == NULL) {
        std::cerr << "Error: Could not open" << output_filename << "for writing." << std::endl;
        // Free allocated resources before exiting
        for (int i = 0; i < NUM_PACKETS; i++) {
            free(input_buffers[i]);
        }
        free(file_buffer);
        return 1;
    }

    size_t bytes_written = fwrite(file_buffer, 1, offset, outfd);
    if (bytes_written != offset) {
        std::cerr << "Warning: Not all data was written to the file. Expected " 
                  << offset << " bytes, but wrote " << bytes_written << " bytes." << std::endl;
    } else {
        std::cout << "All data written successfully." << std::endl;
    }
    printf("Written file with %zu bytes\n", bytes_written);
    fclose(outfd);

    // Optionally save the raw received data to a text file
    // save_to_text_file(file_buffer, offset, "received_output.txt");

    // Free allocated memory
    for (int i = 0; i < NUM_PACKETS; i++) {
        free(input_buffers[i]);
    }
    free(file_buffer);

    // Calculate and display throughput
    std::cout << "--------------- Key Throughputs ---------------" << std::endl;
    float ethernet_latency = ethernet_timer.latency() / 1000.0f;
    float input_throughput = (bytes_written * 8.0f / 1000000.0f) / ethernet_latency; // Mb/s
    std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
              << " (Latency: " << ethernet_latency << "s)." << std::endl;

    std::cout << "-------------------------------------------------" << std::endl;

    return 0;
}
