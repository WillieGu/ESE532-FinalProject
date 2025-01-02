// #include "sha256.h"

// #include <arm_neon.h>
// #include <cstddef>
// #include <cstdint>
// #include <cstring>
// #include <vector>
// #include <thread>


// // SHA256 Constants
// static const uint32_t k[64] = {
//     // First 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311
//     0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
//     0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
//     0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
//     0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
//     0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
//     0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
//     0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
//     0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
//     0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
//     0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
//     0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
//     0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
//     0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
//     0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
//     0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
//     0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
// };

// // Helper function for right rotation
// static inline uint32_t right_rotate(uint32_t value, unsigned int count) {
//     return (value >> count) | (value << (32 - count));
// }
// // Define a function for rotate right
// static inline uint32x4_t vrorq_n_u32(uint32x4_t val, int n) {
//     return vorrq_u32(vshrq_n_u32(val, n), vshlq_n_u32(val, 32 - n));
// }
// // NEON-optimized SHA256 transform function
// void sha256_transform_neon(SHA256_CTX *ctx, const BYTE data[]) {
//     uint32_t a, b, c, d, e, f, g, h;
//     uint32x4_t t1, t2;
//     uint32x4_t W[16];
//     uint32_t Wt;
//     int i, j;

//     // Prepare the message schedule W[0..63]
//     for (i = 0; i < 16; ++i) {
//         // Load 4 bytes and convert to big-endian
//         W[i] = vld1q_u32((uint32_t *)(data + i * 4));
//         W[i] = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(W[i])));
//     }

//     // Initialize working variables
//     a = ctx->state[0];
//     b = ctx->state[1];
//     c = ctx->state[2];
//     d = ctx->state[3];
//     e = ctx->state[4];
//     f = ctx->state[5];
//     g = ctx->state[6];
//     h = ctx->state[7];

//     // Compression function main loop
//     for (i = 0; i < 64; ++i) {
//         // Prepare the message schedule
//         if (i >= 16) {
//             uint32x4_t s0, s1;
//             s0 = veorq_u32(vrorq_n_u32(W[(i - 15) % 16], 7), vrorq_n_u32(W[(i - 15) % 16], 18));
//             s0 = veorq_u32(s0, vshrq_n_u32(W[(i - 15) % 16], 3));

//             s1 = veorq_u32(vrorq_n_u32(W[(i - 2) % 16], 17), vrorq_n_u32(W[(i - 2) % 16], 19));
//             s1 = veorq_u32(s1, vshrq_n_u32(W[(i - 2) % 16], 10));

//             W[i % 16] = vaddq_u32(vaddq_u32(vaddq_u32(W[(i - 16) % 16], s0), W[(i - 7) % 16]), s1);
//         }

//         // Calculate T1 and T2
//         uint32_t S1 = right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
//         uint32_t ch = (e & f) ^ (~e & g);
//         uint32_t temp1 = h + S1 + ch + k[i] + vgetq_lane_u32(W[i % 16], 0);

//         uint32_t S0 = right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
//         uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
//         uint32_t temp2 = S0 + maj;

//         // Update working variables
//         h = g;
//         g = f;
//         f = e;
//         e = d + temp1;
//         d = c;
//         c = b;
//         b = a;
//         a = temp1 + temp2;
//     }

//     // Add the compressed chunk to the current hash value
//     ctx->state[0] += a;
//     ctx->state[1] += b;
//     ctx->state[2] += c;
//     ctx->state[3] += d;
//     ctx->state[4] += e;
//     ctx->state[5] += f;
//     ctx->state[6] += g;
//     ctx->state[7] += h;
// }

// void sha256_init(SHA256_CTX *ctx) {
//     ctx->datalen = 0;
//     ctx->bitlen = 0;
//     ctx->state[0] = 0x6a09e667;
//     ctx->state[1] = 0xbb67ae85;
//     ctx->state[2] = 0x3c6ef372;
//     ctx->state[3] = 0xa54ff53a;
//     ctx->state[4] = 0x510e527f;
//     ctx->state[5] = 0x9b05688c;
//     ctx->state[6] = 0x1f83d9ab;
//     ctx->state[7] = 0x5be0cd19;
// }

