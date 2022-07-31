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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_MINIMAL_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// From integer (including uintx) to explicit array size.
/// ---------------------------------------------------------------------------

template <size_t Size, typename Integer,
    if_integer<Integer> = true>
RCONSTEXPR data_array<Size> to_big_endian_size(Integer value) NOEXCEPT;

template <size_t Size, typename Integer,
    if_integer<Integer> = true>
constexpr data_array<Size> to_little_endian_size(Integer value) NOEXCEPT;

/// From integer (including uintx) to chunk.
/// ---------------------------------------------------------------------------
/// Chunk size infered from integer value bit width (not type size).
/// Use "excess" to reserve the specified number of bytes beyond return size.

template <typename Integer,
    if_integer<Integer> = true>
VCONSTEXPR data_chunk to_big_endian_size(Integer value,
    size_t excess=zero) NOEXCEPT;

template <typename Integer,
    if_integer<Integer> = true>
VCONSTEXPR data_chunk to_little_endian_size(Integer value,
    size_t excess=zero) NOEXCEPT;

/// From chunk to uintx.
/// ---------------------------------------------------------------------------
/// uintx size infered from data value bit width (not vector size).

inline uintx from_big_endian(const data_chunk& data) NOEXCEPT;
inline uintx from_little_endian(const data_chunk& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/minimal.ipp>

#endif
