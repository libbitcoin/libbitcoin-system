/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_SLICE_IPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_SLICE_IPP

#include <array>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
////#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
   
// constructors
// ----------------------------------------------------------------------------

template <data_slice::size_type Size>
constexpr data_slice::data_slice(const char(&bytes)[Size]) NOEXCEPT
  : data_slice(data_slice::from_literal(bytes))
{
}

template <data_slice::size_type Size, typename Byte, if_one_byte<Byte>>
constexpr data_slice::data_slice(const std::array<Byte, Size>& data) NOEXCEPT
  : data_slice(data_slice::from_size(data.begin(), Size))
{
}

template <typename Byte, if_one_byte<Byte>>
constexpr data_slice::data_slice(const std::vector<Byte>& data) NOEXCEPT
  : data_slice(data_slice::from_size(data.begin(), data.size()))
{
}

template <typename Iterator>
constexpr data_slice::data_slice(const Iterator& begin,
    const Iterator& end) NOEXCEPT
  : data_slice(data_slice::from_iterators(begin, end))
{
}

template <typename Byte, if_one_byte<Byte>>
constexpr data_slice::data_slice(const Byte* begin, const Byte* end) NOEXCEPT
  : data_slice(data_slice::from_iterators(begin, end))
{
}
    
constexpr data_slice::data_slice() NOEXCEPT
  : data_slice(nullptr, nullptr, zero)
{
}

constexpr data_slice::data_slice(const std::string& text) NOEXCEPT
  : data_slice(data_slice::from_size(text.begin(), text.size()))
{
}

constexpr data_slice::data_slice(
    std::initializer_list<value_type> bytes) NOEXCEPT
  : data_slice(data_slice::from_size(bytes.begin(), bytes.size()))
{
}

// private
constexpr data_slice::data_slice(pointer begin, pointer end,
    size_type size) NOEXCEPT
  : begin_(begin), end_(end), size_(size)
{
}

// factories (private)
// ----------------------------------------------------------------------------

template <data_slice::size_type Size, typename Byte>
constexpr data_slice data_slice::from_literal(const Byte(&bytes)[Size]) NOEXCEPT
{
    // Guard 0 for lack of null terminator (see below).
    if (is_zero(Size))
        return {};

    // Literals are null terminated but initializer syntax char arrays are not.
    // Those are picked up here and lose their last char. Workaround:
    // data_slice slice({ 'f', 'o', 'o', 'b', 'a', 'r', '\0' });
    // The more common syntax works as expected: data_slice slice("foobar");
    return from_size(&bytes[0], sub1(Size));
}

// static
template <typename Iterator>
constexpr data_slice data_slice::from_iterators(const Iterator& begin,
    const Iterator& end) NOEXCEPT
{
    // An end iterator can be anything, so convert to size.
    const auto size = std::distance(begin, end);

    // Guard negative against iterators reversed (undefined behavior).
    // Guard 0 because &begin[0] is undefined if size is zero.
    if (is_negative(size) || is_zero(size))
        return {};

    // Indexation is guarded above.
    BC_PUSH_WARNING(USE_GSL_AT)
    return from_size(&begin[0], possible_narrow_sign_cast<size_type>(size));
    BC_POP_WARNING()
}

// static
template <typename Pointer>
constexpr data_slice data_slice::from_size(Pointer begin,
    size_type size) NOEXCEPT
{
    // Guard 0 because &begin[0] is undefined if size is zero.
    if (is_zero(size))
        return {};

    // Indexation is guarded above.
    // Pointer may be char* or uin8_t*, or iterator type (cast not required).
    BC_PUSH_WARNING(USE_GSL_AT)
    const auto start = possible_pointer_cast<const value_type>(&begin[0]);
    BC_POP_WARNING()

    return { start, std::next(start, size), size };
}

// methods
// ----------------------------------------------------------------------------

template <data_slice::size_type Size>
std::array<data_slice::value_type, Size>
constexpr data_slice::to_array() const NOEXCEPT
{
    std::array<data_slice::value_type, Size> out;

    for (size_type index = 0; index < Size; ++index)
    {
        // Array operator safely iterates and emits zeros past end.
        BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
        out[index] = (*this)[index];
        BC_POP_WARNING()
    }

    return out;
}

VCONSTEXPR std::vector<data_slice::value_type> data_slice::
to_chunk() const NOEXCEPT
{
    return { begin_, end_ };
}

SCONSTEXPR std::string data_slice::to_string() const NOEXCEPT
{
    return { begin_, end_ };
}

// dependency ordering
////// Cannot provide a "decode" factory since the data is not owned.
////SCONSTEXPR std::string data_slice::encoded() const NOEXCEPT
////{
////    return encode_base16(to_chunk());
////}

constexpr bool data_slice::resize(size_t size) NOEXCEPT
{
    if (size >= size_)
        return false;

    end_ = std::next(begin_, size);
    size_ = size;
    return true;
}

// properties
// ----------------------------------------------------------------------------

// Undefined to dereference >= end.
constexpr data_slice::pointer data_slice::data() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
constexpr data_slice::pointer data_slice::begin() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
constexpr data_slice::pointer data_slice::end() const NOEXCEPT
{
    return end_;
}

constexpr data_slice::value_type data_slice::front() const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return empty() ? 0_u8 : *begin_;
}

constexpr data_slice::value_type data_slice::back() const NOEXCEPT
{
    // Guard against begin underrun (return zero).
    return empty() ? 0_u8 : *std::prev(end_);
}

constexpr data_slice::size_type data_slice::size() const NOEXCEPT
{
    return size_;
}

constexpr bool data_slice::empty() const NOEXCEPT
{
    return is_zero(size_);
}

// operators
// ----------------------------------------------------------------------------

template <data_slice::size_type Size>
constexpr data_slice::
operator std::array<data_slice::value_type, Size>() const NOEXCEPT
{
    return to_array<Size>();
}


VCONSTEXPR data_slice::
operator std::vector<data_slice::value_type>() const NOEXCEPT
{
    return data_slice::to_chunk();
}

constexpr data_slice::value_type data_slice::
operator[](size_type index) const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return index < size_ ? *std::next(begin_, index) : 0_u8;
}

constexpr bool
operator==(const data_slice& left, const data_slice& right) NOEXCEPT
{
    if (left.size() != right.size())
        return false;

    // std::vector performs value comparison, so this does too.

    for (data_slice::size_type index = 0; index < left.size(); ++index)
    {
        // Indexation is guarded above.
        BC_PUSH_WARNING(USE_GSL_AT)
        if (left[index] != right[index]) return false;
        BC_POP_WARNING()
    }

    return true;
}

constexpr bool
operator!=(const data_slice& left, const data_slice& right) NOEXCEPT
{
    return !(left == right);
}

////constexpr data_slice& data_slice::
////operator=(const data_slice& other)
////{
////    begin_ = other.begin_;
////    end_ = other.end_;
////    size_ = other.size_;
////    return *this;
////}

} // namespace system
} // namespace libbitcoin

#endif
