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
#ifndef LIBBITCOIN_CHAIN_POINT_VALUE_HPP
#define LIBBITCOIN_CHAIN_POINT_VALUE_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

/// A valued point, does not implement specialized serialization methods.
class BC_API point_value
  : public point
{
public:
    typedef std::vector<point_value> list;

    // Constructors.
    //-------------------------------------------------------------------------

    point_value()
      : point(), value_(0)
    {
    }

    point_value(point_value&& other)
      : point(std::move(other)), value_(other.value_)
    {
    }

    point_value(const point_value& other)
      : point(other), value_(other.value_)
    {
    }

    point_value(point&& point, uint64_t value)
      : point(std::move(point)), value_(value)
    {
    }

    point_value(const point& point, uint64_t value)
      : point(point), value_(value)
    {
    }

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
    point_value& operator=(point_value&& other)
    {
        static_cast<point>(*this) = std::move(static_cast<point>(other));
        value_ = other.value_;
        return *this;
    }

    point_value& operator=(const point_value& other)
    {
        static_cast<point>(*this) = static_cast<point>(other);
        value_ = other.value_;
        return *this;
    }

    bool operator==(const point_value& other) const
    {
        return static_cast<point>(*this) == static_cast<point>(other) &&
            (value_ == other.value_);
    }

    bool operator!=(const point_value& other) const
    {
        return !(*this == other);
    }

    // Properties (accessors).
    //-------------------------------------------------------------------------

    uint64_t value() const
    {
        return value_;
    }

    void set_value(uint64_t value)
    {
        value_ = value;
    }

private:
    uint64_t value_;
};

} // namespace chain
} // namespace libbitcoin

#endif
