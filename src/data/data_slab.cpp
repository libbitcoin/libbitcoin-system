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
#include <bitcoin/system/data/data_slab.hpp>

#include <array>
#include <string>
#include <vector>
#include <bitcoin/system/data/data_slice.hpp>
/// DELETEMENOW

namespace libbitcoin {
namespace system {

// constructors
// ----------------------------------------------------------------------------

data_slab::data_slab() NOEXCEPT
  : data_slab(nullptr, nullptr, zero)
{
}

data_slab::data_slab(std::string& text) NOEXCEPT
  : data_slab(from_size(text.begin(), text.size()))
{
}

// private
data_slab::data_slab(const pointer begin, const pointer end,
    size_type size) NOEXCEPT
  : begin_(begin), end_(end), size_(size)
{
}

// methods
// ----------------------------------------------------------------------------

std::vector<data_slab::value_type> data_slab::to_chunk() const NOEXCEPT
{
    return { begin_, end_ };
}

data_slice data_slab::to_slice() const NOEXCEPT
{
    return { begin_, end_ };
}

std::string data_slab::to_string() const NOEXCEPT
{
    return { begin_, end_ };
}

// Cannot provide a "decode" factory since the data is not owned.
std::string data_slab::encoded() const NOEXCEPT
{
    return to_slice().encoded();
}

bool data_slab::resize(size_t size) NOEXCEPT
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
data_slab::pointer data_slab::data() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
data_slab::pointer data_slab::begin() const NOEXCEPT
{
    return begin_;
}

// Undefined to dereference >= end.
data_slab::pointer data_slab::end() const NOEXCEPT
{
    return end_;
}

data_slab::value_type data_slab::front() const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return empty() ? 0x00 : *begin_;
}

data_slab::value_type data_slab::back() const NOEXCEPT
{
    // Guard against begin underrun (return zero).
    return empty() ? 0x00 : *std::prev(end_);
}

data_slab::size_type data_slab::size() const NOEXCEPT
{
    return size_;
}

bool data_slab::empty() const NOEXCEPT
{
    return is_zero(size_);
}

// operators
// ----------------------------------------------------------------------------

data_slab::operator std::vector<data_slab::value_type>() const NOEXCEPT
{
    return data_slab::to_chunk();
}

data_slab::operator data_slice() const NOEXCEPT
{
    return data_slab::to_slice();
}

data_slab::value_type data_slab::operator[](size_type index) const NOEXCEPT
{
    // Guard against end overrun (return zero).
    return index < size_ ? *std::next(begin_, index) : 0x00;
}

bool operator==(const data_slab& left, const data_slab& right) NOEXCEPT
{
    return left.to_slice() == right.to_slice();
}

bool operator!=(const data_slab& left, const data_slab& right) NOEXCEPT
{
    return !(left == right);
}

} // namespace system
} // namespace libbitcoin
