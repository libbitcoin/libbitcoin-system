/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_IPP

#include <bit>
#include <bitcoin/system/crypto/pbkdf2_sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace scrypt {
    
constexpr auto block1 = power2(6u);
constexpr auto block2 = power2(7u);
constexpr auto count = block1 / sizeof(uint32_t);
using integers = std::array<uint32_t, count>;

BC_PUSH_WARNING(NO_MALLOC_OR_FREE)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)

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

inline uint8_t* salsa20_8(uint8_t* p) NOEXCEPT
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

template<size_t R>
constexpr uint8_t* get(uint8_t* p) NOEXCEPT
{
    return &p[(sub1(R << one)) * block1];
}

template<size_t R>
inline uint64_t get64(uint8_t* p) NOEXCEPT
{
    return native_from_little_end(unsafe_byte_cast<uint64_t>(get<R>(p)));
}

template<size_t R>
inline void block_salsa(uint8_t* p) NOEXCEPT
{
    constexpr auto r128 = R * block2;
    data_array<block1> y{ unsafe_array_cast<uint8_t, block1>(get<R>(p)) };

    // No need for double working space (xy), just overload p.
    data_array<r128> x;

    for (size_t i = 0; i < (R << 1); ++i)
    {
        xors<block1>(y.data(), &p[i * block1]);
        assign<block1>(&x[i * block1], salsa20_8(y.data()));
    }

    for (size_t i = 0; i < (R << 0); ++i)
    {
        assign<block1>(&p[(i + 0) * block1], &x[((i << one) + 0) * block1]);
        assign<block1>(&p[(i + R) * block1], &x[((i << one) + 1) * block1]);
    }
}

template<size_t W, size_t R>
inline void mix(uint8_t* p) NOEXCEPT
{
    constexpr auto r128 = R * block2;
    data_array<W * r128> w;

    for (size_t i = 0; i < W; ++i)
    {
        assign<r128>(&w[i * r128], p);
        block_salsa<R>(p);
    }

    for (size_t i = 0; i < W; ++i)
    {
        xors<r128>(p, &w[(get64<R>(p) % W) * r128]);
        block_salsa<R>(p);
    }
}

template<size_t W, size_t R, size_t P,
    bool_if<
        ((W > one) && (power2(floored_log2(W)) == W)) &&
        (safe_multiply(R, P) < power2<uint32_t>(30u))>>
inline bool hash(const data_slice& phrase, const data_slice& salt,
    uint8_t* buffer, size_t size) NOEXCEPT
{
    // TODO: move this guard into pbkdf2::sha256.
    if (size > sub1(power2<uint64_t>(32u)) * power2<uint64_t>(5u))
        return false;

    constexpr auto r128 = R * block2;
    data_array<P * r128> p;

    pbkdf2::sha256(phrase.data(), phrase.size(),
        salt.data(), salt.size(), one, p.data(), P * r128);

    for (size_t i = 0; i < P; ++i)
        mix<W, R>(&p[i * r128]);

    pbkdf2::sha256(phrase.data(), phrase.size(),
        p.data(), P * r128, one, buffer, size);

    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace scrypt
} // namespace system
} // namespace libbitcoin

#endif
