// encoder.cpp

#include "encoder.h"

#include "common.h"

// Constants
#define NUM_PACKETS 8
#define pipe_depth 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)

#define NUM_ELEMENTS 16384
#define BLOCKSIZE_ENC 8192
#define HEADER 2            // Assuming HEADER is defined as 2; adjust as needed

// Global Variables
int offset = 0;
unsigned char *file_buffer;
stopwatch cdc_time, sha_time, dedup_time, lzw_time;

// Initialize counters for compression calculation
size_t deduplication_contribution = 0;
size_t lzw_contribution = 0;
size_t total_compressed_size = 0;
size_t input_size_dedup = 0, input_size_lzw = 0, dedup_chunk = 0, lzw_chunk = 0;

// Helper Functions for Byte Order Handling

/**
 * @brief Appends a uint32_t to a byte buffer in Little Endian order.
 *
 * @param buffer The byte buffer to append to.
 * @param value The uint32_t value to append.
 */
void append_uint32_le(std::vector<uint8_t> &buffer, uint32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

/**
 * @brief Saves binary data to a file.
 *
 * @param data Pointer to the data buffer.
 * @param data_size Size of the data in bytes.
 * @param output_filename Name of the output file.
 */
void save_to_text_file(const unsigned char *data, size_t data_size, const std::string &output_filename) {
    // Open the output file in binary mode
    std::ofstream output_file(output_filename, std::ios::binary);
    if (!output_file) {
        std::cerr << "Error: Could not open file " << output_filename << " for writing." << std::endl;
        return;
    }

    // Write the data to the file
    output_file.write(reinterpret_cast<const char *>(data), data_size);
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
void handle_input(int argc, char *argv[], int *blocksize) {
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
 * @brief Processes the received file buffer by performing Content-Defined
 * Chunking (CDC), deduplication, and LZW compression.
 *
 * @param file The file buffer containing the received data.
 * @param file_size The size of the file buffer.
 * @param output_data The vector to store the final encoded output data.
 * @return true if processing is successful, false otherwise.
 */
bool process_packet(unsigned char *file, size_t file_size, std::vector<unsigned char> &output_data) {
    // Step 1: Perform Content-Defined Chunking
    std::vector<size_t> chunk_positions;
    cdc_time.start();
    content_defined_chunking(file, file_size, chunk_positions);
    cdc_time.stop();

    size_t chunk_count = (chunk_positions.empty()) ? 0 : chunk_positions.size() - 1;
    std::cout << "CDC function called: chunk_count = " << chunk_count << std::endl;

    // // print statements to check cdc function (CORRECT)

    // for (size_t i = 0; i < chunk_positions.size() - 1; ++i) {
    //     size_t start = chunk_positions[i];
    //     size_t end = chunk_positions[i + 1];
    //     size_t chunk_size = end - start;

    //     // Get the chunk data
    //     unsigned char* chunk_data = file + start;

    //     // Append the chunk data to output_data for potential
    //     deduplication/compression output_data.insert(output_data.end(),
    //     chunk_data, chunk_data + chunk_size);

    //     // Print chunk information and content
    //     std::cout << "Chunk " << i << ": Start=" << start << ", End=" << end
    //             << ", Size=" << chunk_size << " bytes, Data=\""
    //             << std::string(chunk_data, chunk_data + chunk_size) << "\"" <<
    //             std::endl;
    // }
    size_t original_size = file_size;
    // Step 2: Initialize deduplication structures
    // std::unordered_map<std::string, int64_t> chunk_hash_map;  // Map hash to LZW chunk index

    // Initialize deduplication structures with preallocated capacity
    size_t estimated_unique_chunks = 1000000; // Adjust based on expected data
    std::unordered_map<SHA256Hash, int64_t, SHA256HashHash> chunk_hash_map;
    chunk_hash_map.reserve(estimated_unique_chunks);

    int64_t lzw_chunk_index = 0;  // Starts from 0
    // uint32_t compressed_size_buf = 0;
    // lzw_time.start();
    // initialize_dictionary();
    // lzw_time.stop();

    // Step 3: Process each chunk
    for (size_t i = 0; i < chunk_count; i++) {
        size_t start = chunk_positions[i];
        size_t end = chunk_positions[i + 1];
        uint16_t chunk_size = end - start;
        unsigned char *chunk = file + start;

        if (chunk_size == 0 || chunk_size > MAX_CHUNK_SIZE) {
            std::cerr << "Error: Invalid chunk size " << chunk_size << " for chunk " << i << std::endl;
            continue;
        }
        // Compute SHA-256 hash of the chunk
        // sha_time.start();
        // std::string hash_value = sha256_hash_string(chunk, chunk_size);
        // sha_time.stop();

        //std::cout << "Hash size: " << SHA256_BLOCK_SIZE << " bytes\n";
        // std::cout << "Hash value (hex) of chunk " << i << " is: ";
        // for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        //     printf("%02x", hash_value[i]);
        // }
        // std::cout << "\n";
        // std::cout << std::endl;
        // // NEON SHA implementation
        // BYTE hash_output[SHA256_BLOCK_SIZE];
        // // Call NEON-based parallel SHA-256 implementation
        // sha_time.start();
        // sha256_compute_parallel(chunk, chunk_size, hash_output);
        // sha_time.stop();

        // NEWEST SHA VERSION
        BYTE hash_output[SHA256_BLOCK_SIZE];
        // Call NEON-based parallel SHA-256 implementation
        sha_time.start();
        // sha256_compute_parallel(chunk, chunk_size, hash_output);
        SHA256_CTX ctx;
        sha256_hash(&ctx, chunk, hash_output, chunk_size);
        sha_time.stop();

        // Convert hash_output to a hexadecimal string for use as a key in the deduplication map
        // std::stringstream ss;
        // for (int i = 0; i < 32; ++i) {
        //     ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash_output[i]);
        // }
        // std::string hash_value = ss.str();
        // // Convert hash output to a hexadecimal string
        //std::string hash_value = bytes_to_hex_string(hash_value, SHA256_BLOCK_SIZE);

        // std::cout << "NEON Hash value (hex): ";
        // for (size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        //     printf("%02x", hash_value[i]);
        // }
        // std::cout << std::endl;

        // Print chunk information (CORRECT)
        // std::cout << "Chunk " << i << " (Start: " << start << ", End: " << end <<
        // "):\n"; std::cout << "Chunk Data (Text): "; for (size_t j = 0; j <
        // chunk_size; ++j)
        // {
        //     std::cout << static_cast<char>(chunk[j]);
        // }
        // std::cout << "\nSHA-256 Hash: " << hash_value << "\n\n";
        // std::cout << "\nOld hash value: " << old_hash_value << "\n";

        // dedup_time.start();
        // int64_t duplicate_index = dedup(hash_value, chunk_hash_map, lzw_chunk_index);
        // dedup_time.stop();
        // Convert hash_output to binary SHA256Hash type

        // Convert hash_output to binary SHA256Hash type
        SHA256Hash hash_value;
        std::memcpy(hash_value.data(), hash_output, 32);

        dedup_time.start();
        int64_t duplicate_index = dedup(hash_value, chunk_hash_map, lzw_chunk_index);
        dedup_time.stop();

        // std::cout << "Dup_index is: " << duplicate_index << "\n";
        if (duplicate_index != -1) {
            // Duplicate chunk found
            //std::cout << "Duplicate chunk found with index " << duplicate_index << std::endl;
            // Create the 32-bit header for Duplicate Chunk
            uint32_t header = (duplicate_index << 1) | 0x1;  // Bit 0 is 1
            // Append header to output_data
            append_uint32_le(output_data, header);
            // std::cout << "Duplicate Index of chunk " << i << " is: " << duplicate_index << std::endl;
            // // Print header in binary format
            // std::cout << "Duplicate Header (Binary) of chunk " << i << " is: " << std::bitset<32>(header) << std::endl;
            // Add to deduplication contribution (only the header size)
            deduplication_contribution += 4;
            total_compressed_size += 4;  // Only the header for duplicate chunks
            input_size_dedup += chunk_size;
            dedup_chunk++;
        } else {
            // New chunk, perform LZW compression

            uint8_t compressed_data[OUTPUT_SIZE];
            uint32_t compressed_size_buf = 0;  // Initialize explicitly

            lzw_time.start();
            lzw_encode(chunk, chunk_size, compressed_data, &compressed_size_buf);
            lzw_time.stop();

            // Assign the dereferenced value to a new variable
            uint32_t compressed_size = compressed_size_buf;
            //std::cout << "Debug: Compressed size retrieved in encoder.cpp: " << compressed_size << std::endl;

            if (compressed_size == 0) {
                std::cerr << "Error: Compressed size is zero for chunk " << i << std::endl;
                // Print chunk information and content
                std::cout << "Chunk " << i << ": Start=" << start << ", End=" << end << ", Size=" << chunk_size
                          << " bytes, Data=\"" << std::string(chunk, chunk + chunk_size) << "\"" << std::endl;
            }
            // if (compressed_size == 0) {
            //     std::cerr << "Compression failed for chunk " << i << std::endl;
            //     //return false;
            // } else {
            //     std::cout << "Compressed size of chunk " << i << " is: " << compressed_size << std::endl;
            // }
            // // Print the compressed data
            // std::cout << "Compressed data for chunk " << i << ": ";
            // for (size_t j = 0; j < compressed_size; ++j) {
            //     std::cout << std::hex << static_cast<int>(compressed_data[j]) << " ";
            // }
            // std::cout << std::dec << std::endl;
            uint32_t header = (compressed_size << 1) | 0;
            // std::cout << "Compressed Header (Binary) of chunk" << i << " is: " << std::bitset<32>(header) << std::endl;

            append_uint32_le(output_data, header);
            output_data.insert(output_data.end(), compressed_data, compressed_data + compressed_size);

            lzw_contribution += 4 + compressed_size;
            total_compressed_size += 4 + compressed_size;
            input_size_lzw += chunk_size;
            lzw_chunk++;
        }
    }
    // Calculate compression ratio
    double compression_ratio = static_cast<double>(original_size) / total_compressed_size;

    // Print results to terminal
    // std::cout << "Original Size: " << original_size << " bytes" << std::endl;
    std::cout << "Compressed Size: " << total_compressed_size << " bytes" << std::endl;
    std::cout << "Compression Ratio (original size/ compressed size): " << compression_ratio << std::endl;
    std::cout << "\n";

    std::cout << "Total chunk size as dedup input: " << input_size_dedup << " bytes in " << dedup_chunk << " chunks.\n";
    std::cout << "Deduplication Contribution (dedup headers in Compressed size): " << deduplication_contribution
              << " bytes" << std::endl;
    std::cout << "\n";

    std::cout << "Total chunk size as LZW input: " << input_size_lzw << " bytes in " << lzw_chunk << " chunks.\n";
    std::cout << "LZW Contribution (LZW header + LZ-compressed content in "
                 "Compressed size): "
              << lzw_contribution << " bytes" << std::endl;
    return true;
}

int main(int argc, char *argv[]) {
    if (argc <= 2) {
        std::cerr << "Using " << argv[1] << " as output file name." << std::endl;
    }
    const char *output_filename = argv[1];  // Take the output file name from the command line

    stopwatch ethernet_timer;
    stopwatch runtime;
    unsigned char *input_buffers[NUM_PACKETS];
    int writer = 0;
    int done = 0;
    int length = 0;
    int count = 0;                  // If 'count' is unused, consider removing or using it
    int blocksize = BLOCKSIZE_ENC;  // Fixed macro name
    ESE532_Server server;

    // Handle command-line input
    handle_input(argc, argv, &blocksize);

    // Allocate memory for the file buffer
    file_buffer = (unsigned char *)malloc(sizeof(unsigned char) * 70000000);
    if (file_buffer == NULL) {
        std::cerr << "Failed to allocate memory for file buffer.\n";
        return 1;
    }

    // Allocate memory for input buffers
    for (int i = 0; i < NUM_PACKETS; i++) {
        input_buffers[i] = (unsigned char *)malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
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
    // runtime.start();
    //  Setup the server
    server.setup_server(blocksize);
    writer = pipe_depth;
    server.get_packet(input_buffers[writer]);
    count++;

    // get packet
    unsigned char *buffer = input_buffers[writer];

    // decode
    done = buffer[1] & DONE_BIT_L;
    length = buffer[0] | (buffer[1] << 8);
    length &= ~DONE_BIT_H;
    // printing takes time so be wary of transfer rate
    // printf("length: %d offset %d\n",length,offset);

    // we are just memcpy'ing here, but you should call your
    // top function here.
    memcpy(&file_buffer[offset], &buffer[HEADER], length);

    offset += length;
    writer++;

    // Receive remaining packets
    while (!done) {
        if (writer == NUM_PACKETS) {
            writer = 0;
        }

        ethernet_timer.start();
        server.get_packet(input_buffers[writer]);
        ethernet_timer.stop();

        unsigned char *buffer = input_buffers[writer];

        // Decode packet
        done = buffer[1] & DONE_BIT_L;
        length = buffer[0] | (buffer[1] << 8);
        length &= ~DONE_BIT_H;

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
    //save_to_text_file(file_buffer, offset, "received_output.txt");

    size_t total_input_bytes = offset;                // 'offset' accumulates the total bytes received
    size_t total_input_bits = total_input_bytes * 8;  // Convert bytes to bits

    // Initialize output data buffer
    std::vector<unsigned char> output_data;
    std::cout << "Original Size: " << offset << " bytes" << std::endl;
    runtime.start();
    // Process the received file buffer
    if (!process_packet(file_buffer, offset, output_data)) {
        std::cerr << "Error processing packet!" << std::endl;
    }
    runtime.stop();
    // Access the compressed data in memory
    std::vector<unsigned char> &compressed_data = output_data;

    // Example: Print the size of the compressed data
    std::cout << "Compressed data size: " << compressed_data.size() << " bytes." << std::endl;

    std::cout << std::dec << std::endl;

    // Save the processed data to a binary file
    FILE *outfd = fopen(output_filename, "wb");
    if (outfd == NULL) {
        std::cerr << "Error: Could not open " << output_filename << " for writing." << std::endl;
        // Free allocated resources before exiting
        for (int i = 0; i < NUM_PACKETS; i++) {
            free(input_buffers[i]);
        }
        free(file_buffer);
        return 1;
    }

    size_t bytes_written = fwrite(output_data.data(), 1, output_data.size(), outfd);
    if (bytes_written != output_data.size()) {
        std::cerr << "Warning: Not all data was written to the file. Expected " << output_data.size()
                  << " bytes, but wrote " << bytes_written << " bytes." << std::endl;
    } else {
        std::cout << "All data written successfully." << std::endl;
    }
    printf("Written file with %zu bytes\n", bytes_written);
    fclose(outfd);

    // Free allocated memory
    for (int i = 0; i < NUM_PACKETS; i++) {
        free(input_buffers[i]);
    }
    free(file_buffer);

    // Calculate and display throughput
    std::cout << "--------------- Key Throughputs ---------------" << std::endl;
    float ethernet_latency = ethernet_timer.latency() / 1000.0f;
    float input_throughput = (bytes_written * 8.0f / 1000000.0f) / ethernet_latency;  // Mb/s
    std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
              << " (Latency: " << ethernet_latency << "s)." << std::endl;

    std::cout << "-------------------------------------------------" << std::endl;

    // Calculate throughput in Gb/s
    double total_data_megabits = static_cast<double>(total_input_bits) / 1000000.0;  // Convert bits to gigabits
    double total_time_seconds =
        (cdc_time.latency() + sha_time.latency() + dedup_time.latency() + lzw_time.latency()) / 1000.0000f;
    double overall_throughput = total_data_megabits / total_time_seconds;

    double cdc_throughput = total_data_megabits / (cdc_time.latency() / 1000.0);
    double sha_throughput = total_data_megabits / (sha_time.latency() / 1000.0);
    double dedup_throughput = total_data_megabits / (dedup_time.latency() / 1000.0);
    double lzw_throughput = (input_size_lzw * 8 / 1000000.0) / (lzw_time.latency() / 1000.0);

    std::cout << "Total Data Processed (input original size): " << total_input_bits << " bits.\n";
    std::cout << "Total Time Taken of Encoder: " << total_time_seconds << " seconds or "
              << (cdc_time.latency() + sha_time.latency() + dedup_time.latency() + lzw_time.latency()) << "ms.\n";
    std::cout << "Overall Throughput: " << overall_throughput << " Mb/s.\n";
    std::cout << "-------------------------------------------------" << std::endl;

    std::cout << "\n";

    std::cout << "CDC throughput " << cdc_throughput << " Mb/s.\n";
    std::cout << "SHA throughput " << sha_throughput << " Mb/s.\n";
    std::cout << "Dedup throughput " << dedup_throughput << " Mb/s.\n";
    std::cout << "LZW throughput " << lzw_throughput << " Mb/s.\n";
    std::cout << "\n";

    std::cout << "Average time of CDC: " << cdc_time.avg_latency() << " (ms).\n";
    std::cout << "Average time of SHA: " << sha_time.avg_latency() << " (ms).\n";
    std::cout << "Average time of Dedup: " << dedup_time.avg_latency() << " (ms).\n";
    std::cout << "Average time of LZW: " << lzw_time.avg_latency() << " (ms).\n";
    std::cout << "\n";
    std::cout << "Total time of CDC: " << cdc_time.latency() << " (ms).\n";
    std::cout << "Total time of SHA: " << sha_time.latency() << " (ms).\n";
    std::cout << "Total time of Dedup: " << dedup_time.latency() << " (ms).\n";
    std::cout << "Total time of LZW: " << lzw_time.latency() << " (ms).\n";

    return 0;
}