// void sha256_hash_chunk_neon(const BYTE* data, size_t len, BYTE* hash_output) {
//     SHA256_CTX ctx;
//     sha256_init(&ctx);

//     size_t full_blocks = len / 64;
//     size_t remaining_bytes = len % 64;

//     // Process all full 64-byte blocks
//     for (size_t i = 0; i < full_blocks; ++i) {
//         sha256_transform_neon(&ctx, data + i * 64);
//     }

//     // Buffer to hold remaining data and padding (maximum of two blocks)
//     BYTE buffer[128];
//     size_t buffer_len = 0;

//     // Copy remaining data to buffer
//     if (remaining_bytes > 0) {
//         memcpy(buffer, data + full_blocks * 64, remaining_bytes);
//         buffer_len = remaining_bytes;
//     }

//     // Append the '1' bit (0x80)
//     buffer[buffer_len++] = 0x80;

//     // Calculate the length of padding needed
//     size_t pad_len = (buffer_len > 56) ? (120 - buffer_len) : (56 - buffer_len);

//     // Zero padding
//     memset(buffer + buffer_len, 0, pad_len);
//     buffer_len += pad_len;

//     // Append the message length in bits as a 64-bit big-endian integer
//     uint64_t bit_len = len * 8;
//     bit_len = __builtin_bswap64(bit_len); // Convert to big-endian
//     memcpy(buffer + buffer_len, &bit_len, sizeof(bit_len));
//     buffer_len += sizeof(bit_len);

//     // Process the final block(s)
//     size_t total_blocks = buffer_len / 64;
//     for (size_t i = 0; i < total_blocks; ++i) {
//         sha256_transform_neon(&ctx, buffer + i * 64);
//     }

//     // Produce the final hash value in big-endian format
//     for (int i = 0; i < 8; ++i) {
//         uint32_t be_state = __builtin_bswap32(ctx.state[i]);
//         memcpy(hash_output + i * 4, &be_state, sizeof(uint32_t));
//     }
// }

// void sha256_compute_parallel(const BYTE* data, size_t total_len, BYTE* hash_output) {
//     // For simplicity, we can run sha256_hash_chunk_neon directly
//     sha256_hash_chunk_neon(data, total_len, hash_output);
// }
#include <stdlib.h>
#include <memory.h>
#include <arm_neon.h>
#include "sha256.h"

// #define ROLLED_UP_METHOD

static const WORD k[64] = {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
    0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
    0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
    0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
    0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
    0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
    0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
    0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

/****************************** TYPES *******************************/
struct hash_s {
    uint32x4_t abcd, efgh;
};
typedef struct hash_s hash_state_t;

/*********************** FUNCTION DEFINITIONS ***********************/
void sha256_transform(SHA256_CTX *ctx, const BYTE data[])
{
    WORD i;
    uint32x4_t round_input;
    hash_state_t cur_state, old_state;
#ifdef ROLLED_UP_METHOD
    uint32x4_t sched[4];

    // load state
    cur_state.abcd = vld1q_u32(ctx->state);
    cur_state.efgh = vld1q_u32(ctx->state+4);

    // make the schedule
    for (i = 0; i < 4; i++) {
        // reverses the byte ordering
        sched[i] = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 16*i)));
        round_input = vaddq_u32(sched[i], vld1q_u32(k + i*4));
        old_state.abcd = cur_state.abcd;
        old_state.efgh = cur_state.efgh;
        cur_state.abcd = vsha256hq_u32 (old_state.abcd, old_state.efgh, round_input);
        cur_state.efgh = vsha256h2q_u32(old_state.efgh, old_state.abcd, round_input);
    }
    for (i = 4; i < 16; i++) {
        sched[i%4] = vsha256su1q_u32(vsha256su0q_u32(
                    sched[i%4], sched[(i+1)%4]),
                    sched[(i+2)%4], sched[(i+3)%4]);
        round_input = vaddq_u32(sched[i%4], vld1q_u32(k + i*4));
        old_state.abcd = cur_state.abcd;
        old_state.efgh = cur_state.efgh;
        cur_state.abcd = vsha256hq_u32 (old_state.abcd, old_state.efgh, round_input);
        cur_state.efgh = vsha256h2q_u32(old_state.efgh, old_state.abcd, round_input);
    }
