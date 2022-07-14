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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/minimal.hpp>
#include <bitcoin/system/endian/nominal.hpp>

namespace libbitcoin {
namespace system {

// data => uintx_t<to_bits(Bytes)>
// uintx_t (or other integer) => data_array

// data_slice is not constexpr.

template <size_t Size>
inline uintx_t<to_bits(Size)> uintx_from_big_endian_chunk(
    const data_chunk& data) NOEXCEPT
{
    return from_big_chunk<uintx_t<to_bits(Size)>>(Size, data);
}

template <size_t Size>
inline uintx_t<to_bits(Size)> uintx_from_little_endian_chunk(
    const data_chunk& data) NOEXCEPT
{
    return from_little_chunk<uintx_t<to_bits(Size)>>(Size, data);
}

// TODO: recombine and rename when data_slice is constexpr.
// data_array and data_chunk split from data_slice for constexpr support.

template <size_t Size>
constexpr uintx_t<to_bits(Size)> uintx_from_big_endian_array(
    const data_array<Size>& data) NOEXCEPT
{
    return from_big_array<uintx_t<to_bits(Size)>>(data);
}

template <size_t Size>
constexpr uintx_t<to_bits(Size)> uintx_from_little_endian_array(
    const data_array<Size>& data) NOEXCEPT
{
    return from_little_array<uintx_t<to_bits(Size)>>(data);
}

// Explicit sizing, from any integer type.

template <size_t Size, typename Integer, if_integer<Integer>>
constexpr data_array<Size> to_big_endian(const Integer& value) NOEXCEPT
{
    // minimal
    return to_big_endian_size<Size>(value);
}

template <size_t Size, typename Integer, if_integer<Integer>>
constexpr data_array<Size> to_little_endian(const Integer& value) NOEXCEPT
{
    // minimal
    return to_little_endian_size<Size>(value);
}

} // namespace system
} // namespace libbitcoin

#endif
