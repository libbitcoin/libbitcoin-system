/* libsodium: hash_sha256.c, v0.4.5 2014/04/16 */
/**
 * Copyright 2005,2007,2009 Colin Percival. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "../../../include/bitcoin/system/crypto/external/sha256.h"

#include <stdint.h>
#include <string.h>

// TODO: make constexpr (cpp) and use /math/bytes endians.

// from_big_end
static uint32_t from_big_endian(const void* data)
{
    const uint8_t* byte = (uint8_t const*)data;

    return
        (((uint32_t)byte[3]) <<  0) |
        (((uint32_t)byte[2]) <<  8) |
        (((uint32_t)byte[1]) << 16) |
        (((uint32_t)byte[0]) << 24);
}

// to_big_end
static void to_big_endian(void* data, uint32_t value)
{
    uint8_t* byte = (uint8_t*)data;

    byte[3] = (value >>  0) & 0xff;
    byte[2] = (value >>  8) & 0xff;
    byte[1] = (value >> 16) & 0xff;
    byte[0] = (value >> 24) & 0xff;
}

static void from_big_endian_vector(uint32_t* to, const uint8_t* from,
    size_t size)
{
    size_t i;
    for (i = 0; i < size / sizeof(uint32_t); i++)
    {
        to[i] = from_big_endian(from + i * sizeof(uint32_t));
    }
}

static void to_big_endian_vector(uint8_t* to, const uint32_t* from,
 size_t size)
{
    size_t i;
    for (i = 0; i < size / sizeof(uint32_t); i++)
    {
        to_big_endian(to + i * sizeof(uint32_t), from[i]);
    }
}

#define Ch(x, y, z)  ((x & (y ^ z)) ^ z)
#define Maj(x, y, z) ((x & (y | z)) | (y & z))
#define SHR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << (32 - n)))
#define S0(x)        (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)        (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)        (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)        (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

#define RND(a, b, c, d, e, f, g, h, k) \
    t0 = h + S1(e) + Ch(e, f, g) + k;  \
    t1 = S0(a) + Maj(a, b, c); \
    d += t0; \
    h = t0 + t1;

#define RNDr(S, W, i, k) \
    RND(S[(64 - i) % 8], S[(65 - i) % 8], \
    S[(66 - i) % 8], S[(67 - i) % 8], \
    S[(68 - i) % 8], S[(69 - i) % 8], \
    S[(70 - i) % 8], S[(71 - i) % 8], \
    W[i] + k)

#define Wi(W, i) \
    W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16]

static unsigned char PAD[SHA256_BLOCK_LENGTH] =
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void SHA256Pad(SHA256CTX* context);
void SHA256Transform(uint32_t state[SHA256_STATE_LENGTH],
    const uint8_t block[SHA256_BLOCK_LENGTH]);

void SHA256(const uint8_t* input, size_t length,
    uint8_t digest[SHA256_DIGEST_LENGTH])
{
    SHA256CTX context;
    SHA256Init(&context);
    SHA256Update(&context, input, length);
    SHA256Final(&context, digest);
}

void SHA256Init(SHA256CTX* context)
{
    context->count[0] = 0;
    context->count[1] = 0;
    context->state[0] = 0x6A09E667;
    context->state[1] = 0xBB67AE85;
    context->state[2] = 0x3C6EF372;
    context->state[3] = 0xA54FF53A;
    context->state[4] = 0x510E527F;
    context->state[5] = 0x9B05688C;
    context->state[6] = 0x1F83D9AB;
    context->state[7] = 0x5BE0CD19;
}

void SHA256Update(SHA256CTX* context, const uint8_t* input, size_t length)
{
    uint32_t bitlen[2];
    uint32_t r = (context->count[1] >> 3) & 0x3f;

    bitlen[1] = ((uint32_t)length) << 3;
    bitlen[0] = (uint32_t)(length >> 29);

    if ((context->count[1] += bitlen[1]) < bitlen[1])
    {
        context->count[0]++;
    }

    context->count[0] += bitlen[0];

    if (length < SHA256_BLOCK_LENGTH - r)
    {
        memcpy(&context->buffer[r], input, length);
        return;
    }

    memcpy(&context->buffer[r], input, SHA256_BLOCK_LENGTH - r);
    SHA256Transform(context->state, context->buffer);

    input += SHA256_BLOCK_LENGTH - r;
    length -= SHA256_BLOCK_LENGTH - r;

    while (length >= SHA256_BLOCK_LENGTH)
    {
        SHA256Transform(context->state, input);
        input += SHA256_BLOCK_LENGTH;
        length -= SHA256_BLOCK_LENGTH;
    }

    memcpy(context->buffer, input, length);
}

void SHA256Final(SHA256CTX* context, uint8_t digest[SHA256_DIGEST_LENGTH])
{
    SHA256Pad(context);
    to_big_endian_vector(digest, context->state, SHA256_DIGEST_LENGTH);
}

void SHA256Pad(SHA256CTX* context)
{
    uint8_t len[SHA256_STATE_LENGTH];
    uint32_t r, plen;

    to_big_endian_vector(len, context->count, SHA256_STATE_LENGTH);

    r = (context->count[1] >> 3) & 0x3f;
    plen = (r < 56) ? (56 - r) : (120 - r);

    SHA256Update(context, PAD, plen);
    SHA256Update(context, len, SHA256_STATE_LENGTH);
}

void SHA256Transform(uint32_t state[SHA256_STATE_LENGTH],
    const uint8_t block[SHA256_BLOCK_LENGTH])
{
    uint32_t t0, t1;
    uint32_t W[SHA256_BLOCK_LENGTH];
    uint32_t S[SHA256_STATE_LENGTH];

    from_big_endian_vector(W, block, SHA256_BLOCK_LENGTH);

    Wi(W, 16);
    Wi(W, 17);
    Wi(W, 18);
    Wi(W, 19);
    Wi(W, 20);
    Wi(W, 21);
    Wi(W, 22);
    Wi(W, 23);
    Wi(W, 24);
    Wi(W, 25);
    Wi(W, 26);
    Wi(W, 27);
    Wi(W, 28);
    Wi(W, 29);
    Wi(W, 30);
    Wi(W, 31);
    Wi(W, 32);
    Wi(W, 33);
    Wi(W, 34);
    Wi(W, 35);
    Wi(W, 36);
    Wi(W, 37);
    Wi(W, 38);
    Wi(W, 39);
    Wi(W, 40);
    Wi(W, 41);
    Wi(W, 42);
    Wi(W, 43);
    Wi(W, 44);
    Wi(W, 45);
    Wi(W, 46);
    Wi(W, 47);
    Wi(W, 48);
    Wi(W, 49);
    Wi(W, 50);
    Wi(W, 51);
    Wi(W, 52);
    Wi(W, 53);
    Wi(W, 54);
    Wi(W, 55);
    Wi(W, 56);
    Wi(W, 57);
    Wi(W, 58);
    Wi(W, 59);
    Wi(W, 60);
    Wi(W, 61);
    Wi(W, 62);
    Wi(W, 63);

    S[0] = state[0];
    S[1] = state[1];
    S[2] = state[2];
    S[3] = state[3];
    S[4] = state[4];
    S[5] = state[5];
    S[6] = state[6];
    S[7] = state[7];

    RNDr(S, W, 0, 0x428a2f98);
    RNDr(S, W, 1, 0x71374491);
    RNDr(S, W, 2, 0xb5c0fbcf);
    RNDr(S, W, 3, 0xe9b5dba5);
    RNDr(S, W, 4, 0x3956c25b);
    RNDr(S, W, 5, 0x59f111f1);
    RNDr(S, W, 6, 0x923f82a4);
    RNDr(S, W, 7, 0xab1c5ed5);
    RNDr(S, W, 8, 0xd807aa98);
    RNDr(S, W, 9, 0x12835b01);
    RNDr(S, W, 10, 0x243185be);
    RNDr(S, W, 11, 0x550c7dc3);
    RNDr(S, W, 12, 0x72be5d74);
    RNDr(S, W, 13, 0x80deb1fe);
    RNDr(S, W, 14, 0x9bdc06a7);
    RNDr(S, W, 15, 0xc19bf174);
    RNDr(S, W, 16, 0xe49b69c1);
    RNDr(S, W, 17, 0xefbe4786);
    RNDr(S, W, 18, 0x0fc19dc6);
    RNDr(S, W, 19, 0x240ca1cc);
    RNDr(S, W, 20, 0x2de92c6f);
    RNDr(S, W, 21, 0x4a7484aa);
    RNDr(S, W, 22, 0x5cb0a9dc);
    RNDr(S, W, 23, 0x76f988da);
    RNDr(S, W, 24, 0x983e5152);
    RNDr(S, W, 25, 0xa831c66d);
    RNDr(S, W, 26, 0xb00327c8);
    RNDr(S, W, 27, 0xbf597fc7);
    RNDr(S, W, 28, 0xc6e00bf3);
    RNDr(S, W, 29, 0xd5a79147);
    RNDr(S, W, 30, 0x06ca6351);
    RNDr(S, W, 31, 0x14292967);
    RNDr(S, W, 32, 0x27b70a85);
    RNDr(S, W, 33, 0x2e1b2138);
    RNDr(S, W, 34, 0x4d2c6dfc);
    RNDr(S, W, 35, 0x53380d13);
    RNDr(S, W, 36, 0x650a7354);
    RNDr(S, W, 37, 0x766a0abb);
    RNDr(S, W, 38, 0x81c2c92e);
    RNDr(S, W, 39, 0x92722c85);
    RNDr(S, W, 40, 0xa2bfe8a1);
    RNDr(S, W, 41, 0xa81a664b);
    RNDr(S, W, 42, 0xc24b8b70);
    RNDr(S, W, 43, 0xc76c51a3);
    RNDr(S, W, 44, 0xd192e819);
    RNDr(S, W, 45, 0xd6990624);
    RNDr(S, W, 46, 0xf40e3585);
    RNDr(S, W, 47, 0x106aa070);
    RNDr(S, W, 48, 0x19a4c116);
    RNDr(S, W, 49, 0x1e376c08);
    RNDr(S, W, 50, 0x2748774c);
    RNDr(S, W, 51, 0x34b0bcb5);
    RNDr(S, W, 52, 0x391c0cb3);
    RNDr(S, W, 53, 0x4ed8aa4a);
    RNDr(S, W, 54, 0x5b9cca4f);
    RNDr(S, W, 55, 0x682e6ff3);
    RNDr(S, W, 56, 0x748f82ee);
    RNDr(S, W, 57, 0x78a5636f);
    RNDr(S, W, 58, 0x84c87814);
    RNDr(S, W, 59, 0x8cc70208);
    RNDr(S, W, 60, 0x90befffa);
    RNDr(S, W, 61, 0xa4506ceb);
    RNDr(S, W, 62, 0xbef9a3f7);
    RNDr(S, W, 63, 0xc67178f2);

    state[0] += S[0];
    state[1] += S[1];
    state[2] += S[2];
    state[3] += S[3];
    state[4] += S[4];
    state[5] += S[5];
    state[6] += S[6];
    state[7] += S[7];
}
