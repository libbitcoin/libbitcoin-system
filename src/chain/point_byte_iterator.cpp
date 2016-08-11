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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/point_byte_iterator.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {
namespace chain {

const uint8_t max_offset = hash_size + sizeof(uint32_t);

point_byte_iterator::point_byte_iterator(const point& value)
  : point_(value), offset_(0)
{
}

point_byte_iterator::point_byte_iterator(const point& value, bool end)
  : point_(value), offset_(end ? max_offset : 0)
{
}

point_byte_iterator::point_byte_iterator(const point& value, uint8_t offset)
  : point_(value), offset_(offset)
{
}

point_byte_iterator::operator bool() const
{
    return (offset_ < max_offset);
}

point_byte_iterator::reference point_byte_iterator::operator*() const
{
    uint8_t result = 0;

    if (offset_ < hash_size)
        result = point_.hash[offset_];
    else if (offset_ - hash_size < sizeof(uint32_t))
        result = to_little_endian(point_.index)[offset_ - hash_size];

    return result;
}

point_byte_iterator::pointer point_byte_iterator::operator->() const
{
    uint8_t result = 0;

    if (offset_ < hash_size)
        result = point_.hash[offset_];
    else if (offset_ - hash_size < sizeof(uint32_t))
        result = to_little_endian(point_.index)[offset_ - hash_size];

    return result;
}

bool point_byte_iterator::operator==(const iterator& other) const
{
    return (point_ == other.point_) && (offset_ == other.offset_);
}

bool point_byte_iterator::operator!=(const iterator& other) const
{
    return !(*this == other);
}

point_byte_iterator::iterator& point_byte_iterator::operator++()
{
    increment();
    return *this;
}

point_byte_iterator::iterator point_byte_iterator::operator++(int)
{
    auto it = *this;
    increment();
    return it;
}

point_byte_iterator::iterator& point_byte_iterator::operator--()
{
    decrement();
    return *this;
}

point_byte_iterator::iterator point_byte_iterator::operator--(int)
{
    auto it = *this;
    decrement();
    return it;
}

void point_byte_iterator::increment()
{
    if (offset_ < max_offset)
        offset_++;
}

void point_byte_iterator::decrement()
{
    if (offset_ > 0)
        offset_--;
}

}
}
