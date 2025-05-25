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
#include <bitcoin/system/config/base2.hpp>

#include <sstream>
#include <utility>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace config {

base2::base2() NOEXCEPT
{
}

base2::base2(binary&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base2::base2(const binary& value) NOEXCEPT
  : value_(value)
{
}

base2::base2(const std::string& binary) THROWS
{
    std::istringstream(binary) >> *this;
}

base2::operator const binary&() const NOEXCEPT
{
    return value_;
}

std::istream& operator>>(std::istream& stream, base2& argument) THROWS
{
    std::string binary;
    stream >> binary;

    if (!binary::is_base2(binary))
        throw istream_exception(binary);

    std::istringstream(binary) >> argument.value_;
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base2& argument) NOEXCEPT
{
    stream << argument.value_;
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
