/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DATA_HPP
#define LIBBITCOIN_DATA_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/array_slice.hpp>

namespace libbitcoin {

// Define a byte array of a specified length.
template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

// Define arbitrary byte storage types.
typedef array_slice<uint8_t> data_slice;
typedef std::vector<uint8_t> data_chunk;
typedef std::vector<data_chunk> data_stack;
typedef struct { size_t start; size_t end; } bounds;

/**
 * Create a single byte data chunk with an initial value.
 */
inline byte_array<1> to_byte(uint8_t byte);

/**
 * Concatenate several data slices into a single data_chunk.
 * @param  extra_reserve  Include this many additional bytes when calling
 * `reserve` on the data_chunk (as an optimization).
 */
inline data_chunk build_data(std::initializer_list<data_slice> slices,
    size_t extra_reserve=0);

/**
 * Concatenate several data slices into a single fixed size array.
 * Returns false if the slices don't fit in the array. Underfill is ok.
 */
template <size_t Size>
bool build_array(byte_array<Size>& out,
    std::initializer_list<data_slice> slices);

/**
 * Convert the data buffer to a fixed size array.
 * Returns false if the buffer doesn't fit in the array. Underfill is ok.
 */
template <size_t Size>
bool to_array(byte_array<Size>& out, data_slice bytes);

/**
 * Extend buffer by appending other.
 */
template <class Data, class Type>
void extend_data(Data& bytes, const Type& other);

/**
 * Constrain a numeric value within a given range.
 */
template <typename Value>
Value range_constrain(Value value, Value minimum, Value maximum);

/**
 * Create a data chunk from an interatable object.
 */
template <typename Data>
data_chunk to_data_chunk(const Data& bytes);

/**
 * Return a copy of part of the buffer of length end minus start, starting at
 * start.
 */
BC_API data_chunk slice(data_slice bytes, size_t start, size_t end);

/**
 * Return a copy of part of the buffer of length range.end minus range.start,
 * starting at range.start.
 */
BC_API data_chunk slice(data_slice bytes, const bounds& range);

/**
 * Split a buffer into two even parts (or as close as possible).
 */
BC_API void split(data_chunk& lower, data_chunk& upper, data_slice bytes,
    size_t size);

/**
 * Perform an exclusive or (xor) across two buffers to the length specified.
 * Return the resulting buffer. Caller must ensure offset and length do not
 * exceed either buffer.
 */
BC_API data_chunk xor_data(data_slice bytes1, data_slice bytes2, size_t offset,
    size_t length);

/**
 * Perform an exclusive or (xor) across two buffers to the length specified.
 * Return the resulting buffer. Caller must ensure offsets and lengths do not
 * exceed the respective buffers.
 */
BC_API data_chunk xor_data(data_slice bytes1, data_slice bytes2,
    size_t offset1, size_t offset2, size_t length);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/data.ipp>

#endif
