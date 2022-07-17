/**
 * Copyright 2009 Colin Percival
 * All rights reserved.
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
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */
#include <bitcoin/system/crypto/external/crypto_scrypt.hpp>

#include <bit>
#include <bitcoin/system/crypto/external/pbkdf2_sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

BC_PUSH_WARNING(NO_MALLOC_OR_FREE)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)

namespace libbitcoin {
namespace system {
namespace scrypt {
    
constexpr auto block1 = power2(6u);
constexpr auto block2 = power2(7u);
constexpr auto count = block1 / sizeof(uint32_t);
using integers = std::array<uint32_t, count>;

// TODO: goes away with constexpr R.
constexpr void assign(size_t size, uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < size; ++i)
        to[i] = from[i];
}

// TODO: goes away with constexpr R.
constexpr void xors(size_t size, uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < size; ++i)
        to[i] ^= from[i];
}

template<size_t Size>
constexpr void assign(uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < Size; ++i)
        to[i] = from[i];
}

template<size_t Size>
constexpr void increment(integers& to, const integers& from) NOEXCEPT
{
    for (size_t i = 0; i < Size; ++i)
        to[i] += from[i];
}

template<size_t Size>
constexpr void xors(uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < Size; ++i)
        to[i] ^= from[i];
}

uint8_t* salsa20_8(uint8_t* p) NOEXCEPT
{
    auto words = from_little_endians(unsafe_array_cast<uint32_t, count>(p));
    const integers copy{ words };

    for (size_t i = 0; i < to_half(count); i += two)
    {
        // columns
        words[ 4] ^= std::rotl(words[ 0] + words[12],  7);
        words[ 8] ^= std::rotl(words[ 4] + words[ 0],  9);
        words[12] ^= std::rotl(words[ 8] + words[ 4], 13);
        words[ 0] ^= std::rotl(words[12] + words[ 8], 18);
        words[ 9] ^= std::rotl(words[ 5] + words[ 1],  7);
        words[13] ^= std::rotl(words[ 9] + words[ 5],  9);
        words[ 1] ^= std::rotl(words[13] + words[ 9], 13);
        words[ 5] ^= std::rotl(words[ 1] + words[13], 18);
        words[14] ^= std::rotl(words[10] + words[ 6],  7);
        words[ 2] ^= std::rotl(words[14] + words[10],  9);
        words[ 6] ^= std::rotl(words[ 2] + words[14], 13);
        words[10] ^= std::rotl(words[ 6] + words[ 2], 18);
        words[ 3] ^= std::rotl(words[15] + words[11],  7);
        words[ 7] ^= std::rotl(words[ 3] + words[15],  9);
        words[11] ^= std::rotl(words[ 7] + words[ 3], 13);
        words[15] ^= std::rotl(words[11] + words[ 7], 18);

        // rows
        words[ 1] ^= std::rotl(words[ 0] + words[ 3],  7);
        words[ 2] ^= std::rotl(words[ 1] + words[ 0],  9);
        words[ 3] ^= std::rotl(words[ 2] + words[ 1], 13);
        words[ 0] ^= std::rotl(words[ 3] + words[ 2], 18);
        words[ 6] ^= std::rotl(words[ 5] + words[ 4],  7);
        words[ 7] ^= std::rotl(words[ 6] + words[ 5],  9);
        words[ 4] ^= std::rotl(words[ 7] + words[ 6], 13);
        words[ 5] ^= std::rotl(words[ 4] + words[ 7], 18);
        words[11] ^= std::rotl(words[10] + words[ 9],  7);
        words[ 8] ^= std::rotl(words[11] + words[10],  9);
        words[ 9] ^= std::rotl(words[ 8] + words[11], 13);
        words[10] ^= std::rotl(words[ 9] + words[ 8], 18);
        words[12] ^= std::rotl(words[15] + words[14],  7);
        words[13] ^= std::rotl(words[12] + words[15],  9);
        words[14] ^= std::rotl(words[13] + words[12], 13);
        words[15] ^= std::rotl(words[14] + words[13], 18);
    }

    increment<count>(words, copy);
    to_little_endians(unsafe_array_cast<uint32_t, count>(p), words);
    return p;
}

