/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/script.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {
namespace config {

script::script() NOEXCEPT
  : chain::script()
{
}

script::script(chain::script&& value) NOEXCEPT
  : chain::script(std::move(value))
{
}

script::script(const chain::script& value) NOEXCEPT
  : chain::script(value)
{
}

script::script(const data_chunk& value) NOEXCEPT
  : chain::script(value, false)
{
}

script::script(const std::vector<std::string>& tokens) THROWS
  : script(join(tokens))
{
}

script::script(const std::string& mnemonic)
{
    std::istringstream(mnemonic) >> *this;
}

std::istream& operator>>(std::istream& stream, script& argument) THROWS
{
    std::istreambuf_iterator<char> end;
    std::string mnemonic(std::istreambuf_iterator<char>(stream), end);

    argument = chain::script{ mnemonic };

    if (!argument.is_valid())
        throw istream_exception(mnemonic);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const script& argument) NOEXCEPT
{
    stream << argument.to_string(chain::flags::all_rules);
    return stream;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
