/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/config/endpoint.hpp>

#include <cstdint>
#include <iostream>
#include <regex>
#include <string>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {
namespace config {
    
using namespace boost;
using namespace boost::program_options;

endpoint_type::endpoint_type()
  : port_(0)
{
}

endpoint_type::endpoint_type(const std::string& value)
  : endpoint_type()
{
    std::stringstream(value) >> *this;
}

endpoint_type::endpoint_type(const endpoint_type& other)
  : scheme_(other.get_scheme()), host_(other.get_host()),
    port_(other.get_port())
{
}

const std::string& endpoint_type::get_scheme() const
{
    return scheme_;
}

const std::string& endpoint_type::get_host() const
{
    return host_;
}

uint16_t endpoint_type::get_port() const
{
    return port_;
}

endpoint_type::operator const std::string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

std::istream& operator>>(std::istream& input, endpoint_type& argument)
{
    std::string value;
    input >> value;

    // std::regex requires gcc 4.9, so we are using boost::regex for now.
    // When matched will always generate 6 tokens, we want 2, 3 and 5.
    const boost::regex regular(
        "((tcp|udp):\\/\\/)?([0-9a-z\\.\\*-]+)(:([0-9]{1,5}))?");

    try 
    {
        boost::sregex_iterator it(value.begin(), value.end(), regular), end;

        // No match?
        if (it == end)
        {
            BOOST_THROW_EXCEPTION(invalid_option_value(value));
        }

        boost::smatch match = *it;

        // Extract the three tokens of interest.
        argument.scheme_ = match[2];
        argument.host_ = match[3];
        const std::string number(match[5]);

        if (!number.empty())
            argument.port_ = boost::lexical_cast<uint16_t>(number);
    }
    catch (const boost::exception&)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }
    catch (const std::exception&)
    {
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return input;
}

std::ostream& operator<<(std::ostream& output, const endpoint_type& argument)
{
    if (!argument.scheme_.empty())
        output << argument.scheme_ << "://";

    output << argument.host_;

    if (argument.port_ != 0)
        output << ":" << argument.port_;

    return output;
}

} // namespace config
} // namespace libbitcoin
