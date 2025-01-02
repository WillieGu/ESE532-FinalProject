// #include "cdc.h"
// #include "common.h"

// // Precomputed powers of 3 (up to WIN_SIZE+...)
// int expo_table[] = {
//     1,           // 3^0
//     3,           // 3^1
//     9,           // 3^2
//     27,          // 3^3
//     81,          // 3^4
//     243,         // 3^5
//     729,         // 3^6
//     2187,        // 3^7
//     6561,        // 3^8
//     19683,       // 3^9
//     59049,       // 3^10
//     177147,      // 3^11
//     531441,      // 3^12
//     1594323,     // 3^13
//     4782969,     // 3^14
//     14348907,    // 3^15
//     43046721,    // 3^16
//     129140163,   // 3^17
//     387420489,   // 3^18
//     1162261467   // 3^19
// };
// uint64_t last_hash = 0;

// // Rolling hash function
// uint64_t hash_func(unsigned char* input, unsigned int pos) {
//     uint64_t hash = 0;

//     if (last_hash == 0) {
//         // Initial hash calculation for the first window
//         for (int i = 0; i < WIN_SIZE; i++) {
//             hash += static_cast<uint64_t>(input[pos + WIN_SIZE - 1 - i]) * expo_table[i + 1];
//         }
//     } else {
//         // Rolling hash for subsequent positions
//         hash = last_hash * PRIME 
//                - static_cast<uint64_t>(input[pos - 1]) * expo_table[WIN_SIZE + 1] 
//                + static_cast<uint64_t>(input[pos + WIN_SIZE - 1]) * PRIME;
//     }

//     last_hash = hash;
//     return hash;
// }

// // CDC function to find chunk positions
// void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions) {
//     chunk_positions.push_back(0);  // Start of the first chunk

//     for (unsigned int i = WIN_SIZE; i < buff_size - WIN_SIZE; i++) {
//         uint64_t hash = hash_func(buffer, i);

//         if ((hash % MODULUS) == TARGET) {
//             chunk_positions.push_back(i + 1);  // End of the current chunk, start of the next
//         }
//     }

//     chunk_positions.push_back(buff_size);  // End of the last chunk
//     last_hash = 0;  // Reset hash for next call
// }

#include "cdc.h"
#include <stdint.h>
#include <stdlib.h>
#include <vector>

