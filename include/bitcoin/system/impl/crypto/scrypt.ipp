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

#include <atomic>
#include <algorithm>
#include <bit>
#include <cstdlib>
#include <execution>
#include <memory>
#include <bitcoin/system/crypto/pbkdf2_sha256.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)

#define TEMPLATE \
template<size_t W, size_t R, size_t P, bool Concurrent, \
    bool_if<is_scrypt_args<W, R, P>> If>

TEMPLATE
constexpr auto scrypt<W, R, P, Concurrent, If>::
parallel() NOEXCEPT
{
    if constexpr (Concurrent)
        return std::execution::par_unseq;
    else
        return std::execution::seq;
}

TEMPLATE
template<typename Block>
inline auto scrypt<W, R, P, Concurrent, If>::
allocate() NOEXCEPT
{
    return std::shared_ptr<Block>(new Block);
}

TEMPLATE
template<size_t Size>
constexpr void scrypt<W, R, P, Concurrent, If>::
set(uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < Size; ++i)
        to[i] = from[i];
}

TEMPLATE
template<size_t Size>
constexpr void scrypt<W, R, P, Concurrent, If>::
exc(uint8_t* to, const uint8_t* from) NOEXCEPT
{
    for (size_t i = 0; i < Size; ++i)
        to[i] ^= from[i];
}

TEMPLATE
constexpr void scrypt<W, R, P, Concurrent, If>::
add(integers& to, const integers& from) NOEXCEPT
{
    to[0]  += from[0];
    to[1]  += from[1];
    to[2]  += from[2];
    to[3]  += from[3];
    to[4]  += from[4];
    to[5]  += from[5];
    to[6]  += from[6];
    to[7]  += from[7];
    to[8]  += from[8];
    to[9]  += from[9];
    to[10] += from[10];
    to[11] += from[11];
    to[12] += from[12];
    to[13] += from[13];
    to[14] += from[14];
    to[15] += from[15];
}

TEMPLATE
constexpr size_t scrypt<W, R, P, Concurrent, If>::
offset(auto a, auto b, auto c) NOEXCEPT
{
    return ((a << b) + c) * block_size;
};

TEMPLATE
constexpr uint8_t* scrypt<W, R, P, Concurrent, If>::
get(uint8_t* p) NOEXCEPT
{
    return &p[offset(R, 1, -1)];
}

TEMPLATE
inline uint64_t scrypt<W, R, P, Concurrent, If>::
get64(uint8_t* p) NOEXCEPT
{
    return native_from_little_end(unsafe_byte_cast<uint64_t>(get(p)));
}

TEMPLATE
scrypt<W, R, P, Concurrent, If>::block_type& scrypt<W, R, P, Concurrent, If>::
salsa_1(block_type& block) NOEXCEPT
{
    // [2 x 64] bytes stack allocated for each P (may be concurrent).
    auto words = from_little_endians(array_cast<uint32_t>(block));
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

    add(words, copy);
    to_little_endians(array_cast<uint32_t>(block), words);
    return block;
}

TEMPLATE
bool scrypt<W, R, P, Concurrent, If>::
salsa_p(uint8_t* p) NOEXCEPT
{
    // [(R * 128) + 64] bytes stack allocated for each P (may be concurrent).
    block_type block{ unsafe_array_cast<uint8_t, block_size>(get(p)) };
    const auto x_ptr = allocate<rblock_type>();
    if (!x_ptr) return false;
    auto& x = *x_ptr;

    for (size_t i = 0; i < (R << 1); ++i)
    {
        exc<block_size>(block.data(), &p[offset(i, 0, 0)]);
        set<block_size>(&x[offset(i, 0, 0)], salsa_1(block).data());
    }

    for (size_t i = 0; i < (R << 0); ++i)
    {
        set<block_size>(&p[offset(i, 0, 0)], &x[offset(i, 1, 0)]);
        set<block_size>(&p[offset(i, 0, R)], &x[offset(i, 1, 1)]);
    }

    return true;
}

TEMPLATE
bool scrypt<W, R, P, Concurrent, If>::
mix(uint8_t* p) NOEXCEPT
{
    // [W * (R * 128)] bytes heap allocated for each P (may be concurrent).
    const auto w_ptr = allocate<wblock_type>();
    if (!w_ptr) return false;
    auto& w = *w_ptr;

    for (size_t i = 0; i < W; ++i)
    {
        const auto offset = i * rblock;
        set<rblock>(&w[offset], p);
        if (!salsa_p(p)) return false;
    }

    for (size_t i = 0; i < W; ++i)
    {
        const auto offset = (get64(p) % W) * rblock;
        exc<rblock>(p, &w[offset]);
        if (!salsa_p(p)) return false;
    }

    return true;
}

TEMPLATE
bool scrypt<W, R, P, Concurrent, If>::
hash(const data_slice& phrase, const data_slice& salt, uint8_t* buffer,
    size_t size) NOEXCEPT
{
    // First pbkdf2 cannot fail because [P * rblock] is guarded to be less
    // than power2<uint32_t>(30u) which is less than pbkdf2::maximum_size.
    static_assert(power2<uint32_t>(30u) < pbkdf2::maximum_size);

    // [P * (R * 128)] bytes heap allocated.
    const auto p_ptr = allocate<pblock_type>();
    if (!p_ptr) return false;
    auto& p = *p_ptr;

    pbkdf2::sha256(phrase.data(), phrase.size(),
        salt.data(), salt.size(), one, p.data(), p.size());

    // Parallel conditionally enables parallel execution (blows up memory).
    std::atomic_bool success{ true };
    auto& blocks = array_cast<rblock_type>(p);
    std::for_each(parallel(), blocks.begin(), blocks.end(), [&](auto& block)
    {
        success = success && mix(block.data());
    });

    // False if mix failed or size > pbkdf2::maximum_size.
    return success && pbkdf2::sha256(phrase.data(), phrase.size(),
        p.data(), p.size(), one, buffer, size);
}

#undef TEMPLATE

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
