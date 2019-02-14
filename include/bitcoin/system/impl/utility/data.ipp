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
#ifndef LIBBITCOIN_SYSTEM_DATA_IPP
#define LIBBITCOIN_SYSTEM_DATA_IPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <initializer_list>
#include <bitcoin/system/utility/assert.hpp>

namespace libbitcoin {
namespace system {

inline one_byte to_array(uint8_t byte)
{
    return byte_array<1>{ { byte } };
}

inline data_chunk to_chunk(uint8_t byte)
{
    return data_chunk{ byte };
}

inline data_chunk build_chunk(loaf slices, size_t extra_reserve)
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
bool build_array(byte_array<Size>& out, loaf slices)
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

template <class Target, class Extension>
void extend_data(Target& bytes, const Extension& other)
{
    bytes.insert(std::end(bytes), std::begin(other), std::end(other));
}

// std::array<> is used in place of byte_array<> to enable Size deduction.
template <size_t Start, size_t End, size_t Size>
byte_array<End - Start> slice(const std::array<uint8_t, Size>& bytes)
{
    static_assert(End <= Size, "Slice end must not exceed array size.");
    byte_array<End - Start> out;
    std::copy(std::begin(bytes) + Start, std::begin(bytes) + End, out.begin());
    return out;
}

template <size_t Left, size_t Right>
byte_array<Left + Right> splice(const std::array<uint8_t, Left>& left,
    const std::array<uint8_t, Right>& right)
{
    byte_array<Left + Right> out;
    /* safe to ignore */ build_array<Left + Right>(out, { left, right });
    return out;
}

template <size_t Left, size_t Middle, size_t Right>
byte_array<Left + Middle + Right> splice(const std::array<uint8_t, Left>& left,
    const std::array<uint8_t, Middle>& middle,
    const std::array<uint8_t, Right>& right)
{
    byte_array<Left + Middle + Right> out;
    /* safe to ignore */ build_array(out, { left, middle, right });
    return out;
}

template <size_t Size>
byte_array_parts<Size / 2> split(const byte_array<Size>& bytes)
{
    static_assert(Size != 0, "Split requires a non-zero parameter.");
    static_assert(Size % 2 == 0, "Split requires an even length parameter.");
    static const size_t half = Size / 2;
    byte_array_parts<half> out;
    std::copy_n(std::begin(bytes), half, out.left.begin());
    std::copy_n(std::begin(bytes) + half, half, out.right.begin());
    return out;
}

// unsafe
template <size_t Size>
byte_array<Size> to_array(const data_slice& bytes)
{
    byte_array<Size> out;
    DEBUG_ONLY(const auto result =) build_array(out, { bytes });
    BITCOIN_ASSERT(result);
    return out;
}

template <typename Source>
data_chunk to_chunk(const Source& bytes)
{
    return data_chunk(std::begin(bytes), std::end(bytes));
}

template <typename Source>
bool starts_with(const typename Source::const_iterator& begin,
    const typename Source::const_iterator& end, const Source& value)
{
    const auto length = std::distance(begin, end);
    return length >= 0 && static_cast<size_t>(length) >= value.size() &&
        std::equal(value.begin(), value.end(), begin);
}

// unsafe
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2)
{
    return xor_data<Size>(bytes1, bytes2, 0);
}

// unsafe
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2, size_t offset)
{
    return xor_data<Size>(bytes1, bytes2, offset, offset);
}

// unsafe
template <size_t Size>
byte_array<Size> xor_data(const data_slice& bytes1, const data_slice& bytes2, size_t offset1,
    size_t offset2)
{
    BITCOIN_ASSERT(offset1 + Size <= bytes1.size());
    BITCOIN_ASSERT(offset2 + Size <= bytes2.size());
    const auto& data1 = bytes1.data();
    const auto& data2 = bytes2.data();
    byte_array<Size> out;
    for (size_t i = 0; i < Size; i++)
        out[i] = data1[i + offset1] ^ data2[i + offset2];

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
