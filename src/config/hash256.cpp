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
#include <bitcoin/system/config/hash256.hpp>

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

hash256::hash256() NOEXCEPT
  : value_(null_hash)
{
}

hash256::hash256(hash_digest&& value) NOEXCEPT
  : value_(std::move(value))
{
}

hash256::hash256(const hash_digest& value) NOEXCEPT
  : value_(value)
{
}

hash256::hash256(const std::string& base16) THROWS
{
    std::istringstream(base16) >> *this;
}

std::string hash256::to_string() const NOEXCEPT
{
    std::ostringstream value;
    value << *this;
    return value.str();
}

hash256::operator const hash_digest&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, hash256& argument) THROWS
{
    std::string base16;
    stream >> base16;

    if (!decode_hash(argument.value_, base16))
        throw istream_exception(base16);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash256& argument) NOEXCEPT
{
    stream << encode_hash(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
