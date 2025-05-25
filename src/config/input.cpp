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
#include <bitcoin/system/config/input.hpp>

#include <sstream>
#include <utility>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/config/point.hpp>
#include <bitcoin/system/config/script.hpp>
#include <bitcoin/system/serial/deserialize.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

// Input format is currently private to bx:
// "script:txhash:index:sequence=max_input_sequence"

static bool decode_input(chain::input& input,
    const std::string& tuple) THROWS
{
    const auto tokens = split(tuple, point::delimiter);
    if (tokens.size() != 3 && tokens.size() != 4)
        return false;

    auto sequence = chain::max_input_sequence;
    if (tokens.size() == 4 && !deserialize(sequence, tokens[3]))
        return false;

    // Throws istream_exception.
    input = chain::input
    {
        point{ tokens[1] + point::delimiter + tokens[2] },
        script{ tokens[0] },
        sequence
    };

    return true;
}

static std::string encode_input(const chain::input& input) NOEXCEPT
{
    std::ostringstream result;
    result << script(input.script()) << point(input.point())
        << point::delimiter << input.sequence();
    return result.str();
}

input::input() NOEXCEPT
  : chain::input()
{
}

input::input(chain::input&& value) NOEXCEPT
  : chain::input(std::move(value))
{
}

input::input(const chain::input& value) NOEXCEPT
  : chain::input(value)
{
}

input::input(const std::string& tuple) THROWS
  : input()
{
    std::istringstream(tuple) >> *this;
}

std::istream& operator>>(std::istream& stream, input& argument) THROWS
{
    std::string tuple;
    stream >> tuple;

    if (!decode_input(argument, tuple))
        throw istream_exception(tuple);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const input& argument) NOEXCEPT
{
    stream << encode_input(argument);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
