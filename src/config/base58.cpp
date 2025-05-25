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
#include <bitcoin/system/config/base58.hpp>

#include <sstream>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base58::base58() NOEXCEPT
{
}

base58::base58(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base58::base58(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base58::base58(const std::string& base58) THROWS
{
    std::istringstream(base58) >> *this;
}

base58::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base58& argument) THROWS
{
    std::string base58;
    stream >> base58;

    if (!decode_base58(argument.value_, base58))
        throw istream_exception(base58);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base58& argument) NOEXCEPT
{
    stream << encode_base58(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
