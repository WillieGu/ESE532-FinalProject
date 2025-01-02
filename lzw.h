#ifndef LZW_H
#define LZW_H

#include <cstdint>  // For uint32_t, uint8_t, etc.
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <iostream>
//#include "common.h"

#define CAPACITY 8192          // Size of the hash table
#define ASSOCIATE_MEM_STORE 64 // Size of the associative memory
#define SEED 524057            // Seed for the custom hash function

#define INPUT_SIZE 8192 //9000//1024 //8192                    // Maximum input size
#define OUTPUT_SIZE 40960                  // Maximum output size
// Function prototype for LZW compression
/**
 * @brief Encodes input data using the LZW (Lempel-Ziv-Welch) compression algorithm.
 *
 * This function takes an array of input data and compresses it using the LZW
 * algorithm, storing the compressed result in a provided vector.
 *
 * @param input_data Pointer to the array of input data to be compressed.
 * @param input_size The size of the input data array in bytes.
 * @param compressed_data Reference to a vector where the compressed output will be stored.
 *
 * @note The input data must be non-empty for the function to perform compression.
 * The `compressed_data` vector will be cleared and then filled with the compressed data.
 * Each compressed code word is packed into bytes using a 13-bit code length, with the most
 * significant bit (MSB) first.
 *
 * @input_form The input is an array of `unsigned char` elements representing the data to
 * be compressed. The size of the array is specified by `input_size`.
 *
 * @output_form The output is stored in a `std::vector<uint8_t>` containing the compressed
 * data packed in bytes. Each byte may contain part of one or more LZW codes.
 */
//void lzw_encode(const unsigned char *input_data, size_t input_size, std::vector<uint8_t> &compressed_data);
void initialize_dictionary();
void lzw_encode(const unsigned char *input_data, uint16_t input_size, uint8_t compressed_data[OUTPUT_SIZE], uint32_t *compressed_size);
#endif
