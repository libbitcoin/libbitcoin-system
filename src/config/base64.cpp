/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/base64.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base64::base64() NOEXCEPT
{
}

base64::base64(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base64::base64(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base64::base64(const std::string& base64) THROWS
{
    std::istringstream(base64) >> *this;
}

base64::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base64& argument) THROWS
{
    std::string base64;
    stream >> base64;

    if (!decode_base64(argument.value_, base64))
        throw istream_exception(base64);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base64& argument) NOEXCEPT
{
    stream << encode_base64(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
