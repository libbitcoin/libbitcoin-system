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
#include <bitcoin/system/config/base16.hpp>

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

base16::base16() NOEXCEPT
{
}

base16::base16(data_chunk&& value) NOEXCEPT
  : value_(std::move(value))
{
}

base16::base16(const data_chunk& value) NOEXCEPT
  : value_(value)
{
}

base16::base16(const std::string& base16) THROWS
{
    std::istringstream(base16) >> *this;
}

base16::operator const data_chunk&() const NOEXCEPT
{
    return value_;
}

////base16::operator data_slice() const NOEXCEPT
////{
////    return { value_.begin(), value_.end() };
////}

std::istream& operator>>(std::istream& stream, base16& argument) THROWS
{
    std::string base16;
    stream >> base16;

    if (!decode_base16(argument.value_, base16))
    {
        using namespace boost::program_options;
        throw istream_exception(base16);
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const base16& argument) NOEXCEPT
{
    stream << encode_base16(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
