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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_INTEGERS_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_INTEGERS_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
    
/// Use for misaligned size and non-integral integers.
/// Integer size (apart from single byte guard) is not considered.
/// Data size is not required to match Integer size, zero-padding applied.

/// Native endianness integer to data.
/// ---------------------------------------------------------------------------

// Data to_big_data(Data&&, Integer)
template <typename Data, typename Integer, if_integer<Integer> = true>
constexpr Data to_big_data(Data&& bytes, Integer value) NOEXCEPT;

// Data to_little_data(Data&&, Integer)
template <typename Data, typename Integer, if_integer<Integer> = true>
constexpr Data to_little_data(Data&& bytes, Integer value) NOEXCEPT;

/// Native endianness integer from big-endian data.
/// ---------------------------------------------------------------------------

// Integer from_big_array<Integer>(data_array)
template <typename Integer, size_t Size, if_integer<Integer> = true>
constexpr Integer from_big_array(
    const data_array<Size>& data) NOEXCEPT;

// Integer from_big_array<Integer>(length, data_array)
template <typename Integer, size_t Size, if_integer<Integer> = true>
constexpr Integer from_big_array(size_t length,
    const data_array<Size>& data) NOEXCEPT;

// Integer from_big_chunk<Integer>(length, data_chunk)
template <typename Integer, if_integer<Integer> = true>
VCONSTEXPR Integer from_big_chunk(size_t length,
    const data_chunk& data) NOEXCEPT;

/// Native endianness integer from little-endian data.
/// ---------------------------------------------------------------------------

// Integer from_little_array<Integer>(data_array)
template <typename Integer, size_t Size, if_integer<Integer> = true>
constexpr Integer from_little_array(
    const data_array<Size>& data) NOEXCEPT;

// Integer from_little_array<Integer>(length, data_array)
template <typename Integer, size_t Size, if_integer<Integer> = true>
constexpr Integer from_little_array(size_t length,
    const data_array<Size>& data) NOEXCEPT;

// Integer from_little_chunk<Integer>(length, data_chunk)
template <typename Integer, if_integer<Integer> = true>
VCONSTEXPR Integer from_little_chunk(size_t length,
    const data_chunk& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/integers.ipp>

#endif
