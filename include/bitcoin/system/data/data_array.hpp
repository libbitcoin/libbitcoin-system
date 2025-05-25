/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_ARRAY_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_ARRAY_HPP

#include <utility>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Byte array of a specified length.
template <size_t Size>
using data_array = std_array<uint8_t, Size>;
template <size_t Size>
using data_array_cptr = std::shared_ptr<const data_array<Size>>;

/// Return type for splitters.
template <size_t Size>
using split_parts = std::pair<data_array<Size>, data_array<Size>>;

/// Arbitrary byte storage types.
typedef data_array<one> one_byte;

/// Create a single byte array with given element value.
constexpr one_byte to_array(uint8_t byte) NOEXCEPT { return { { byte } };}

/////// Convert the data slice to an array.
/////// Underfill is padded with 0x00, excess is truncated.
/////// to_array(to_string(data)) == data.
////template <size_t Size>
////constexpr data_array<Size> to_array(const data_slice& bytes) NOEXCEPT;

/// Convert the string literal to an array.
template <size_t Size, if_nonzero<Size> = true>
constexpr data_array<sub1(Size)> to_array(const char(&string)[Size]) NOEXCEPT;

/// Create a data stack from vector of data array.
template <size_t Size>
const data_stack to_stack(
    const std_vector<data_array<Size>>& values) NOEXCEPT;

/// Concatenate several data slices into a single array.
/// Underfill is padded with 0x00, excess is truncated.
template <size_t Size>
constexpr data_array<Size> build_array(const data_loaf& slices) NOEXCEPT;

/// Extend insertable target by copying extension.
template <class Target>
constexpr Target& extend(Target& target, const data_slice& extension) NOEXCEPT;

/// Extend insertable target by moving extension.
template <class Target, class Extension>
constexpr Target& extend(Target& target, Extension&& extension) NOEXCEPT;

/// Extract a byte subarray from start position with length end minus start.
template <size_t Start, size_t End, size_t Size,
    if_not_greater<Start, Size> = true, if_not_greater<End, Size> = true,
    if_not_lesser<End, Start> = true>
constexpr data_array<End - Start> slice(
    const data_array<Size>& bytes) NOEXCEPT;

/// Break an evenly-sized byte array array into two equal length parts.
template <size_t Size, if_even<Size> = true>
constexpr split_parts<to_half(Size)> split(
    const data_array<Size>& bytes) NOEXCEPT;

/// Concatenate two byte arrays into a new array.
template <size_t Left, size_t Right>
constexpr data_array<Left + Right> splice(const data_array<Left>& left,
    const data_array<Right>& right) NOEXCEPT;

/// Concatenate three byte arrays into a new array.
template <size_t Left, size_t Middle, size_t Right>
constexpr data_array<Left + Middle + Right> splice(const data_array<Left>& left,
    const data_array<Middle>& middle, const data_array<Right>& right) NOEXCEPT;

/// Perform an exclusive or (xor) on two arrays to specified length.
template <size_t Size, size_t Size1, size_t Size2,
    if_not_lesser<Size1, Size> = true, if_not_lesser<Size2, Size> = true>
constexpr data_array<Size> xor_data(const data_array<Size1>& bytes1,
    const data_array<Size2>& bytes2) NOEXCEPT;

/// Perform an exclusive or (xor) on two arrays at specified offsets and length.
template <size_t Size, size_t Offset1, size_t Offset2, size_t Size1, size_t Size2,
    if_not_lesser<Size1, Offset1 + Size> = true,
    if_not_lesser<Size2, Offset2 + Size> = true>
constexpr data_array<Size> xor_offset(const data_array<Size1>& bytes1,
    const data_array<Size2>& bytes2) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_array.ipp>

#endif
