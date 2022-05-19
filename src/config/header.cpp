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
#include <bitcoin/system/config/header.hpp>

#include <sstream>
#include <string>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/config/base16.hpp>
#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

header::header() noexcept
  : value_()
{
}

header::header(const chain::header& value) noexcept
  : value_(value)
{
}

header::header(const header& other) noexcept
  : header(other.value_)
{
}

header::header(const std::string& hexcode) noexcept(false)
  : value_()
{
    std::stringstream(hexcode) >> *this;
}

header::operator const chain::header&() const noexcept
{
    return value_;
}

std::istream& operator>>(std::istream& input, header& argument) noexcept(false)
{
    std::string hexcode;
    input >> hexcode;

    argument.value_ = chain::header(base16(hexcode));

    if (!argument.value_.is_valid())
        throw istream_exception(hexcode);

    return input;
}

std::ostream& operator<<(std::ostream& output, const header& argument) noexcept
{
    const auto bytes = argument.value_.to_data();

    output << base16(bytes);
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
