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

template<size_t Size> using byte_array = std::array<uint8_t, Size>;

inline byte_array<1> to_byte(uint8_t byte)
{
    return byte_array<1>{{byte}};
}

// Arbitrary byte storage.
typedef array_slice<uint8_t> data_slice;
typedef std::vector<uint8_t> data_chunk;
typedef std::vector<data_chunk> data_stack;

template<typename T>
data_chunk to_data_chunk(T iterable);

/**
 * Concatenates several data slices into a single data_chunk.
 * @param extra_space include this much extra space when calling
 * `reserve` on the data_chunk (as an optimization).
 */
data_chunk build_data(std::initializer_list<data_slice> slices,
    size_t extra_space=0);

/**
 * Extend `data` by appending `other`.
 */
template <typename D, typename T>
void extend_data(D& data, const T& other);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/data.ipp>

#endif

