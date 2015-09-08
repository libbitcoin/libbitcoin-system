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
#ifndef LIBBITCOIN_DATA_IPP
#define LIBBITCOIN_DATA_IPP

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

inline byte_array<1> to_byte(uint8_t byte)
{
    return byte_array<1>{{byte}};
}

inline data_chunk build_data(std::initializer_list<data_slice> slices,
    size_t extra_reserve)
{
    size_t size = 0;
    for (const auto slice: slices)
        size += slice.size();

    data_chunk out;
    out.reserve(size + extra_reserve);
    for (const auto slice: slices)
        out.insert(out.end(), slice.begin(), slice.end());

    return out;
}

template <size_t Size>
bool build_array(byte_array<Size>& out,
    std::initializer_list<data_slice> slices)
{
    size_t size = 0;
    for (const auto slice: slices)
        size += slice.size();

    if (size > Size)
        return false;

    auto position = out.begin();
    for (const auto slice: slices)
    {
        std::copy(slice.begin(), slice.end(), position);
        position += slice.size();
    }

    return true;
}

template <class Data, class Type>
void extend_data(Data& buffer, const Type& other)
{
    buffer.insert(std::end(buffer), std::begin(other), std::end(other));
}

template <typename Value>
Value range_constrain(Value value, Value minimum, Value maximum)
{
    if (value < minimum)
        return minimum;

    if (value > maximum)
        return maximum;

    return value;
}

template <class Data>
data_chunk slice(const Data& buffer, size_t start, size_t end)
{
    BITCOIN_ASSERT(start <= buffer.size());

    const auto& data = buffer.data();
    return
    {
        &data[start], &data[end]
    };
}

template <class Data>
data_chunk slice(const Data& buffer, const bounds& range)
{
    return slice(buffer, range.start, range.end);
}

template <class Data>
void split(Data& buffer, data_chunk& lower, data_chunk& upper, size_t size)
{
    BITCOIN_ASSERT(buffer.size() == size);

    const size_t front = size / 2;
    const size_t rest = size - front;
    lower.assign(buffer.begin(), buffer.end() - front);
    upper.assign(buffer.begin() + rest, buffer.end());
}

template <size_t Size>
bool to_array(byte_array<Size>& out, data_slice buffer)
{
    return build_array(out, { buffer });
}

template <class Data>
data_chunk to_data_chunk(const Data iterable)
{
    return data_chunk(std::begin(iterable), std::end(iterable));
}

} // namespace libbitcoin

#endif