#else
    uint32x4_t sched[16];

    // make the schedule
    for (i = 0; i < 4; i++) {
        // reverses the byte ordering
        sched[i] = vreinterpretq_u32_u8(vrev32q_u8(vld1q_u8(data + 16*i)));
    }
    for (i = 4; i < 16; i++) {
        sched[i] = vsha256su1q_u32(vsha256su0q_u32(
                sched[i-4], sched[i-3]), sched[i-2], sched[i-1]);
    }

    // load state
    cur_state.abcd = vld1q_u32(ctx->state);
    cur_state.efgh = vld1q_u32(ctx->state+4);

    // do the hashing
    for (i = 0; i < 16; i++) {
        round_input = vaddq_u32(sched[i], vld1q_u32(k + i*4));
        old_state.abcd = cur_state.abcd;
        old_state.efgh = cur_state.efgh;
        cur_state.abcd = vsha256hq_u32 (old_state.abcd, old_state.efgh, round_input);
        cur_state.efgh = vsha256h2q_u32(old_state.efgh, old_state.abcd, round_input);
    }
#endif

    // add in the state
    vst1q_u32(ctx->state, vaddq_u32(cur_state.abcd, vld1q_u32(ctx->state)));
    vst1q_u32(ctx->state+4, vaddq_u32(cur_state.efgh, vld1q_u32(ctx->state+4)));
}

void sha256_init(SHA256_CTX *ctx)
{
    ctx->datalen = 0;
    ctx->bitlen = 0;
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
    WORD i;

    for (i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i];
        ctx->datalen++;
        if (ctx->datalen == 64) {
            sha256_transform(ctx, ctx->data);
            ctx->bitlen += 512;
            ctx->datalen = 0;
        }
    }
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
    WORD i;

    i = ctx->datalen;

    // Pad whatever data is left in the buffer.
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80;
        while (i < 56)
            ctx->data[i++] = 0x00;
    }
    else {
        ctx->data[i++] = 0x80;
        while (i < 64)
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data);
        memset(ctx->data, 0, 56);
    }

    // Append to the padding the total message length in bits and transform.
    ctx->bitlen += (uint64_t)ctx->datalen * 8;
    ctx->data[63] = (BYTE)(ctx->bitlen);
    ctx->data[62] = (BYTE)(ctx->bitlen >> 8);
    ctx->data[61] = (BYTE)(ctx->bitlen >> 16);
    ctx->data[60] = (BYTE)(ctx->bitlen >> 24);
    ctx->data[59] = (BYTE)(ctx->bitlen >> 32);
    ctx->data[58] = (BYTE)(ctx->bitlen >> 40);
    ctx->data[57] = (BYTE)(ctx->bitlen >> 48);
    ctx->data[56] = (BYTE)(ctx->bitlen >> 56);
    sha256_transform(ctx, ctx->data);

    // Since this implementation uses little endian byte ordering and SHA uses big endian,
    // reverse all the bytes when copying the final state to the output hash.
    for (i = 0; i < 8; i += 4) {
        vst1q_u8(hash + (i * 4),
                 vrev32q_u8(vld1q_u8((BYTE*)&ctx->state[i])));
    }
}

/*
 * sha256_hash:
 * data    - input message
 * hash    - where to put the output (32 bytes)
 * runLen  - length of the data in bytes to be hashed once (or number of times you want to update)
 *
 * NOTE: Previously, this tried to use strlen(data) which is incorrect for binary data.
 * We now rely on runLen as the actual length of the data.
 */
void sha256_hash(SHA256_CTX* ctx, const BYTE data[], BYTE hash[], size_t dataLen)
{
    sha256_init(ctx);
    sha256_update(ctx, data, dataLen);
    sha256_final(ctx, hash);
}

/*
 * sha256_hash_file:
 * data    - input message
 * hash    - where to put the output (32 bytes)
 * dataLen - length of data in bytes
 */
void sha256_hash_file(SHA256_CTX* ctx, const BYTE data[], BYTE hash[], size_t dataLen)
{
    sha256_init(ctx);
    sha256_update(ctx, data, dataLen);
    sha256_final(ctx, hash);
}