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
#include <bitcoin/bitcoin/network/authority.hpp>

#include <cstdint>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

namespace libbitcoin {
namespace network {
    
using boost::format;
using boost::asio::ip::tcp;

authority::authority()
  : port(0)
{
}

// This is the format utilized by the seed list.
authority::authority(const std::string& line)
  : authority()
{
    const auto parts = split(boost::trim_copy(line), ":");
    host = parts[0];

    // TODO: verify lexical_cast ndebug exception safety.
    if (parts.size() == 2)
        port = boost::lexical_cast<uint16_t>(parts[1]);

    // The port was invalid or unspecified.
    BITCOIN_ASSERT(port != 0);
}

// This is the format returned from peers on the bitcoin network.
authority::authority(const message::network_address& net)
{
    const auto& ip = net.ip;
    const auto formatted = format("%d.%d.%d.%d") %
        (int)ip[12] % (int)ip[13] % (int)ip[14] % (int)ip[15];
    host = formatted.str();
    port = net.port;
}

// This is the manual construction expected format.
authority::authority(const std::string& host, uint16_t port)
  : host(host), port(port)
{
    // Can't explicitly set an invalid port.
    BITCOIN_ASSERT(port != 0);
}

// This is the format obtained from a boost asio channel.
authority::authority(const tcp::socket::endpoint_type& endpoint)
{
    host = endpoint.address().to_string();
    port = endpoint.port();
}

bool authority::operator==(const authority& other) const
{
    // Note that addresses are not inherently normalized.
    return host == other.host && port == other.port;
}

// This produces a pretty format even if the address is invalid.
std::string authority::to_string() const
{
    if (host.empty())
        return (format("0.0.0.0:%d") % port).str();

    return (format("%s:%d") % host % port).str();
}

} // namespace network
} // namespace libbitcoin

