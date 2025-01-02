// test_lzw.cpp

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cstring>
#include <fstream>

// Include necessary headers and define constants
#define CODE_LENGTH 13
#define CODE_MASK ((1 << CODE_LENGTH) - 1)

void lzw_encode(const unsigned char *input_data, size_t input_size, std::vector<uint8_t> &compressed_data)
{
    if (input_size == 0)
    {
        // Handle empty input
        return;
    }

    // Initialize dictionary with single-character strings (codes 0 to 255)
    size_t dict_size = 256;                                // Next available code in the dictionary
    const size_t max_dict_size = (size_t)1 << CODE_LENGTH; // Maximum dictionary size based on code length

    // Dictionary mapping strings to codes
    std::unordered_map<std::string, uint16_t> dictionary;
    for (uint16_t i = 0; i < 256; ++i)
    {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current_str;
    current_str += input_data[0];
    std::vector<uint16_t> code_words;

    // LZW Compression Algorithm
    for (size_t i = 1; i < input_size; ++i)
    {
        char next_char = input_data[i];
        std::string combined_str = current_str + next_char;

        if (dictionary.find(combined_str) != dictionary.end())
        {
            current_str = combined_str;
        }
        else
        {
            // Output the code for current_str
            code_words.push_back(dictionary[current_str]);

            // Add new string to the dictionary if there's space
            if (dict_size < max_dict_size)
            {
                dictionary[combined_str] = dict_size++;
            }

            current_str = std::string(1, next_char);
        }
    }

    // Output the code for the last string
    code_words.push_back(dictionary[current_str]);

    // Now pack the code words into bytes as per the decoder's expectations
    // We need to pack bits starting from the most significant bit in each byte
    uint32_t bit_buffer = 0;
    int bit_count = 0;

    // Prepare compressed_data vector
    compressed_data.clear();

    for (uint16_t code : code_words)
    {
        // Ensure code is within CODE_LENGTH bits
        code &= CODE_MASK;

        // Shift code into the bit buffer (MSB-first)
        bit_buffer = (bit_buffer << CODE_LENGTH) | code;
        bit_count += CODE_LENGTH;

        // Extract bytes from the bit buffer
        while (bit_count >= 8)
        {
            // Extract the top 8 bits
            uint8_t byte = (bit_buffer >> (bit_count - 8)) & 0xFF;
            compressed_data.push_back(byte);
            bit_count -= 8;
        }
    }

    // Handle remaining bits (if any)
    if (bit_count > 0)
    {
        // Left-align the remaining bits in the final byte (MSB-first)
        uint8_t byte = (bit_buffer << (8 - bit_count)) & 0xFF;
        compressed_data.push_back(byte);
    }
}

// Decompress function from decoder.cpp
static size_t Input_position;
static std::vector<std::string> Code_table;

static int Read_code(const std::vector<uint8_t> &input_data)
{
    static int Byte = 0;

    int Code = 0;
    int Length = CODE_LENGTH;
    for (int i = 0; i < Length; i++)
    {
        if (Input_position % 8 == 0)
        {
            if (Input_position / 8 >= input_data.size())
            {
                // Reached end of input data
                return -1;
            }
            Byte = input_data[Input_position / 8];
        }
        Code = (Code << 1) | ((Byte >> (7 - Input_position % 8)) & 1);
        Input_position++;
    }
    return Code;
}

static const std::string Decompress(const std::vector<uint8_t> &input_data)
{
    Input_position = 0;

    Code_table.clear();
    for (int i = 0; i < 256; i++)
        Code_table.push_back(std::string(1, (char)i));

    int Old = Read_code(input_data);
    if (Old == -1)
    {
        // Handle EOF or error
        return "";
    }

    std::string Symbol(1, (unsigned char)Old);
    std::string Output = Symbol;
    while (true)
    {
        int New = Read_code(input_data);
        if (New == -1)
        {
            break; // Reached EOF
        }

        std::string Symbols;
        if (New >= (int)Code_table.size())
            Symbols = Code_table[Old] + Symbol;
        else
            Symbols = Code_table[New];

        Output += Symbols;
        Symbol = std::string(1, (unsigned char)Symbols[0]);
        Code_table.push_back(Code_table[Old] + Symbol);
        Old = New;
    }

    return Output;
}

int main()
{
    // Step 1: Define the input data
    const std::string input_text = "TOBEORNOTTOBEORTOBEORNOT";
    const unsigned char *input_data = reinterpret_cast<const unsigned char *>(input_text.c_str());
    size_t input_size = input_text.size();

    // Step 2: Compress the input data
    std::vector<uint8_t> compressed_data;
    lzw_encode(input_data, input_size, compressed_data);

    // Optionally, write the compressed data to a file for inspection
    std::ofstream compressed_file("compressed.bin", std::ios::binary);
    compressed_file.write(reinterpret_cast<char *>(compressed_data.data()), compressed_data.size());
    compressed_file.close();

    // Step 3: Decompress the compressed data
    std::string decompressed_output = Decompress(compressed_data);

    // Step 4: Verify that the decompressed output matches the original input
    if (decompressed_output == input_text)
    {
        std::cout << "Success! Decompressed output matches the original input.\n";
        std::cout << "Original Input: \"" << input_text << "\"\n";
        std::cout << "Decompressed Output: \"" << decompressed_output << "\"\n";
    }
    else
    {
        std::cout << "Failure! Decompressed output does not match the original input.\n";
        std::cout << "Original Input: \"" << input_text << "\"\n";
        std::cout << "Decompressed Output: \"" << decompressed_output << "\"\n";
    }

    // Optionally, write the decompressed output to a file
    std::ofstream decompressed_file("decompressed_output.txt");
    decompressed_file << decompressed_output;
    decompressed_file.close();

    return 0;
}
