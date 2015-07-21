/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/config/authority.hpp>

#include <cstdint>
#include <string>
#include <sstream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

namespace libbitcoin {
namespace config {

using namespace boost;
using namespace boost::asio;
using namespace boost::program_options;

authority::authority()
  : authority(undefined_ip_address, 0)
{
}

authority::authority(const authority& other)
  : authority(other.ip_, other.port_)
{
}

// This supports both IPv4 and IPv6 formats.
authority::authority(const std::string& value)
  : authority()
{
    std::stringstream(value) >> *this;
}

// This is the format returned from peers on the bitcoin network.
authority::authority(const network_address_type& net)
  : authority(net.ip, net.port)
{
}

authority::authority(const ip_address_type& ip, uint16_t port)
  : ip_(ip::address_v6(ip)), port_(port)
{
}

authority::authority(const ip::tcp::endpoint& endpoint)
  : authority(endpoint.address(), endpoint.port())
{
}

authority::authority(const ip::address& ip, uint16_t port)
  : ip_(ip), port_(port)
{
}

const ip_address_type& authority::ip() const
{
    return ip_.to_v6().to_bytes();
}

uint16_t authority::port() const
{
    return port_;
}

std::string authority::to_hostname() const
{
    return ip_.to_string();
}

network_address_type authority::to_network_address() const
{
    static constexpr uint32_t services = 0;
    static constexpr uint64_t timestamp = 0;
    const network_address_type network_address
    {
        timestamp,
        services,
        ip_.to_v6().to_bytes(),
        port_,
    };

    return network_address;
}

std::string authority::to_string() const
{
    std::stringstream value;
    value << *this;
    return value.str();
}

bool authority::operator==(const authority& other) const
{
    return ip() == other.ip() && port() == other.port();
}

std::istream& operator>>(std::istream& input, authority& argument)
{
    std::string value;
    input >> value;

    // This supports both IPv4 and IPv6 formats.
    const auto parts = split(trim_copy(value), ":");
    argument.ip_.from_string(parts[0]);

    if (parts.size() == 2)
        argument.port_ = lexical_cast<uint16_t>(parts[1]);

    return input;
}

std::ostream& operator<<(std::ostream& output, const authority& argument)
{
    output << argument.to_hostname();

    if (argument.port() != 0)
        output << ":" << argument.port();

    return output;
}

} // namespace config
} // namespace libbitcoin
