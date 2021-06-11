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
#include <bitcoin/system/data/data_slice.hpp>

#include <array>
#include <ostream>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>
#include <bitcoin/system/radix/base_16.hpp>

namespace libbitcoin {
namespace system {

const data_slice::value_type data_slice::pad = 0x00;

// constructors
// ----------------------------------------------------------------------------

data_slice::data_slice()
  : data_slice(nullptr, nullptr, 0u)
{
}

data_slice::data_slice(const data_slice& other)
  : begin_(other.begin_), end_(other.end_), size_(other.size_)
{
}

data_slice::data_slice(const std::string& text)
  : data_slice(from_size(text.begin(), text.size()))
{
}

data_slice::data_slice(std::initializer_list<value_type> bytes)
  : data_slice(from_size(bytes.begin(), bytes.size()))
{
}

// private
data_slice::data_slice(const_pointer begin, const_pointer end, size_type size)
  : begin_(begin), end_(end), size_(size)
{
}

// methods
// ----------------------------------------------------------------------------

std::vector<data_slice::value_type> data_slice::to_chunk() const
{
    return { begin_, end_ };
}

std::string data_slice::to_string() const
{
    return { begin_, end_ };
}

// Cannot provide a "decode" factory since the data is not owned.
std::string data_slice::encoded() const
{
    return encode_base16(to_chunk());
}

bool data_slice::resize(size_t size)
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
data_slice::const_pointer data_slice::data() const
{
    return begin_;
}

// Undefined to dereference >= end.
data_slice::const_pointer data_slice::begin() const
{
    return begin_;
}

// Undefined to dereference >= end.
data_slice::const_pointer data_slice::end() const
{
    return end_;
}

data_slice::value_type data_slice::front() const
{
    // Guard against end overrun (return zero).
    return empty() ? pad : *begin_;
}

data_slice::value_type data_slice::back() const
{
    // Guard against begin underrun (return zero).
    return empty() ? pad : *std::prev(end_, 1);
}

data_slice::size_type data_slice::size() const
{
    return size_;
}

bool data_slice::empty() const
{
    return size_ == 0u;
}

// operators
// ----------------------------------------------------------------------------

data_slice::operator std::vector<data_slice::value_type>() const
{
    return data_slice::to_chunk();
}

data_slice::value_type data_slice::operator[](size_type index) const
{
    // Guard against end overrun (return zero).
    return index < size_ ? *std::next(begin_, index) : pad;
}

bool operator==(const data_slice& left, const data_slice& right)
{
    // Produces clang template specialization error.
    ////return std::equal(left.begin(), left.end(), right.begin(), right.end());

    if (left.size() != right.size())
        return false;

    for (data_slice::size_type index = 0; index < left.size(); ++index)
        if (left[index] != right[index])
            return false;

    return true;
}

bool operator!=(const data_slice& left, const data_slice& right)
{
    return !(left == right);
}

} // namespace system
} // namespace libbitcoin
