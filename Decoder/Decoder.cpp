#include <stdint.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <bitset> 

#define CODE_LENGTH (13)
#define MAX_CHUNK_SIZE 40960

typedef std::vector<std::string> code_table;
typedef std::vector<std::string> chunk_list;

static code_table Code_table;

static std::ifstream Input;
static size_t Input_position;

static int Read_code(void) {
    static unsigned char Byte;

    int Code = 0;
    int Length = CODE_LENGTH;
    for (int i = 0; i < Length; i++) {
        if (Input_position % 8 == 0) Byte = Input.get();
        Code = (Code << 1) | ((Byte >> (7 - Input_position % 8)) & 1);
        Input_position++;
    }
    return Code;
}

static const std::string Decompress(size_t Size) {
    Input_position = 0;

    Code_table.clear();
    for (int i = 0; i < 256; i++) Code_table.push_back(std::string(1, (char)i));

    int Old = Read_code();
    // if (Old >= (int)Code_table.size() || Old < 0) {
    //     std::cerr << "Error: Invalid initial code. Old=" << Old << ", Code_table size=" << Code_table.size() << "\n";
    //     return "";
    // }
    std::string Symbol(1, Old);
    std::string Output = Symbol;
    while (Input_position / 8 < Size - 1) {
        int New = Read_code();
        //std::cout << "Read New code: " << New << " (Code_table size: " << Code_table.size() << ")\n";
        std::string Symbols;
        if (New >= (int)Code_table.size())
            Symbols = Code_table[Old] + Symbol;
        else
            Symbols = Code_table[New];
        Output += Symbols;
        Symbol = std::string(1, Symbols[0]);
        Code_table.push_back(Code_table[Old] + Symbol);
        Old = New;
    }

    return Output;
}

int main(int Parameter_count, char *Parameters[]) {
    if (Parameter_count < 3) {
        std::cout << "Usage: " << Parameters[0] << " <Compressed file> <Decompressed file>\n";
        return EXIT_SUCCESS;
    }

    Input.open(Parameters[1], std::ios::binary);
    if (!Input.good()) {
        std::cerr << "Could not open input file.\n";
        return EXIT_FAILURE;
    }

    std::ofstream Output(Parameters[2], std::ios::binary);
    if (!Output.good()) {
        std::cerr << "Could not open output file.\n";
        return EXIT_FAILURE;
    }

    chunk_list Chunks;

    int total_chunks_in_header = 0;
    int successful_chunks = 0;  // Track chunks successfully processed
    int failed_chunks = 0;      // Track chunks that failed

    int i = 0;
    while (true) {
        uint32_t Header;
        Input.read((char *)&Header, sizeof(int32_t));
        if (Input.eof()) break;
        total_chunks_in_header++;

        if ((Header & 1) == 0) {
            int Chunk_size = Header >> 1;

            // Validate chunk size
            if (Chunk_size <= 0 || Chunk_size > MAX_CHUNK_SIZE) {
                std::cerr << "Error: Invalid chunk size " << Chunk_size << " in header.\n";
                failed_chunks++;
                std::cerr << "Error: Invalid chunk size " << Chunk_size << " in header. Raw header value: " << std::bitset<32>(Header)
                          << "\n";
                continue;
            }

            const std::string &Chunk = Decompress(Chunk_size);

            if (Chunk.empty()) {
                std::cerr << "Error: Failed to decompress chunk " << total_chunks_in_header << ".\n";
                failed_chunks++;
                continue;
            }
            Chunks.push_back(Chunk);
            //std::cout << "Decompressed chunk of size " << Chunk.length() << ".\n";
            Output.write(&Chunk[0], Chunk.length());
            successful_chunks++;

        } else {
            int Location = Header >> 1;
            // if (Location < Chunks.size()) {  // defensive programming to avoid out-of-bounds reference
            //     const std::string &Chunk = Chunks[Location];
            //     //std::cout << "Found chunk of size " << Chunk.length() << " in database.\n";
            //     Output.write(&Chunk[0], Chunk.length());
            // } else {
            //     // std::cerr << "Location " << Location << " not in database of length " << Chunks.size()
            //     //           << " ignoring block.  Likely encoder error.\n";
            // }
            // Validate deduplication chunk location
            if (Location < 0 || Location >= (int)Chunks.size()) {
                std::cerr << "Error: Invalid dedup location " << Location << ".\n";
                failed_chunks++;
                continue;
            }

            const std::string &Chunk = Chunks[Location];
            Output.write(&Chunk[0], Chunk.length());
            successful_chunks++;
        }
        i++;
    }
    std::cout << "Decoding complete.\n";
    std::cout << "Total chunks in header: " << total_chunks_in_header << "\n";
    std::cout << "Successfully processed chunks: " << successful_chunks << "\n";
    std::cout << "Failed chunks: " << failed_chunks << "\n";

    if (successful_chunks == total_chunks_in_header) {
        std::cout << "All chunks were successfully processed!\n";
    } else {
        std::cerr << "Some chunks failed to process. Verify input file integrity.\n";
    }
    return EXIT_SUCCESS;
}