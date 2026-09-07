/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/config/base32b.hpp>

#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base32b::base32b() NOEXCEPT
{
}

base32b::base32b(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base32b::base32b(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base32b::base32b(const std::string& base32) THROWS
{
    std::istringstream(base32) >> *this;
}

base32b::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base32b& argument) THROWS
{
    std::string base32;
    stream >> base32;

    if (!decode_base32b(argument.value_, base32))
        throw istream_exception(base32);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base32b& argument) NOEXCEPT
{
    stream << encode_base32b(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
