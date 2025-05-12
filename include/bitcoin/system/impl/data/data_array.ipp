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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_ARRAY_IPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_ARRAY_IPP

#include <algorithm>
#include <iterator>
#include <utility>
#include <bitcoin/system/data/collection.hpp>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/data/memory.hpp>
#include <bitcoin/system/data/no_fill_allocator.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

////template <size_t Size>
////constexpr data_array<Size> to_array(const data_slice& bytes) NOEXCEPT
////{
////    return bytes.to_array<Size>();
////}

template <size_t Size, if_nonzero<Size>>
constexpr data_array<sub1(Size)> to_array(const char(&string)[Size]) NOEXCEPT
{
    data_array<sub1(Size)> out{};
    for (auto i = zero; i < sub1(Size); ++i)
        out[i] = string[i];

    return out;
}

// TODO: test.
template <size_t Size>
const data_stack to_stack(
    const std_vector<data_array<Size>>& values) NOEXCEPT
{
    data_stack chunks(values.size());
    std::transform(values.begin(), values.end(), chunks.begin(),
        [](const data_array<Size>& value) NOEXCEPT
        {
            return to_chunk(value);
        });

    return chunks;
}

template <size_t Size>
constexpr data_array<Size> build_array(const data_loaf& slices) NOEXCEPT
{
    data_array<Size> out;
    auto position = out.begin();
    for (const auto& slice: slices)
    {
        const auto remaining = limit<data_slice::size_type>(
            std::distance(position, out.end()));
        const auto size = std::min(remaining, slice.size());
        std::copy_n(slice.begin(), size, position);
        std::advance(position, size);
    }

    // C++17: Parallel policy for std::fill.
    // Pad any unfilled remainder of the array with zeros.
    std::fill(position, out.end(), 0_u8);
    return out;
}

// This is really more of a data_chunk utility.
template <class Target>
constexpr Target& extend(Target& target, const data_slice& extension) NOEXCEPT
{
    target.insert(std::end(target), std::begin(extension),
        std::end(extension));

    return target;
}

// This is really more of a data_chunk utility.
template <class Target, class Extension>
constexpr Target& extend(Target& target, Extension&& extension) NOEXCEPT
{
    target.insert(std::end(target),
        std::make_move_iterator(std::begin(extension)),
        std::make_move_iterator(std::end(extension)));

    return target;
}

template <size_t Start, size_t End, size_t Size,
    if_not_greater<Start, Size>, if_not_greater<End, Size>,
    if_not_lesser<End, Start>>
constexpr data_array<End - Start> slice(const data_array<Size>& bytes) NOEXCEPT
{
    data_array<End - Start> out;
    std::copy(std::next(bytes.begin(), Start), std::next(bytes.begin(), End),
        out.begin());
    return out;
}

template <size_t Left, size_t Right>
constexpr data_array<Left + Right> splice(const data_array<Left>& left,
    const data_array<Right>& right) NOEXCEPT
{
    data_array<Left + Right> out;
    std::copy(right.begin(), right.end(),
        std::copy(left.begin(), left.end(), out.begin()));

    return out;
}

template <size_t Left, size_t Middle, size_t Right>
constexpr data_array<Left + Middle + Right> splice(const data_array<Left>& left,
    const data_array<Middle>& middle, const data_array<Right>& right) NOEXCEPT
{
    data_array<Left + Middle + Right> out;
    std::copy(right.begin(), right.end(),
        std::copy(middle.begin(), middle.end(),
            std::copy(left.begin(), left.end(), out.begin())));

    return out;
}

template <size_t Size, if_even<Size>>
constexpr split_parts<to_half(Size)> split(const data_array<Size>& bytes) NOEXCEPT
{
    constexpr auto half = to_half(Size);
    split_parts<half> out;
    std::copy_n(bytes.begin(), half, out.first.begin());
    std::copy_n(std::next(bytes.begin(), half), half, out.second.begin());
    return out;
}

template <size_t Size, size_t Size1, size_t Size2,
    if_not_lesser<Size1, Size>, if_not_lesser<Size2, Size>>
constexpr data_array<Size> xor_data(const data_array<Size1>& bytes1,
    const data_array<Size2>& bytes2) NOEXCEPT
{
    return xor_offset<Size, zero, zero>(bytes1, bytes2);
}

template <size_t Size, size_t Offset1, size_t Offset2, size_t Size1, size_t Size2,
    if_not_lesser<Size1, Offset1 + Size>, if_not_lesser<Size2, Offset2 + Size>>
constexpr data_array<Size> xor_offset(const data_array<Size1>& bytes1,
    const data_array<Size2>& bytes2) NOEXCEPT
{
    data_array<Size> out;
    for (size_t index = 0; index < Size; index++)
        out[index] = bytes1[index + Offset1] ^ bytes2[index + Offset2];

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
