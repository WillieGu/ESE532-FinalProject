// Host.cpp

#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>  // Include if not already included
#include <vector>

#include "../Server/common.h"
#include "../Server/stopwatch.h"
#include "utils.h"

// Constants
#define NUM_PACKETS 8
#define PIPE_DEPTH 4
#define DONE_BIT_L (1 << 7)
#define DONE_BIT_H (1 << 15)
#define MAX_DICT_SIZE (1 << 13)
#define NUM_ELEMENTS 16384
#define BLOCKSIZE_ENC 8192  // Adjust as needed
#define HEADER 2

// OpenCL constants
#define INPUT_SIZE 8192    // Maximum chunk size in bytes
#define OUTPUT_SIZE 40960  // Adjust based on expected compressed data size

using namespace std;

// Global Variables
int offset = 0;
unsigned char* file_buffer;

stopwatch cdc_time, sha_time, dedup_time, lzw_time;

// Initialize counters for compression calculation
size_t deduplication_contribution = 0;
size_t lzw_contribution = 0;
size_t total_compressed_size = 0;
size_t input_size_dedup = 0, input_size_lzw = 0, dedup_chunk = 0, lzw_chunk = 0;

std::vector<cl::Device> get_xilinx_devices();
char* read_binary_file(const std::string& xclbin_file_name, unsigned& nb);

