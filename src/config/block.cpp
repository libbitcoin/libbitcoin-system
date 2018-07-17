/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/config/block.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/config/base16.hpp>
#include <bitcoin/bitcoin/settings.hpp>

namespace libbitcoin {
namespace config {

block::block(const libbitcoin::settings& settings)
  : value_(settings)
{
}

block::block(const std::string& hexcode, const libbitcoin::settings& settings)
  : value_(settings)
{
    std::stringstream(hexcode) >> *this;
}

block::block(const chain::block& value)
  : value_(value)
{
}

block::block(const block& other)
  : block(other.value_)
{
}

bool block::operator==(const block& other) const
{
    return value_ == other.value_;
}

block::operator const chain::block&() const
{
    return value_;
}

std::string block::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, block& argument)
{
    std::string hexcode;
    input >> hexcode;

    if (!argument.value_.from_data(base16(hexcode)))
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(hexcode));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const block& argument)
{
    const auto bytes = argument.value_.to_data();

    output << base16(bytes);
    return output;
}

} // namespace config
} // namespace libbitcoin
