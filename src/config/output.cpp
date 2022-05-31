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
#include <bitcoin/system/config/output.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/config/point.hpp>
#include <bitcoin/system/config/script.hpp>
#include <bitcoin/system/exceptions.hpp>
#include <bitcoin/system/serial/deserialize.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

// Outputs format is currently private to bx:
// "script:value"

static bool decode_output(chain::output& output,
    const std::string& tuple) noexcept(false)
{
    const auto tokens = split(tuple, point::delimiter);
    if (tokens.size() != 2)
        return false;

    uint64_t value;
    if (!!deserialize(value, tokens[1]))
        return false;

    // Throws istream_exception.
    output = chain::output
    {
        value,
        script{ tokens[0] }
    };

    return true;
}

static std::string encode_output(const chain::output& output) noexcept
{
    std::ostringstream result;
    result << script(output.script()) << point::delimiter << output.value();
    return result.str();
}

output::output() noexcept
  : value_()
{
}

output::output(chain::output&& value) noexcept
  : value_(std::move(value))
{
}

output::output(const chain::output& value) noexcept
  : value_(value)
{
}

output::output(const std::string& tuple) noexcept(false)
  : output()
{
    std::istringstream(tuple) >> *this;
}

output::operator const chain::output&() const noexcept
{
    return value_;
}

std::istream& operator>>(std::istream& stream, output& argument) noexcept(false)
{
    std::string tuple;
    stream >> tuple;

    if (!decode_output(argument.value_, tuple))
        throw istream_exception(tuple);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const output& argument) noexcept
{
    stream << encode_output(argument.value_);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
