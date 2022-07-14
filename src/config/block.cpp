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
#include <bitcoin/system/config/block.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/config/base16.hpp>
/// DELETEMENOW
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

block::block() NOEXCEPT
  : value_()
{
}

block::block(chain::block&& value) NOEXCEPT
  : value_(std::move(value))
{
}

block::block(const chain::block& value) NOEXCEPT
  : value_(value)
{
}

block::block(const std::string& base16) THROWS
  : value_()
{
    std::istringstream(base16) >> *this;
}

block& block::operator=(chain::block&& value) NOEXCEPT
{
    value_ = std::move(value);
    return *this;
}

block& block::operator=(const chain::block& value) NOEXCEPT
{
    value_ = value;
    return *this;
}

bool block::operator==(const block& other) const NOEXCEPT
{
    return value_ == other.value_;
}

block::operator const chain::block&() const NOEXCEPT
{
    return value_;
}

std::string block::to_string() const NOEXCEPT
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& stream, block& argument) THROWS
{
    std::string base16;
    stream >> base16;

    data_chunk bytes;
    if (!decode_base16(bytes, base16))
        throw istream_exception(base16);

    argument.value_ = chain::block{ bytes, true };

    if (!argument.value_.is_valid())
        throw istream_exception(base16);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const block& argument) NOEXCEPT
{
    stream << encode_base16(argument.value_.to_data(true));
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
