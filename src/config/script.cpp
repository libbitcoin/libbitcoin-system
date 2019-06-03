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
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost;
using namespace boost::program_options;

script::script()
  : value_()
{
}

script::script(const std::string& mnemonic)
{
    std::stringstream(mnemonic) >> *this;
}

script::script(const chain::script& value)
  : value_(value)
{
}

script::script(const data_chunk& value)
{
    value_.from_data(value, false);
}

script::script(const std::vector<std::string>& tokens)
{
    const auto mnemonic = join(tokens);
    std::stringstream(mnemonic) >> *this;
}

script::script(const script& other)
  : script(other.value_)
{
}

data_chunk script::to_data() const
{
    return value_.to_data(false);
}

std::string script::to_string(uint32_t flags) const
{
    return value_.to_string(flags);
}

script::operator const chain::script&() const
{
    return value_;
}

std::istream& operator>>(std::istream& input, script& argument)
{
    std::istreambuf_iterator<char> end;
    std::string mnemonic(std::istreambuf_iterator<char>(input), end);
    boost::trim(mnemonic);

    // Test for invalid result sentinel.
    if (!argument.value_.from_string(mnemonic) && mnemonic.length() > 0)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(mnemonic));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const script& argument)
{
    static constexpr auto flags = machine::rule_fork::all_rules;
    output << argument.value_.to_string(flags);
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
