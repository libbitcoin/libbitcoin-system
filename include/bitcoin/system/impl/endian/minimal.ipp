/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_IPP

#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/integers.hpp>
#include <bitcoin/system/math/math.hpp>

#define BUFFERED_STREAM

namespace libbitcoin {
namespace system {
    
// From integer (including uintx) to explicit array size.
// ----------------------------------------------------------------------------

template <size_t Size, typename Integer,
    if_integer<Integer>>
constexpr data_array<Size> to_big_endian_size(Integer value) NOEXCEPT
{
    return to_big_data(data_array<Size>{}, value);
}

template <size_t Size, typename Integer,
    if_integer<Integer>>
constexpr data_array<Size> to_little_endian_size(Integer value) NOEXCEPT
{
    return to_little_data(data_array<Size>{}, value);
}

// From integer (including uintx) to chunk.
// ----------------------------------------------------------------------------

template <typename Integer,
    if_integer<Integer>>
data_chunk to_big_endian_size(Integer value, size_t excess) NOEXCEPT
{
    BC_ASSERT(!is_add_overflow(byte_width(value), excess));

    // Vector capacity is never reduced when resizing to smaller size.
    const auto size = byte_width(value);
    data_chunk chunk(size + excess);
    chunk.resize(size);
    return to_big_data(std::move(chunk), value);
}

template <typename Integer,
    if_integer<Integer>>
data_chunk to_little_endian_size(Integer value, size_t excess) NOEXCEPT
{
    BC_ASSERT(!is_add_overflow(byte_width(value), excess));

    // Vector capacity is never reduced when resizing to smaller size.
    const auto size = byte_width(value);
    data_chunk chunk(size + excess);
    chunk.resize(size);

    return to_little_data(std::move(chunk), value);
}

// From chunk to uintx.
// ----------------------------------------------------------------------------

inline uintx from_big_endian(const data_chunk& data) NOEXCEPT
{
    return from_big_chunk<uintx>(data.size(), data);
}

inline uintx from_little_endian(const data_chunk& data) NOEXCEPT
{
    return from_little_chunk<uintx>(data.size(), data);
}

} // namespace system
} // namespace libbitcoin

#endif
