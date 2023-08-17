/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_IPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_IPP

#include <array>
#include <vector>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
   
// constructors
// ----------------------------------------------------------------------------

template <data_slab::size_type Size, typename Byte, if_one_byte<Byte>>
constexpr data_slab::data_slab(std::array<Byte, Size>& data) NOEXCEPT
  : data_slab(from_size(data.begin(), Size))
{
}

// std::vector.begin not constexpr (need full C++20).
template <typename Byte, if_one_byte<Byte>>
VCONSTEXPR data_slab::data_slab(std::vector<Byte>& data) NOEXCEPT
  : data_slab(from_size_(data.begin(), data.size()))
{
}

template <typename Iterator>
constexpr data_slab::data_slab(const Iterator& begin,
    const Iterator& end) NOEXCEPT
  : data_slab(from_iterators(begin, end))
{
}

template <typename Byte, if_one_byte<Byte>>
constexpr data_slab::data_slab(const Byte* begin, const Byte* end) NOEXCEPT
  : data_slab(from_iterators(begin, end))
{
}

constexpr data_slab::data_slab() NOEXCEPT
  : data_slab(nullptr, nullptr, zero)
{
}

// std::string.begin not constexpr (need full C++20).
SCONSTEXPR data_slab::data_slab(std::string& text) NOEXCEPT
  : data_slab(from_size_(text.begin(), text.size()))
{
}

// private
constexpr data_slab::data_slab(const pointer begin, const pointer end,
    size_type size) NOEXCEPT
  : begin_(begin), end_(end), size_(size)
{
}

// factories (private)
// ----------------------------------------------------------------------------

// static
template <typename Iterator>
constexpr data_slab data_slab::from_iterators(const Iterator& begin,
    const Iterator& end) NOEXCEPT
{
    // An end iterator can be anything, so convert to size.
    const auto size = std::distance(begin, end);

    // Guard negative against iterators reversed (undefined behavior).
    // Guard 0 because &begin[0] is undefined if size is zero.
    if (is_negative(size) || is_zero(size))
        return {};

    return from_size(&begin[0], possible_narrow_sign_cast<size_type>(size));
}

// static
template <typename Pointer>
constexpr data_slab data_slab::from_size(const Pointer begin,
    size_type size) NOEXCEPT
{
    // Guard 0 because &begin[0] is undefined if size is zero.
    if (is_zero(size))
        return {};

    // Indexation is guarded above.
    // Pointer may be char* or uin8_t*, or iterator type (cast not required).
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)
    const auto start = possible_pointer_cast<value_type>(&begin[0]);
    BC_POP_WARNING()

    return { start, std::next(start, size), size };
}

// static
template <typename Pointer>
SVCONSTEXPR data_slab data_slab::from_size_(const Pointer begin,
    size_type size) NOEXCEPT
{
    return from_size(begin, size);
}

// methods
// ----------------------------------------------------------------------------

VCONSTEXPR std::vector<data_slab::value_type> data_slab::to_chunk() const NOEXCEPT
{
    return { begin_, end_ };
}

SCONSTEXPR std::string data_slab::to_string() const NOEXCEPT
{
    return { begin_, end_ };
}

constexpr data_slice data_slab::to_slice() const NOEXCEPT
{
    return { begin_, end_ };
}

constexpr bool data_slab::resize(size_t size) NOEXCEPT
{
    if (size >= size_)
        return false;

    end_ = std::next(begin_, size);
    size_ = size;
    return true;
}

// properties
// ----------------------------------------------------------------------------

template <data_slab::size_type Size>
constexpr std::array<data_slab::value_type, Size>
data_slab::to_array() const NOEXCEPT
{
    std::array<data_slab::value_type, Size> out;

    // Array operator safely iterates and emits zeros past end.
    for (size_type index = 0; index < Size; ++index)
    {
        // Array operator safely iterates and emits zeros past end.
        BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
        out[index] = (*this)[index];
        BC_POP_WARNING()
    }

    return out;
}

// Undefined to dereference >= end.
constexpr data_slab::pointer data_slab::data() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
constexpr data_slab::pointer data_slab::begin() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
constexpr data_slab::pointer data_slab::end() const NOEXCEPT
{
    return end_;
}

constexpr data_slab::value_type data_slab::front() const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return empty() ? 0x00 : *begin_;
}

constexpr data_slab::value_type data_slab::back() const NOEXCEPT
{
    // Guard against begin underrun (return zero).
    return empty() ? 0x00 : *std::prev(end_);
}

constexpr data_slab::size_type data_slab::size() const NOEXCEPT
{
    return size_;
}

constexpr bool data_slab::empty() const NOEXCEPT
{
    return is_zero(size_);
}

// operators
// ----------------------------------------------------------------------------

////template <data_slab::size_type Size>
////constexpr data_slab::operator
////std::array<data_slab::value_type, Size>() const NOEXCEPT
////{
////    return to_array<Size>();
////}
////
////VCONSTEXPR data_slab::operator
////std::vector<data_slab::value_type>() const NOEXCEPT
////{
////    return data_slab::to_chunk();
////}

constexpr data_slab::
operator data_slice() const NOEXCEPT
{
    return data_slab::to_slice();
}

constexpr data_slab::value_type data_slab::
operator[](size_type index) const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return index < size_ ? *std::next(begin_, index) : 0x00;
}

constexpr bool operator==(const data_slab& left, const data_slab& right) NOEXCEPT
{
    return left.to_slice() == right.to_slice();
}

constexpr bool operator!=(const data_slab& left, const data_slab& right) NOEXCEPT
{
    return !(left == right);
}

} // namespace system
} // namespace libbitcoin

#endif
