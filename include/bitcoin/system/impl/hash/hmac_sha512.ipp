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
#ifndef LIBBITCOIN_SYSTEM_HASH_HMAC_SHA512_IPP
#define LIBBITCOIN_SYSTEM_HASH_HMAC_SHA512_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/functions.hpp>

namespace libbitcoin {
namespace system {
namespace hmac {
namespace sha512 {

constexpr auto block_size = array_count<algorithm::block_t>;
constexpr auto digest_size = array_count<algorithm::digest_t>;
    
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
constexpr void xor_n(uint8_t* to, const uint8_t* from, size_t size) NOEXCEPT
BC_POP_WARNING()
{
    for (size_t i = 0; i < size; ++i)
    {
        BC_PUSH_WARNING(NO_ARRAY_INDEXING)
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        to[i] ^= from[i];
        BC_POP_WARNING()
        BC_POP_WARNING()
    }
}

inline void hash(const uint8_t* data, size_t length, const uint8_t* key,
    size_t key_size, uint8_t* digest) NOEXCEPT
{
    context context{};
    initialize(context, key, key_size);
    update(context, data, length);
    finalize(context, digest);
}

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
inline void initialize(context& context, const uint8_t* key, size_t size) NOEXCEPT
BC_POP_WARNING()
{
    context.out.reset();
    context.in.reset();
    data_array<block_size> pad{};
    data_array<digest_size> key_hash{};

    if (size > block_size)
    {
        context.in.write(size, key);
        context.in.flush(key_hash.data());
        context.in.reset();
        key = key_hash.data();
        size = digest_size;
    }
    
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

    pad.fill(0x36_u8);
    xor_n(&pad[0], key, size);
    context.in.write(block_size, pad.data());
    pad.fill(0x5c_u8);
    xor_n(&pad[0], key, size);

    BC_POP_WARNING()
    BC_POP_WARNING()
    BC_POP_WARNING()

    context.out.write(block_size, pad.data());
}

inline void update(context& context, const uint8_t* data, size_t size) NOEXCEPT
{
    context.in.write(size, data);
}

inline void finalize(context& context, uint8_t* digest) NOEXCEPT
{
    context.in.flush(digest);
    context.out.write(digest_size, digest);
    context.out.flush(digest);
    context.out.reset();
    context.in.reset();
}

} // namespace sha512
} // namespace hmac
} // namespace system
} // namespace libbitcoin

#endif
