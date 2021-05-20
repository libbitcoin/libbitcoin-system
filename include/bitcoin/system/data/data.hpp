/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <queue>
#include <utility>
#include <vector>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Byte array of a specified length.
template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

/// Return type for splitters.
template <size_t Size>
using split_parts = std::pair<byte_array<Size>, byte_array<Size>>;

/// Arbitrary byte storage types.
typedef byte_array<1> one_byte;
typedef std::vector<uint8_t> data_chunk;
typedef std::queue<data_chunk> data_queue;
typedef std::vector<data_chunk> data_stack;

/// Cast a char to a byte.
inline uint8_t to_byte(char character);

/// Convert the data slice to an array.
/// Underfill is padded with 0x00, excess is truncated.
template <size_t Size>
byte_array<Size> to_array(const data_slice& bytes);

/// Concatenate several data slices into a single fixed size array.
/// Underfill is padded with 0x00, excess is truncated.
template <size_t Size>
byte_array<Size> build_array(const data_loaf& slices);

/// Extend insertable target by copying extension.
template <class Target, class Extension>
Target& extend_data(Target& target, const Extension& extension);

/// Extend insertable target by moving extension.
template <class Target, class Extension>
Target& extend_data(Target& target, Extension&& extension);

/// Extract a subarray from start position with length end minus start.
template <size_t Start, size_t End, size_t Size>
byte_array<End - Start> slice(const byte_array<Size>& bytes);

/// Break an evenly-sized array array into two equal length parts.
template <size_t Size>
split_parts<Size / 2u> split(const byte_array<Size>& bytes);

/// Concatenate two arrays into a new array.
template <size_t Left, size_t Right>
byte_array<Left + Right> splice(const byte_array<Left>& left,
    const byte_array<Right>& right);

/// Concatenate three arrays into a new array.
template <size_t Left, size_t Middle, size_t Right>
byte_array<Left + Middle + Right> splice(const byte_array<Left>& left,
    const byte_array<Middle>& middle, const byte_array<Right>& right);

/// Determine if a buffer starts with a byte sequence.
template <typename Source, class Target>
bool starts_with(const typename Source::const_iterator& begin,
    const typename Source::const_iterator& end, const Source& value);

/// Perform an exclusive or (xor) on two arrays to specified length.
template <size_t Size, size_t Size1, size_t Size2>
byte_array<Size> xor_data(const byte_array<Size1>& bytes1,
    const byte_array<Size2>& bytes2);

/// Perform an exclusive or (xor) on two arrays at specified offsets and length.
template <size_t Size, size_t Offset1, size_t Offset2, size_t Size1, size_t Size2>
byte_array<Size> xor_offset(const byte_array<Size1>& bytes1,
    const byte_array<Size2>& bytes2);

/// Create a single byte arrray with given element value.
BC_API one_byte to_array(uint8_t byte);

/// Create a single byte data_chunk with given element value.
BC_API data_chunk to_chunk(uint8_t byte);

/// Create a data chunk from data slice.
BC_API data_chunk to_chunk(const data_slice& bytes);

/// Concatenate several data slices into a single data_chunk.
/// extra_reserve reserves but does not allocate additional bytes.
BC_API data_chunk build_chunk(const data_loaf& slices, size_t extra_reserve=0);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data.ipp>

#endif