uint64_t gearTable[256]={
    0xf63bfbb258cb4baf, 0xc72bf934392f09f8, 0x39923acc50553b72, 0x6853377ee33eb584,
    0x4d0c064ef0393a1c, 0x45cadf6423d5fcca, 0x63a4bb6b7958c080, 0x184220c872f63aff,
    0xda10afa24249e94c, 0xc38ff37dbe825a58, 0x5d54f07bcb38fb34, 0x2cb101b02638a084,
    0x621a472c1229e635, 0xbef19b298f8abd7,  0xa163c13729ef46e0, 0xfbf4fed9cbab575,
    0xc6a2df396053bfb4, 0x313a6f1cc20f12f2, 0xf87720d5b330b7cd, 0x539b1dbbe10724a7,
    0x133c8874f801a411, 0xc4935e289c2ce5e3, 0x10f3b6a0ed98b158, 0x4dfad3ff9b79229c,
    0xdc75e7b332fd31bd, 0x391eb3a8876fd17f, 0x9ca863b2816938f3, 0x8d5e3d498ba838c3,
    0x6e8d833278bbe93f, 0xd751fbeefb43b660, 0x3b2e7ae6e3498c32, 0x90ebc53815bfcedb,
    0x827152dcec4573f7, 0xd408bd7f46ea5a51, 0xfd4531774e5ae155, 0xfae188b397c4f03a,
    0x1a10d31622852d9d, 0x42bdd312cf5dcbe9, 0x32c774310b70a780, 0x32f7ad6bd5af8c2e,
    0x853dc396e013e84e, 0xf4e53ffbd9b5fee0, 0xadcab60ede33fb8e, 0x6e6e87ff92bd6751,
    0xc9e5a07fffd3d80,  0xc60f6d436acf2ba1, 0x49ae4e9462b04686, 0xb38dbf72ea80d326,
    0x77b0dfbc890dbb24, 0x2e5da7353371328f, 0x9c90f9de35cb85,   0x84d17861744bb9ff,
    0xd3ec1374c2497680, 0x3d61dc891363996d, 0x5d36b6817e554069, 0x9602681c8b5dd616,
    0x8272bcdd9e03b2a4, 0x64054235a989fd60, 0x1a2f722e4cd7695f, 0xb0a1f20f4f0030a2,
    0x38349600dc31458f, 0xb675a499a37f467,  0xc9bd6986ecae3f07, 0x17b0b4a98eb504d,
    0x83008c2db8acd6cd, 0x6654876de218b02e, 0x6243855a5554a9a1, 0x38e3229bafdc3a8f,
    0x25ab24dd691ed88a, 0x7ee721ab11d3c860, 0x9be586b29c6f6635, 0xb89161c2d6c4d588,
    0x15a202499f89d2aa, 0xd90da29c7d2376c9, 0x6370ce3d278978c5, 0xa82482a33c205484,
    0x2c3eb2d2daac3747, 0x5a4f630b3701bf19, 0xac3a0c5ec2595bc6, 0x96dcb284ff4c58fc,
    0xa0f47bf651b706c7, 0x6416465c55584aea, 0x19f33c6c41319c91, 0x57dced4b55823012,
    0x40c0a8ee20d14195, 0x214361aa519de08c, 0xec72a959acc4d0c5, 0x5361721773cdeab8,
    0x616fdbf3d303eb12, 0xc5455ed12806ca94, 0x314e09e1bba42948, 0xb81cd1ce26b4234,
    0x6fa8d50b9001fcb2, 0xe0d2d07a9f4777ff, 0xe4084bc1226b850,  0x35bcc17418f75864,
    0x84f0901000e02186, 0x737e80fa21de4510, 0x8a466b0638ddafd8, 0x708864567a5a7a34,
    0xd2ee09132ad7e4b9, 0xf0f7266df6703133, 0x94ed8141c375fba,  0xabbca6fe9a26b384,
    0xa14a30b84e99915d, 0xce42baa9c0750fbc, 0xb9e380f5221caece, 0x83bc73831eb96b2b,
    0x9f23d232ee037b2a, 0x68df5ab2c821df4d, 0xe29414f0370b3575, 0x5bc20a6494f7ea6d,
    0x3e399cbe5bd74a62, 0x4479e1ac2968f74d, 0x81ec814bd0663d43, 0x3192adda9bc5a5d6,
    0x97b5c6c5d27a5d21, 0xd822b80e038ae90e, 0x90a5999d293124c,  0xb28dd743e5f140b,
    0xcb975c22adbd7be1, 0x251f824c363487d7, 0xb4c58eeeb9579193, 0x42778f2995867563,
    0xb07699b7f1465ca9, 0x1a3a6779905639b6, 0x8739924ba15771db, 0x1879aea519bc0549,
    0xf75882563e5094b2, 0x516c7470406c30d4, 0x39c4fdc50a5fb74e, 0xff406db1dd40c74d,
    0x4a5f43528f9dfc4e, 0x216005971949c79c, 0x1ffc1740524fa3ac, 0x81011a2a072c0f3a,
    0x74e792bf035959ae, 0x78ce072f7256ffe6, 0x82ae737053a003ba, 0x7d4e8112fde29fb4,
    0xc83f8a15dc0a5a1b, 0x565d3026a17e89bb, 0x99d82547c7302c70, 0xc0d18a3c12d5171b,
    0xb87377c2be972a0d, 0x811e71375cc1da7f, 0xa47170222848323b, 0xf6490c609fa76a20,
    0xc8c1834ca45852cf, 0xb11bd20948f854e0, 0x83c7242ad13f9222, 0x52408a7b6aa31fb0,
    0xf434270efd257dfc, 0x521919b5756c636d, 0x5602d3b0c149c207, 0x9247d9cc0b1ae4e9,
    0xf146c9b9ed71918f, 0x51bfb3dbc8b01c19, 0xc2fc63f292c8181e, 0xebafcc6626cfcfbc,
    0x91e54633be82299b, 0xb138d61159130eb4, 0xa747c2270c99f7b3, 0xb819feec041392f2,
    0x5e013e4f09d76cc1, 0x4e5044b1dfa0dc05, 0x7042b5aa1ccb2239, 0x7a91d300509cfa63,
    0x6a11d9d65b49ff50, 0xa9042e337501af66, 0x806312bfd39f5792, 0xcdfb71fb779b08b5,
    0x8e4dae66c0021081, 0x60dce88671cc4b48, 0x497b9c4ce29482a7, 0x7c0910820ac74589,
    0xcb3d50e9bf23e9a1, 0x3206dad2c00e60ff, 0xdbebcf7c50609159, 0x13c93728af44b2a9,
    0xe62c23dfc8887494, 0x74c5c7afec8f0fe2, 0xbc7bad651e972220, 0xccbc4cb33e3ad9fc,
    0x739ffa50a9efc5b9, 0x3726c57530be8c96, 0x5f9d1567e89f31e4, 0x2289c18816b1238b,
    0xfb05d535751a114e, 0xbde13d8eb1990b69, 0x3d1b17747e27339a, 0xaf66350ba625e574,
    0x649f1e6341c8b8a7, 0x412c3011ad9241cc, 0x6930926afca9e5a3, 0xd7159a3474a8e772,
    0xbb597014ba85e5c4, 0xf5f72ec78b584f42, 0xd8f23ef3b542f1c9, 0x2979391bafb95f7b,
    0xcb98490f62da9b1d, 0x4ff031b130238ed6, 0x8fb72066e677d67d, 0x5611003bcbbb3fa7,
    0xac916e776d4e2830, 0x2b10607ffd1bc7d9, 0xc25cf7c7504bf803, 0x9b1d4b9854134bfb,
    0x5b94ce7090f75d3a, 0x37043bef3d5a5b03, 0xc8d7c4f046ba2d34, 0xc8f0979f234e26e0,
    0x92f3efe4eb2fafa7, 0x8d0abfaebe8cf915, 0x55739f6ab3ad8be9, 0x8d0caccbfe5d7ee3,
    0x1818bf68546596f,  0x47bf454105448f9a, 0x7512335f128d0ec8, 0x6786b91f842c2d0,
    0xba5d61f89a879985, 0xa3d62117417fea05, 0xe897e59263684e79, 0x2a3b013123c56a96,
    0x82a3ef2b61b8813c, 0x2c17b8863c85f231, 0xa965565bf3158acf, 0x7f766a733fe3aa31,
    0xe1fa0595bef52cda, 0xed3e19e7dc0d6ad8, 0x4bdd84621c739a6f, 0xe0355737571be8ad,
    0x37f75f6a100746a3, 0x584a884dbe8d6a9f, 0x6d1e441fbf01d2cd, 0xd9b43f0198dff50f,
    0xf7b1de7329057c58, 0xbd7b4abfd9cef2fb, 0x9baac302fcb71d07, 0x3377e9efe6511f0b,
    0x937f2a5b4c417ee5, 0xdefae49a75d11236, 0x366dd01edc1e958f, 0x94fffe9a5b0bdde6,
    0x2c67f4bb14c771c7, 0xee41e8837eedd14d, 0x26ca5846173ca3b2, 0x586ac0dcab910415,
    0xae522bf562c538fb, 0x70050c6848b19585, 0x8c0a6f2819a4e923, 0xf448baa4be811b4e,
    0xb52d93ae25be184b, 0x8bcce38348efada3, 0x5cbc03cc41a9087f, 0x3efa34e231046b44,
};

void content_defined_chunking(unsigned char* buffer, unsigned int buff_size, std::vector<size_t>& chunk_positions) {
    uint64_t hash = 0;
    size_t chunk_start = 0;
    size_t i = 0;

    chunk_positions.push_back(0);

    while (i < buff_size) {
        hash = (hash >> 1) + gearTable[buffer[i]];
        i++;
        size_t chunk_size = i - chunk_start;
        if (((hash & (MODULUS - 1)) == TARGET && chunk_size >= MIN_CHUNK_SIZE) || chunk_size >= MAX_CHUNK_SIZE || i == buff_size) {
            chunk_positions.push_back(i);
            hash = 0;
            chunk_start = i;
        }
    }
}