////template<size_t R>
constexpr uint8_t* get(size_t R,
    uint8_t* p) NOEXCEPT
{
    return &p[(sub1(R << one)) * block1];
}

////template<size_t R>
inline uint64_t get64(size_t R,
    uint8_t* p) NOEXCEPT
{
    return native_from_little_end(unsafe_byte_cast<uint64_t>(get(R, p)));
}

////template<size_t R>
void block_salsa(size_t R,
    uint8_t* p, uint8_t* y) NOEXCEPT
{
    data_array<block1> X{ unsafe_array_cast<uint8_t, block1>(get(R, p)) };

    for (size_t i = 0; i < (R << 1); ++i)
    {
        xors<block1>(X.data(), &p[i * block1]);
        assign<block1>(&y[i * block1], salsa20_8(X.data()));
    }

    for (size_t i = 0; i < (R << 0); ++i)
    {
        assign<block1>(&p[(i + 0) * block1], &y[((i << one) + 0) * block1]);
        assign<block1>(&p[(i + R) * block1], &y[((i << one) + 1) * block1]);
    }
}

////template<size_t W, size_t R>
void mix(size_t W, size_t R,
    uint8_t* p, uint8_t* w, uint8_t* x) NOEXCEPT
{
    // There is no need to double x (xy) for working space, just overload p.
    // TODO: with r128 constexpr use assign/xors templates and use data_array.
    const auto r128 = R * block2;
    ////data_array<r128> x;

    for (size_t i = 0; i < W; ++i)
    {
        assign(r128, &w[i * r128], p);
        block_salsa(R, p, x);
    }

    for (size_t i = 0; i < W; ++i)
    {
        xors(r128, p, &w[(get64(R, p) % W) * r128]);
        block_salsa(R, p, x);
    }
}

////template<size_t W, uint32_t R, uint32_t P>
bool hash(size_t W, size_t R, size_t P,
    const uint8_t* phrase, size_t phrase_size, const uint8_t* salt,
    size_t salt_size, uint8_t* buffer, size_t size) NOEXCEPT
{
    // Validate parameters (move to template contraints for constexpr eval).
    // ------------------------------------------------------------------------

    // [W]ork must be a power of 2 greater than 1.
    if (!(W > one) || !(power2(floored_log2(W)) == W))
        return false;

    // [R]esources and [P]arallelism must satisfy [(R * P) < 2^30].
    if (is_multiply_overflow(R, P) || !((R * P) < power2<uint32_t>(30u)))
        return false;

    // buffer size must not exceed pbkdf2::sha256 limit of [sub1(2^32) * 2^5].
    if (size > sub1(power2<uint64_t>(32u)) * power2<uint64_t>(5u))
        return false;

    const auto r128 = R * block2;

    uint8_t* p;
    uint8_t* w;
    uint8_t* x;

    // TODO: with P, W, R constexpr use data_array (stack allocation).
    ////data_array<P * r128> p;
    ////data_array<W * r128> w;

    if (is_null((p = pointer_cast<uint8_t>(std::malloc(P * r128)))))
    {
        return false;
    }

    if (is_null((w = pointer_cast<uint8_t>(std::malloc(W * r128)))))
    {
        std::free(p);
        return false;
    }

    // There is no need to double this (XY) for working space.
    if (is_null((x = pointer_cast<uint8_t>(std::malloc(r128)))))
    {
        std::free(w);
        std::free(p);
        return false;
    }

    pbkdf2::sha256(phrase, phrase_size, salt, salt_size, one, p, P * r128);

    for (size_t i = 0; i < P; ++i)
        mix(W, R, &p[i * r128], w, x);

    pbkdf2::sha256(phrase, phrase_size, p, P * r128, one, buffer, size);

    std::free(x);
    std::free(w);
    std::free(p);
    return true;
}

} // namespace scrypt
} // namespace system
} // namespace libbitcoin

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
