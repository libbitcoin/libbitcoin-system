/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/point_iterator.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <utility>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
////#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>

namespace libbitcoin {
namespace chain {

static const auto point_size = static_cast<unsigned>(
    std::tuple_size<point>::value);

// Constructors.
//-----------------------------------------------------------------------------

point_iterator::point_iterator(const point_iterator& other)
  : point_iterator(other.point_, other.current_)
{
}

point_iterator::point_iterator(const point& value, unsigned index)
  : point_(value), current_(index)
{
}

// Operators.
//-----------------------------------------------------------------------------

point_iterator::operator bool() const
{
    return current_ < point_size;
}

// private
uint8_t point_iterator::current() const
{
    if (current_ < hash_size)
        return point_.hash()[current_];

    // TODO: move the little-endian iterator into endian.hpp.
    const auto position = current_ - hash_size;
    return static_cast<uint8_t>(point_.index() >> (position * byte_bits));
}

point_iterator::pointer point_iterator::operator->() const
{
    return current();
}

point_iterator::reference point_iterator::operator*() const
{
    return current();
}

point_iterator::iterator& point_iterator::operator++()
{
    increment();
    return *this;
}

point_iterator::iterator point_iterator::operator++(int)
{
    auto it = *this;
    increment();
    return it;
}

point_iterator::iterator& point_iterator::operator--()
{
    decrement();
    return *this;
}

point_iterator::iterator point_iterator::operator--(int)
{
    auto it = *this;
    decrement();
    return it;
}

point_iterator point_iterator::operator+(int value) const
{
    return value < 0 ? decrease(static_cast<unsigned>(std::abs(value))) :
        increase(value);
}

point_iterator point_iterator::operator-(int value) const
{
    return value < 0 ? increase(static_cast<unsigned>(std::abs(value))) :
        decrease(value);
}

bool point_iterator::operator==(const point_iterator& other) const
{
    return (current_ == other.current_) && (&point_ == &other.point_);
}

bool point_iterator::operator!=(const point_iterator& other) const
{
    return !(*this == other);
}

// Utilities.
//-----------------------------------------------------------------------------
// private

void point_iterator::increment()
{
    if (current_ < point_size)
        current_++;
}

void point_iterator::decrement()
{
    if (current_ > 0)
        current_--;
}

point_iterator point_iterator::increase(unsigned value) const
{
    const auto index = ceiling_add(current_, value);
    return point_iterator(point_, std::max(index, point_size));
}

point_iterator point_iterator::decrease(unsigned value) const
{
    const auto index = floor_subtract(current_, value);
    return point_iterator(point_, index);
}

} // namespace chain
} // namespace libbitcoin
