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
#ifndef LIBBITCOIN_SYSTEM_DATA_HPP
#define LIBBITCOIN_SYSTEM_DATA_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <queue>
#include <vector>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/array_slice.hpp>

namespace libbitcoin {
namespace system {

// Define a byte array of a specified length.
template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

template <size_t Size>
struct byte_array_parts
{
    byte_array<Size> left;
    byte_array<Size> right;
};

// Define arbitrary byte storage types.
typedef byte_array<1> one_byte;
typedef array_slice<uint8_t> data_slice;
typedef std::vector<uint8_t> data_chunk;
typedef std::queue<data_chunk> data_queue;
typedef std::vector<data_chunk> data_stack;
typedef std::initializer_list<data_slice> loaf;

/**
 * Create a single byte array with an initial value.
 */
inline one_byte to_array(uint8_t byte);

/**
* Create a single byte data chunk with an initial value.
*/
inline data_chunk to_chunk(uint8_t byte);

/**
 * Concatenate several data slices into a single data_chunk.
 * @param  extra_reserve  Include this many additional bytes when calling
 * `reserve` on the data_chunk (as an optimization).
 */
inline data_chunk build_chunk(loaf slices, size_t extra_reserve=0);

/**
 * Concatenate several data slices into a single fixed size array.
 * Returns false if the slices don't fit in the array. Underfill is ok.
 */
template <size_t Size>
bool build_array(byte_array<Size>& out, loaf slices);

/**
 * Extend iterable object by appending other.
 */
template <class Target, class Extension>
void extend_data(Target& target, const Extension& extension);

/**
 * Extracty a subarray from start position with length end minus start.
 */
template <size_t Start, size_t End, size_t Size>
byte_array<End - Start> slice(const std::array<uint8_t, Size>& bytes);

/**
 * Break an evenly-sized array array into two equal length parts.
 */
template <size_t Size>
byte_array_parts<Size / 2> split(const byte_array<Size>& bytes);

/**
 * Concatenate two arrays into a new array.
 */
template <size_t Left, size_t Right>
byte_array<Left + Right> splice(const std::array<uint8_t, Left>& left,
    const std::array<uint8_t, Right>& right);

/**
 * Concatenate three arrays into a new array.
 */
template <size_t Left, size_t Middle, size_t Right>
byte_array<Left + Middle + Right> splice(const std::array<uint8_t, Left>& left,
    const std::array<uint8_t, Middle>& middle,
    const std::array<uint8_t, Right>& right);

/**
 * Convert the data slice to an array. Underfill is ok.
 */
template <size_t Size>
byte_array<Size> to_array(const data_slice& bytes);

/**
 * Create a data chunk from an iterable object.
 */
template <typename Source>
data_chunk to_chunk(const Source& bytes);

/**
 * Safely determine if a buffer starts with a byte sequence.
 */
template <typename Source>
bool starts_with(const typename Source::const_iterator& begin,
    const typename Source::const_iterator& end, const Source& value);

/**
 * Perform an exclusive or (xor) across two buffers to the length specified.
 * Return the resulting buffer. Caller must ensure length does not exceed
 * either buffer.
 */
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2);

/**
 * Perform an exclusive or (xor) across two buffers to the length specified.
 * Return the resulting buffer. Caller must ensure offset and length do not
 * exceed either buffer.
 */
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2,
    size_t offset);

/**
 * Perform an exclusive or (xor) across two buffers to the length specified.
 * Return the resulting buffer. Caller must ensure offsets and lengths do not
 * exceed the respective buffers.
 */
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2,
    size_t offset1, size_t offset2);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/data.ipp>

#endif
