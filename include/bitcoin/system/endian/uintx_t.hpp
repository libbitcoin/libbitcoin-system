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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Bytes is not required to match data size or Integer type implicit size.
/// Integer may be any integral, uintx or other integer type.

// uintx_t    from_big|little_endian<Size>(data_slice)
// uintx_t    from_big|little_endian_array(data_array)
// data_array   to_big|little_endian<Size>(uintx_t/integer)
    
// TODO: this affects current test expectations.
////template <size_t Size, typename Return = unsigned_exact_type<Size>>

template <size_t Size, typename Return = uintx_t<to_bits(Size)>>
inline Return uintx_from_big_endian_chunk(
    const data_slice& data) NOEXCEPT;

template <size_t Size, typename Return = uintx_t<to_bits(Size)>>
inline Return uintx_from_little_endian_chunk(
    const data_slice& data) NOEXCEPT;

template <size_t Size, typename Return = uintx_t<to_bits(Size)>>
constexpr Return uintx_from_big_endian_array(
    const data_array<Size>& data) NOEXCEPT;

template <size_t Size, typename Return = uintx_t<to_bits(Size)>>
constexpr Return uintx_from_little_endian_array(
    const data_array<Size>& data) NOEXCEPT;

// Explicit sizing, from any integer type.

template <size_t Size, typename Integer,
    if_integer<Integer> = true>
constexpr data_array<Size> to_big_endian(const Integer& value) NOEXCEPT;

template <size_t Size, typename Integer,
    if_integer<Integer> = true>
constexpr data_array<Size> to_little_endian(const Integer& value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/uintx_t.ipp>

#endif
