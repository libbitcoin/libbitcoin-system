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
#include <bitcoin/system/config/hash160.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/crypto/crypto.hpp>
/// DELETEMENOW
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

hash160::hash160() NOEXCEPT
  : value_(null_short_hash)
{
}

hash160::hash160(short_hash&& value) NOEXCEPT
  : value_(std::move(value))
{
}

hash160::hash160(const short_hash& value) NOEXCEPT
    : value_(value)
{
}

hash160::hash160(const std::string& base16) THROWS
{
    std::istringstream(base16) >> *this;
}

hash160::operator const short_hash&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, hash160& argument) THROWS
{
    std::string base16;
    stream >> base16;

    if (!decode_base16(argument.value_, base16))
        throw istream_exception(base16);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash160& argument) NOEXCEPT
{
    stream << encode_base16(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
