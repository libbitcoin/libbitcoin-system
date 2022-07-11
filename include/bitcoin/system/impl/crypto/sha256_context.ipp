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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_IPP

#include <algorithm>
#include <array>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

// Buffer initialization is not required, as counter refects its logical size.
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)
constexpr context::context() NOEXCEPT
  : counter{}, state{ sha256::initial }
{
}
BC_POP_WARNING()

constexpr void context::reset() NOEXCEPT
{
    counter.fill(0);
    state = initial;
}

constexpr size_t context::gap_size() const NOEXCEPT
{
    return block_size - get_size();
}

constexpr size_t context::pad_size() const NOEXCEPT
{
    // size + counter_size cannot exceed two blocks. The difference modulo
    // block_size is the required padding after adding the serialized counter.
    return ((two * block_size) - (get_size() + counter_size)) % block_size;
}

constexpr void context::set_data(size_t size, const uint8_t* bytes) NOEXCEPT
{
    // Overflowing the 64bit counter requires (2^64-1)/8 accumulated bytes.
    // However a set_data exceeding 2^32 bytes will invalidate the counter.
    BC_ASSERT_MSG(size <= max_uint32, "sha256::set_data overflow");

    add_data(size, bytes);
    set_size(possible_narrow_cast<integral>(size));
}

// Cop gguarded byte_get_size (modulo block_size) and fill (< block_size).
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
constexpr void context::add_data(size_t size, const uint8_t* bytes) NOEXCEPT
{
    const auto used = get_size();
    const auto fill = block_size - used;
    std::copy_n(bytes, std::min(fill, size), &buffer[used]);
}
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// protected

// Array is dereferenced only by constexpr indexes.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
constexpr void context::set_size(integral size) NOEXCEPT
{
    // [size << 3] multiplies by 8 to obtain bit count.
    // over is only used to guarantee size is hashed into last block.
    const auto bitsize = size << to_bits;
    const auto overage = size >> to_over;

    // If bits overflows uint32 then over is incremented.
    counter[bits] += bitsize;
    counter[over] += overage + to_int(counter[bits] < bitsize);
}
BC_POP_WARNING()

// Array is dereferenced only by constexpr index.
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
constexpr size_t context::get_size() const NOEXCEPT
{
    // [bits >> 3] divides by 8 to obtain byte count.
    // bytes % block_size is count of bytes of the buffer used.
    return (counter[bits] >> to_bits) % block_size;
}
BC_POP_WARNING()

// inline

inline data_array<context::counter_size>
context::serialize_counter() const NOEXCEPT
{
    // array_cast uses reinterpret_cast, not constexpr
    return array_cast<uint8_t>(to_big_endian_set(counter));
}

inline void context::serialize_state(hash& out) const NOEXCEPT
{
    // array_cast uses reinterpret_cast, not constexpr
    to_big_endian_set(array_cast<uint32_t>(out), state);
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
