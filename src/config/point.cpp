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
#include <bitcoin/system/config/point.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/config/hash256.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
// DELETEMENOW
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

const std::string point::delimiter = ":";

// Point format is currently private to bx:
// "txhash:index"

static bool decode_point(chain::point& point,
    const std::string& tuple) THROWS
{
    uint32_t index;
    const auto tokens = split(tuple, point::delimiter);
    if (tokens.size() != 2 || !deserialize(index, tokens[1]))
        return false;

    // Throws istream_exception.
    point = chain::point
    {
        hash256{ tokens[0] },
        index
    };

    return true;
}

static std::string encode_point(const chain::point& point) NOEXCEPT
{
    std::ostringstream result;
    result << hash256(point.hash()) << point::delimiter << point.index();
    return result.str();
}

point::point() NOEXCEPT
  : value_()
{
}

point::point(chain::point&& value) NOEXCEPT
  : value_(std::move(value))
{
}

point::point(const chain::point& value) NOEXCEPT
  : value_(value)
{
}

point::point(const std::string& tuple) THROWS
{
    std::istringstream(tuple) >> *this;
}

point::operator const chain::point&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, point& argument) THROWS
{
    std::string tuple;
    stream >> tuple;

    if (!decode_point(argument.value_, tuple))
        throw istream_exception(tuple);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const point& argument) NOEXCEPT
{
    stream << encode_point(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
