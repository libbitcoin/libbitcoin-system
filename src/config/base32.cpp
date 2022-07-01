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
#include <bitcoin/system/config/base32.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/data/data.hpp>
// DELETEMENOW
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base32::base32() NOEXCEPT
{
}

base32::base32(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base32::base32(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base32::base32(const std::string& base32) THROWS
{
    std::istringstream(base32) >> *this;
}

base32::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base32& argument) THROWS
{
    std::string base32;
    stream >> base32;

    if (!decode_base32(argument.value_, base32))
        throw istream_exception(base32);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base32& argument) NOEXCEPT
{
    stream << encode_base32(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