// Helper Functions for Byte Order Handling
void append_uint32_le(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Warning: (must input 2 arguments) Usage: " << argv[0] << " <output filename>" << std::endl;
        return 1;
    }

    const char* output_filename = argv[1];

    // ------------------------------------------------------------------------------------
    // Step 1: Initialize the OpenCL environment
    // ------------------------------------------------------------------------------------
    cl_int err;
    std::string binaryFile = "encoder.xclbin";
    unsigned fileBufSize;
    std::cout << "INFO: Initializing OpenCL environment..." << std::endl;

    std::vector<cl::Device> devices = get_xilinx_devices();

    if (devices.empty()) {
        std::cerr << "ERROR: No Xilinx devices found." << std::endl;
        return 1;
    }

    devices.resize(1);
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL, &err);

    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create OpenCL context. Error code: " << err << std::endl;
        return 1;
    }
    char* fileBuf = read_binary_file(binaryFile, fileBufSize);

    if (!fileBuf) {
        std::cerr << "ERROR: Failed to read the binary file." << std::endl;
        return 1;
    }
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    cl::Program program(context, devices, bins, NULL, &err);

    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create program from binary. Error code: " << err << std::endl;
        return 1;
    }
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create command queue. Error code: " << err << std::endl;
        return 1;
    }
    // Create LZW kernel
    cl::Kernel lzw_kernel(program, "lzw_encode_hw", &err);
    if (err != CL_SUCCESS) {
        std::cerr << "ERROR: Failed to create kernel. Error code: " << err << std::endl;
        return 1;
    }
    // ------------------------------------------------------------------------------------
    // Step 2: Allocate buffers and initialize variables
    // ------------------------------------------------------------------------------------
    // Allocate OpenCL buffers

    // cl::Buffer input_buf(context, CL_MEM_READ_ONLY, sizeof(unsigned char) * MAX_CHUNK_SIZE, NULL, &err);
    // cl::Buffer output_buf(context, CL_MEM_WRITE_ONLY, sizeof(uint8_t) * MAX_OUTPUT_SIZE, NULL, &err);

    // ------------------------------------------------------------------------------------
    // Step 3: Receive and process packets
    // ------------------------------------------------------------------------------------
    stopwatch ethernet_timer;
    stopwatch runtime;
    unsigned char* input_buffers[NUM_PACKETS];
    int writer = 0;
    int done = 0;
    int length = 0;
    int count = 0;
    int blocksize = BLOCKSIZE_ENC;
    ESE532_Server server;

    // Handle command-line input
    handle_input(argc, argv, &blocksize);

    // Allocate memory for the file buffer
    file_buffer = (unsigned char*)malloc(sizeof(unsigned char) * 70000000);
    if (file_buffer == NULL) {
        std::cerr << "Failed to allocate memory for file buffer.\n";
        return 1;
    }

    // Allocate memory for input buffers
    for (int i = 0; i < NUM_PACKETS; i++) {
        input_buffers[i] = (unsigned char*)malloc(sizeof(unsigned char) * (NUM_ELEMENTS + HEADER));
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
    writer = PIPE_DEPTH;
    server.get_packet(input_buffers[writer]);
    count++;

    // Get initial packet
    unsigned char* buffer = input_buffers[writer];

    // Decode
    done = buffer[1] & DONE_BIT_L;
    length = buffer[0] | (buffer[1] << 8);
    length &= ~DONE_BIT_H;

    // Copy data into file_buffer
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

        buffer = input_buffers[writer];

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

    size_t total_input_bytes = offset;
    size_t total_input_bits = total_input_bytes * 8;

    // ------------------------------------------------------------------------------------
    // Step 4: Process the file buffer
    // ------------------------------------------------------------------------------------
    runtime.start();

    // Step 4.1: Perform Content-Defined Chunking (CDC)
    std::vector<size_t> chunk_positions;
    cdc_time.start();
    content_defined_chunking(file_buffer, total_input_bytes, chunk_positions);
    cdc_time.stop();

    size_t chunk_count = (chunk_positions.empty()) ? 0 : chunk_positions.size() - 1;
    std::cout << "CDC function called: chunk_count = " << chunk_count << std::endl;

    // Step 4.2: Initialize deduplication structures
    // Initialize deduplication structures with preallocated capacity
    size_t estimated_unique_chunks = 1000000; // Adjust based on expected data
    std::unordered_map<SHA256Hash, int64_t, SHA256HashHash> chunk_hash_map;
    chunk_hash_map.reserve(estimated_unique_chunks);
    int64_t lzw_chunk_index = 0;
    size_t original_size = total_input_bytes;

    // Initialize output data buffer
    std::vector<unsigned char> output_data;
    cl::Buffer input_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_READ_ONLY, sizeof(unsigned char) * INPUT_SIZE, nullptr,
                         &err);
    cl::Buffer output_buf(context, CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY, sizeof(uint8_t) * OUTPUT_SIZE, nullptr,
                          &err);
    cl::Buffer compressed_size_buf(context, CL_MEM_WRITE_ONLY, sizeof(uint32_t), nullptr, &err);

    // Map buffers once
    // unsigned char* input_data_host =
    //     (unsigned char*)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0, sizeof(unsigned char) * INPUT_SIZE);
    // unsigned char* compressed_data_host =
    //     (unsigned char*)q.enqueueMapBuffer(output_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(uint8_t) * OUTPUT_SIZE);
    // size_t* compressed_size_host =
    //     (size_t*)q.enqueueMapBuffer(compressed_size_buf, CL_TRUE, CL_MAP_READ, 0, sizeof(size_t));

    // Step 4.3: Process each chunk
    for (size_t i = 0; i < chunk_count; i++) {
        size_t start = chunk_positions[i];
        size_t end = chunk_positions[i + 1];
        size_t chunk_size = end - start;
        unsigned char* chunk = file_buffer + start;

        if (chunk_size == 0 || chunk_size > MAX_CHUNK_SIZE) {
            std::cerr << "Error: Invalid chunk size " << chunk_size << " for chunk " << i << std::endl;
            continue;
        }

        // NEWEST SHA VERSION
        BYTE hash_output[SHA256_BLOCK_SIZE];
        // Call NEON-based parallel SHA-256 implementation
        sha_time.start();
        // sha256_compute_parallel(chunk, chunk_size, hash_output);
        SHA256_CTX ctx;
        sha256_hash(&ctx, chunk, hash_output, chunk_size);
        sha_time.stop();

        // Convert hash_output to binary SHA256Hash type
        SHA256Hash hash_value;
        std::memcpy(hash_value.data(), hash_output, 32);

        dedup_time.start();
        int64_t duplicate_index = dedup(hash_value, chunk_hash_map, lzw_chunk_index);
        dedup_time.stop();

        if (duplicate_index != -1) {
            // Duplicate chunk found
            uint32_t header = (duplicate_index << 1) | 0x1;  // Bit 0 is 1
            // std::cout << "Duplicate Index of chunk " << i << " is: " << duplicate_index << std::endl;

            // // Print header in binary format
            // std::cout << "Duplicate Header (Binary) of chunk" << i << " is: " << std::bitset<32>(header) << std::endl;

            // Append header to output_data
            append_uint32_le(output_data, header);

            // Update contributions
            deduplication_contribution += 4;
            total_compressed_size += 4;
            input_size_dedup += chunk_size;
            dedup_chunk++;
            //std::cout << "Duplicate chunk " << i << std::endl;
        } else {
            // New chunk, perform LZW compression using FPGA
            //uint8_t compressed_data[OUTPUT_SIZE];
            uint32_t compressed_size = 0;

            // Zero out compressed_size and write to device buffer
            q.enqueueWriteBuffer(compressed_size_buf, CL_TRUE, 0, sizeof(uint32_t), &compressed_size);
            q.finish();

            if (chunk_size > INPUT_SIZE) {
                std::cerr << "Error: Chunk size exceeds input buffer size." << std::endl;
                return 1;
            }
            // Map input buffer for writing
            unsigned char* input_data_host = (unsigned char*)q.enqueueMapBuffer(input_buf, CL_TRUE, CL_MAP_WRITE, 0,
                                                                                sizeof(unsigned char) * INPUT_SIZE);
            // Copy your data into input_data_host
            memcpy(input_data_host, chunk, chunk_size);

            // Unmap input buffer before migrating it to the device
            q.enqueueUnmapMemObject(input_buf, input_data_host);
            q.finish();

            //std::cout << "Chunk size of chunk i = " << i << " is " << chunk_size << "\n";
            // Set kernel arguments
            lzw_kernel.setArg(0, input_buf);                         // Input data buffer
            lzw_kernel.setArg(1, static_cast<uint16_t>(chunk_size));  // Scalar: Input size
            lzw_kernel.setArg(2, output_buf);                        // Output data buffer
            lzw_kernel.setArg(3, compressed_size_buf);               // Pointer to compressed size buffer

            // Memory object migration enqueue host->device
            cl::Event event_sp;
            // q.enqueueMigrateMemObjects({input_buf}, 0 /*o means from host*/, NULL, &event_sp);
            // clWaitForEvents(1, (const cl_event*)&event_sp);
            // Migrate input and compressed_size buffers to device
            q.enqueueMigrateMemObjects({input_buf, compressed_size_buf}, 0 /* Host to Device */);
            q.finish();

            // Launch kernel
            lzw_time.start();
            q.enqueueTask(lzw_kernel, NULL, &event_sp);
            lzw_time.stop();
            clWaitForEvents(1, (const cl_event*)&event_sp);  // Wait for kernel execution to finish
            q.finish();
            //lzw_time.stop();

            // Retrieve compressed size
            q.enqueueReadBuffer(compressed_size_buf, CL_TRUE, 0, sizeof(uint32_t), &compressed_size);
            q.finish();
            //std::cout << "Compressed size retrieved: " << compressed_size << std::endl;

            // if (compressed_size == 0) {
            //     std::cerr << "Compression failed for chunk " << i << std::endl;
            //     //continue;  // Skip to the next chunk
            // } else {
            //     std::cout << "Compressed size of NEW chunk " << i << " is: " << compressed_size << std::endl;
            // }

            // Read compressed data
            uint8_t* compressed_data_host = new uint8_t[compressed_size];
            q.enqueueReadBuffer(output_buf, CL_TRUE, 0, compressed_size, compressed_data_host);
            // Print the compressed data
            // std::cout << "Compressed data for chunk " << i << ": ";
            // for (size_t j = 0; j < compressed_size; ++j) {
            //     std::cout << std::hex << static_cast<int>(compressed_data_host[j]) << " ";
            // }
            // std::cout << std::dec << std::endl;
            // // Copy the compressed data from compressed_data_host to compressed_data
            // if (compressed_size > OUTPUT_SIZE) {
            //     std::cerr << "Error: Compressed size exceeds buffer size for chunk " << i << std::endl;
            //     continue;
            // }
            // //memcpy(compressed_data, compressed_data_host, compressed_size);

            uint32_t header = (compressed_size << 1) | 0;
            // std::cout << "Compressed Header (Binary) of chunk" << i << " is: " << std::bitset<32>(header) << std::endl;

            append_uint32_le(output_data, header);
            //output_data.insert(output_data.end(), compressed_data, compressed_data + compressed_size);
            output_data.insert(output_data.end(), compressed_data_host, compressed_data_host + compressed_size);
            // Update statistics
            lzw_contribution += 4 + compressed_size;
            total_compressed_size += 4 + compressed_size;
            input_size_lzw += chunk_size;
            lzw_chunk++;

            // Free host memory
            delete[] compressed_data_host;
        }
    }

    runtime.stop();

    // ------------------------------------------------------------------------------------
    // Step 5: Write output and print statistics
    // ------------------------------------------------------------------------------------

    // Save the processed data to a binary file
    FILE* outfd = fopen(output_filename, "wb");
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

    // Print compression statistics
    double compression_ratio = static_cast<double>(original_size) / total_compressed_size;

    std::cout << "Original Size: " << original_size << " bytes" << std::endl;
    std::cout << "Compressed Size: " << total_compressed_size << " bytes" << std::endl;
    std::cout << "Compression Ratio (original size / compressed size): " << compression_ratio << std::endl;
    std::cout << "\n";
    std::cout << "Total chunk size as dedup input: " << input_size_dedup << " bytes in " << dedup_chunk << " chunks.\n";
    std::cout << "Deduplication Contribution (dedup headers in Compressed size): " << deduplication_contribution
              << " bytes" << std::endl;
    std::cout << "\n";

    std::cout << "Total chunk size as LZW input: " << input_size_lzw << " bytes in " << lzw_chunk << " chunks.\n";
    std::cout << "LZW Contribution (LZW header + LZ-compressed content in "
                 "Compressed size): "
              << lzw_contribution << " bytes" << std::endl;

    // ------------------------------------------------------------------------------------
    // Step 6: Clean up resources
    // ------------------------------------------------------------------------------------

    // Free allocated memory
    for (int i = 0; i < NUM_PACKETS; i++) {
        free(input_buffers[i]);
    }
    free(file_buffer);

    delete[] fileBuf;
    std::cout << "INFO: Program completed successfully." << std::endl;

    // ------------------------------------------------------------------------------------
    // Step 7: Calculate and display throughput
    // ------------------------------------------------------------------------------------
    // Calculate and display throughput
    std::cout << "--------------- Key Throughputs ---------------" << std::endl;
    float ethernet_latency = ethernet_timer.latency() / 1000.0f;
    float input_throughput = (bytes_written * 8.0f / 1000000.0f) / ethernet_latency;  // Mb/s
    std::cout << "Input Throughput to Encoder: " << input_throughput << " Mb/s."
              << " (Latency: " << ethernet_latency << "s)." << std::endl;

    std::cout << "-------------------------------------------------" << std::endl;

    // Calculate throughput in Gb/s
    double total_data_megabits = static_cast<double>(total_input_bits) / 1000000.0;  // Convert bits to Megabits
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
