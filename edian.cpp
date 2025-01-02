#include <cstdint>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <iostream>
/**
 * @brief Appends a uint32_t to a byte buffer in Little Endian order.
 *
 * @param buffer The byte buffer to append to.
 * @param value The uint32_t value to append.
 */
void append_uint32_le(std::vector<uint8_t> &buffer, uint32_t value)
{
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

int main()
{
    std::vector<uint8_t> buffer;
    uint32_t test_value = 0x12345678; // Example value

    append_uint32_le(buffer, test_value);

    // Expected buffer content: 0x78, 0x56, 0x34, 0x12
    std::cout << "Buffer content in hex:";
    for (uint8_t byte : buffer)
    {
        printf(" %02X", byte);
    }
    std::cout << std::endl;

    return 0;
}