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
#include <bitcoin/system/config/script.hpp>

#include <sstream>
#include <string>
#include <vector>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {
namespace system {
namespace config {

script::script() noexcept
  : value_()
{
}

script::script(const script& other) noexcept
  : script(other.value_)
{
}

script::script(const chain::script& value) noexcept
  : value_(value)
{
}

script::script(const data_chunk& value) noexcept
{
    value_ = chain::script(value, false);
}

script::script(const std::vector<std::string>& tokens) noexcept(false)
  : script(join(tokens))
{
}

script::script(const std::string& mnemonic)
{
    std::stringstream(mnemonic) >> *this;
}

data_chunk script::to_data() const noexcept
{
    return value_.to_data(false);
}

std::string script::to_string(uint32_t flags) const noexcept
{
    return value_.to_string(flags);
}

script::operator const chain::script&() const noexcept
{
    return value_;
}

std::istream& operator>>(std::istream& input, script& argument) noexcept(false)
{
    std::istreambuf_iterator<char> end;
    std::string mnemonic(std::istreambuf_iterator<char>(input), end);

    argument.value_ = chain::script(mnemonic);

    if (!argument.value_.is_valid())
        throw istream_exception(mnemonic);

    return input;
}

std::ostream& operator<<(std::ostream& output, const script& argument) noexcept
{
    static constexpr auto flags = chain::forks::all_rules;
    output << argument.value_.to_string(flags);
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
