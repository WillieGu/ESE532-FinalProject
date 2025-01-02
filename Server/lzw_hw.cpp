#include "lzw_hw.h"

uint16_t hw_dictionary[MAX_DICT_SIZE][2] = {0};  // Global dictionary

void hw_initialize_dictionary() {
    static bool initialized = false;  // Static flag to ensure one-time initialization
    if (!initialized) {
        for (uint16_t i = 0; i < 256; ++i) {
            #pragma HLS UNROLL
            hw_dictionary[i][0] = 0xFFFF;  // No prefix for single characters
            hw_dictionary[i][1] = i;       // Character itself
        }
        initialized = true;  // Set flag to true after initialization
    }
}

void lzw_encode_hw(const unsigned char *input_data, uint16_t input_size,
                   uint8_t compressed_data[OUTPUT_SIZE],
                   uint32_t *compressed_size_buf)
{
#pragma HLS INTERFACE m_axi port=input_data bundle=axim1 depth=8192
#pragma HLS INTERFACE m_axi port=compressed_data bundle=axim2 depth=40960
#pragma HLS INTERFACE m_axi port=compressed_size_buf bundle=axim2 depth=1

      if (input_size == 0) {
        // Handle empty input
        *compressed_size_buf = 0;
        return;
      }
    //uint16_t hw_dictionary[MAX_DICT_SIZE][2] = {0}; // Dictionary
#pragma HLS ARRAY_PARTITION variable=hw_dictionary type=block factor=1024 dim=1
#pragma HLS ARRAY_PARTITION variable=hw_dictionary complete dim=2
    
    hw_initialize_dictionary();

//     for (uint16_t i = 0; i < 256; ++i)
//     {
// #pragma HLS UNROLL
//         hw_dictionary[i][0] = 0xFFFF; // No prefix for single characters
//         hw_dictionary[i][1] = i;      // Character itself
//     }

#pragma HLS BIND_STORAGE variable=hw_dictionary type=ram_2p impl=bram latency=1

    uint16_t dict_size = 256;              // Next available code in the dictionary
    uint16_t current_code = input_data[0]; // Current prefix code
    uint16_t code_words[INPUT_SIZE];       // Store the encoded code words
    uint16_t code_word_count = 0;

    // LZW Compression Algorithm
    for (uint16_t i = 1; i < input_size; ++i)
    {
        #pragma HLS PIPELINE II = 1
        uint8_t next_char = input_data[i];
        bool found = false;

        // Search for the combined string in the dictionary
        for (uint16_t j = 256; j < dict_size; ++j)
        {
            bool match = (hw_dictionary[j][0] == current_code) & (hw_dictionary[j][1] == next_char);
            if (match)
            {
                current_code = j;
                found = true;
                break;
            }
        }

        if (!found)
        {
            // Output the current_code
            code_words[code_word_count++] = current_code;

            // Add new string to the dictionary if there's space
            if (dict_size < MAX_DICT_SIZE)
            {
                hw_dictionary[dict_size][0] = current_code;
                hw_dictionary[dict_size][1] = next_char;
                ++dict_size;
            }

            current_code = next_char; // Start new string with next_char
        }
    }

    // Output the code for the last string
    code_words[code_word_count++] = current_code;

    // Pack code words into bytes (MSB-first)
    uint32_t bit_buffer = 0;
    uint16_t bit_count = 0;
    uint32_t compressed_size = 0;

    for (uint16_t i = 0; i < code_word_count; ++i)
    {
        #pragma HLS PIPELINE II = 1
        uint16_t code = code_words[i] & CODE_MASK;
        bit_buffer = (bit_buffer << CODE_LENGTH) | code;
        bit_count += CODE_LENGTH;

        // Extract bytes from the bit buffer
        while (bit_count >= 8)
        {
            uint8_t byte = (bit_buffer >> (bit_count - 8)) & 0xFF;
            compressed_data[compressed_size++] = byte;
            bit_count -= 8;
        }
    }

    // Handle remaining bits (if any)
    if (bit_count > 0)
    {
        uint8_t byte = (bit_buffer << (8 - bit_count)) & 0xFF;
        compressed_data[compressed_size++] = byte;
    }
    // Write compressed size back to global memory
    *compressed_size_buf = compressed_size;
}
