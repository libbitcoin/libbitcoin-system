/* libsodium: hash_sha256_.c, v0.4.5 2014/04/16 */
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
#include <bitcoin/system/crypto/intrinsics/intrinsics.hpp>

#include <algorithm>
#include <array>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

 // C-style functions, all usage verified.
BC_PUSH_WARNING(USE_GSL_AT)
BC_PUSH_WARNING(USE_NOT_NULL)
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

namespace libbitcoin {
namespace system {
namespace intrinsics {

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

void sha256_x1_portable(uint32_t state[8], const uint8_t block[64]) NOEXCEPT
{
    constexpr auto count = 64_size / sizeof(uint32_t);
    std::array<uint32_t, 64> W;

    // uint32_t<64> to uint32_t<16> (narrowing array cast)
    // uint8_t[64]  as uint32_t<16> (unsafe array cast)
    to_big_endian(
        narrowing_array_cast<uint32_t, count>(W),
        unsafe_array_cast<uint32_t, count>(&block[0]));

    uint32_t t0, t1;
    std::array<uint32_t, 8> S
    {
        state[0],
        state[1],
        state[2],
        state[3],
        state[4],
        state[5],
        state[6],
        state[7]
    };

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

// This calls single_sha256, which may call any of the instrinsic transforms or
// may call sha256_x1_portable (above), depending on platform configuration.
void sha256_update(sha256_context& context, const uint8_t input[],
    size_t size) NOEXCEPT
{
    constexpr auto block_size = 64_size;
    const std::array<uint32_t, 2> bit_length
    {
        possible_narrow_cast<uint32_t>(size >> 29u),
        possible_narrow_cast<uint32_t>(size << 3u)
    };

    const auto r = (context.count[1] >> 3u) & 0x3f;

    if (((context.count[1] += bit_length[1])) < bit_length[1])
        context.count[0]++;

    context.count[0] += bit_length[0];

    if (size < (block_size - r))
    {
        std::copy_n(input, size, &context.buffer[r]);
        return;
    }

    std::copy_n(input, block_size - r, &context.buffer[r]);
    sha256_single(context.state.data(), &context.buffer[0]);

    input += (block_size - r);
    size -= (block_size - r);

    while (size >= block_size)
    {
        sha256_single(context.state.data(), input);
        input += block_size;
        size -= block_size;
    }

    std::copy_n(input, size, &context.buffer[0]);
}

void sha256_pad(sha256_context& context) NOEXCEPT
{
    const auto size = to_big_endian(context.count);
    constexpr auto count = size.size() * sizeof(uint32_t);

    uint32_t r, psize;
    r = (context.count[1] >> 3_size) & 0x3f;
    psize = (r < 56) ? (56 - r) : (120 - r);

    sha256_update(context, sha256_padding.data(), psize);
    sha256_update(context, array_cast<uint8_t>(size).data(), count);
}

void sha256_finalize(sha256_context& context, uint8_t digest[32]) NOEXCEPT
{
    constexpr auto count = 32_size / sizeof(uint32_t);

    sha256_pad(context);
    to_big_endian(unsafe_array_cast<uint32_t, count>(&digest[0]), context.state);
}

void sha256(const uint8_t input[], size_t size, uint8_t digest[32]) NOEXCEPT
{
    sha256_context context{};
    sha256_update(context, input, size);
    sha256_finalize(context, digest);
}

} // namespace intrinsics
} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()