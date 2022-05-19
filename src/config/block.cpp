/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

#include <sstream>
#include <string>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/config/base16.hpp>
#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {
namespace system {
namespace config {

block::block() noexcept
  : value_()
{
}

block::block(const chain::block& value) noexcept
  : value_(value)
{
}

block::block(const block& other) noexcept
  : block(other.value_)
{
}

block::block(const std::string& hexcode) noexcept(false)
  : value_()
{
    std::stringstream(hexcode) >> *this;
}

block& block::operator=(const block& other) noexcept
{
    value_ = chain::block(other.value_);
    return *this;
}

block& block::operator=(chain::block&& other) noexcept
{
    value_ = std::move(other);
    return *this;
}

bool block::operator==(const block& other) const noexcept
{
    return value_ == other.value_;
}

block::operator const chain::block&() const noexcept
{
    return value_;
}

std::string block::to_string() const noexcept
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, block& argument) noexcept(false)
{
    std::string hexcode;
    input >> hexcode;

    argument.value_ = chain::block{ base16(hexcode), true };

    if (!argument.value_.is_valid())
        throw istream_exception(hexcode);

    return input;
}

std::ostream& operator<<(std::ostream& output, const block& argument) noexcept
{
    const auto bytes = argument.value_.to_data(true);

    output << base16(bytes);
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
