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
#include <bitcoin/system/wallet/point_value.hpp>

#include <utility>
#include <vector>
#include <bitcoin/system/chain/chain.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// Constructors.
//-------------------------------------------------------------------------

point_value::point_value() NOEXCEPT
  : point(), value_(0)
{
}

point_value::point_value(point&& instance, uint64_t value) NOEXCEPT
  : point(std::move(instance)), value_(value)
{
}

point_value::point_value(const point& instance, uint64_t value) NOEXCEPT
  : point(instance), value_(value)
{
}

// Operators.
//-------------------------------------------------------------------------

bool point_value::operator==(const point_value& other) const NOEXCEPT
{
    return static_cast<point>(*this) == static_cast<point>(other) &&
        (value_ == other.value_);
}

bool point_value::operator!=(const point_value& other) const NOEXCEPT
{
    return !(*this == other);
}

// Properties (accessors).
//-------------------------------------------------------------------------

uint64_t point_value::value() const NOEXCEPT
{
    return value_;
}

void point_value::set_value(uint64_t value) NOEXCEPT
{
    value_ = value;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
