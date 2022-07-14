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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_IPP

#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/algorithm.hpp>

#define BUFFERED_STREAM

namespace libbitcoin {
namespace system {

// integer => data (value-sized) or array (explicit size)
// These allocate the to-endian outgoing buffer and forward the call.
    
// stack allocation/constexpr.
template <size_t Size, typename Integer,
    if_integer<Integer>>
constexpr data_array<Size> to_big_endian_size(Integer value) NOEXCEPT
{
    // Cannot bytecast because non-integral.
    return to_big_data(data_array<Size>{}, value);
}

// stack allocation/constexpr.
template <size_t Size, typename Integer,
    if_integer<Integer>>
constexpr data_array<Size> to_little_endian_size(Integer value) NOEXCEPT
{
    // Cannot bytecast because non-integral.
    return to_little_data(data_array<Size>{}, value);
}

// constexpr (C++20).
template <typename Integer,
    if_integer<Integer>>
VCONSTEXPR data_chunk to_big_endian_size(Integer value,
    size_t excess) NOEXCEPT
{
    // TODO: overflows requires common type.
    ////if constexpr (is_integral<Integer>)
    ////{
    ////    BC_ASSERT(!is_overflow(value, excess));
    ////}

    // TODO: machine::number should be able to predict excess.
    // Vector capacity is never reduced when resizing to smaller size.
    const auto size = byte_width(value);
    data_chunk chunk(size + excess);
    chunk.resize(size);

    // Cannot bytecast because non-integral.
    return to_big_data(std::move(chunk), value);
}

// constexpr (C++20).
template <typename Integer,
    if_integer<Integer>>
VCONSTEXPR data_chunk to_little_endian_size(Integer value,
    size_t excess) NOEXCEPT
{
    // TODO: overflows requires common type.
    ////if constexpr (is_integral<Integer>)
    ////{
    ////    BC_ASSERT(!is_overflow(value, excess));
    ////}

    // TODO: machine::number should be able to predict excess.
    // Vector capacity is never reduced when resizing to smaller size.
    const auto size = byte_width(value);
    data_chunk chunk(size + excess);
    chunk.resize(size);

    // Cannot bytecast because non-integral.
    return to_little_data(std::move(chunk), value);
}

} // namespace system
} // namespace libbitcoin

#endif
