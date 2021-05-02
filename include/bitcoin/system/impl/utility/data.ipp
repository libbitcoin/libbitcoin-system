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
#include <cstdint>
#include <iterator>
#include <utility>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data_slice.hpp>

namespace libbitcoin {
namespace system {

inline uint8_t to_byte(char character)
{
    return static_cast<uint8_t>(character);
}

template <size_t Size>
byte_array<Size> to_array(const data_slice& bytes)
{
    return bytes.to_array<Size>();
}

template <size_t Size>
byte_array<Size> build_array(const loaf& slices)
{
    static const ptrdiff_t zero{ 0 };
    byte_array<Size> out;
    auto position = out.begin();
    for (const auto& slice: slices)
    {
        const auto unfilled = std::distance(position, out.end());
        const auto remain = static_cast<size_t>(std::max(unfilled, zero));
        const auto size = std::min(remain, slice.size());
        std::copy_n(slice.begin(), size, position);
        std::advance(position, size);
    }

    // Pad any unfilled remainder of the array with zeros.
    std::fill(position, out.end(), 0x00);
    return out;
}

template <class Target, class Extension>
Target& extend_data(Target& target, const Extension& extension)
{
    target.insert(std::end(target), std::begin(extension),
        std::end(extension));

    return target;
}

template <class Target, class Extension>
Target& extend_data(Target& target, Extension&& extension)
{
    target.insert(std::end(target),
        std::make_move_iterator(std::begin(extension)),
        std::make_move_iterator(std::end(extension)));

    return target;
}

template <size_t Start, size_t End, size_t Size>
byte_array<End - Start> slice(const byte_array<Size>& bytes)
{
    static_assert(End <= Size, "Slice end must not exceed array size.");
    byte_array<End - Start> out;
    std::copy(bytes.begin() + Start, bytes.begin() + End, out.begin());
    return out;
}

template <size_t Left, size_t Right>
byte_array<Left + Right> splice(const byte_array<Left>& left,
    const byte_array<Right>& right)
{
    return build_array<Left + Right>({ left, right });
}

template <size_t Left, size_t Middle, size_t Right>
byte_array<Left + Middle + Right> splice(const byte_array<Left>& left,
    const byte_array<Middle>& middle, const byte_array<Right>& right)
{
    return build_array<Left + Middle + Right>({ left, middle, right });
}

template <size_t Size>
split_parts<Size / 2u> split(const byte_array<Size>& bytes)
{
    static_assert(Size != 0u, "Split requires a non-zero parameter.");
    static_assert(Size % 2u == 0u, "Split requires an even length parameter.");
    static const auto half = Size / 2u;
    split_parts<half> out;
    std::copy_n(bytes.begin(), half, out.first.begin());
    std::copy_n(std::next(bytes.begin(), half), half, out.second.begin());
    return out;
}

template <typename Source>
bool starts_with(const typename Source::const_iterator& begin,
    const typename Source::const_iterator& end, const Source& value)
{
    const auto length = std::distance(begin, end);
    return length >= 0 && static_cast<uint64_t>(length) >= value.size() &&
        std::equal(value.begin(), value.end(), begin);
}

template <size_t Size, size_t Size1, size_t Size2>
byte_array<Size> xor_data(const byte_array<Size1>& bytes1,
    const byte_array<Size2>& bytes2)
{
    return xor_offset<Size, 0u, 0u>(bytes1, bytes2);
}

template <size_t Size, size_t Offset1, size_t Offset2, size_t Size1, size_t Size2>
byte_array<Size> xor_offset(const byte_array<Size1>& bytes1,
    const byte_array<Size2>& bytes2)
{
    static_assert(Size + Offset1 <= Size1, "xor_data Size + Offset1 > Size1");
    static_assert(Size + Offset2 <= Size2, "xor_data Size + Offset2 > Size2");

    byte_array<Size> out;
    const auto& data1 = bytes1.data();
    const auto& data2 = bytes2.data();
    for (size_t index = 0; index < Size; index++)
        out[index] = data1[index + Offset1] ^ data2[index + Offset2];

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
