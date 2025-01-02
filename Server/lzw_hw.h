#ifndef LZW_HW_H
#define LZW_HW_H

#include <cstdint>
#include <stdint.h>
#include <stddef.h>
//#include <hls_stream.h>

// Constants for hardware
#define CODE_LENGTH 13                     // Code length in bits
#define CODE_MASK ((1 << CODE_LENGTH) - 1) // Mask to extract CODE_LENGTH bits
#define MAX_DICT_SIZE (1 << CODE_LENGTH)   // Maximum dictionary size
#define INPUT_SIZE 8192                 // Maximum input size
#define OUTPUT_SIZE 40960                  // Maximum output size

// Hardware-optimized LZW Encoding Function
extern "C"
{
    void hw_initialize_dictionary(); 
    void lzw_encode_hw(const unsigned char *input_data, uint16_t input_size, uint8_t compressed_data[OUTPUT_SIZE], uint32_t *compressed_size);
}

#endif // LZW_HW_H


