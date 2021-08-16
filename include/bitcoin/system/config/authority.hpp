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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_AUTHORITY_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_AUTHORITY_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/messages/network_address.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// This is a container for a {ip address, port} tuple.
class BC_API authority
{
public:
    typedef std::vector<authority> list;

    authority();
    authority(const authority& other);

    /// Deserialize a IPv4 or IPv6 address-based hostname[:port].
    /// The port is optional and will be set to zero if not provided.
    /// The host can be in one of two forms:
    /// [2001:db8::2]:port or 1.2.240.1:port.
    authority(const std::string& authority);

    authority(const messages::network_address& address);
    authority(const messages::ip_address& ip, uint16_t port);

    /// The host can be in one of three forms:
    /// [2001:db8::2] or 2001:db8::2 or 1.2.240.1
    authority(const std::string& host, uint16_t port);
    authority(const boost::asio::ip::address& ip, uint16_t port);
    authority(const boost::asio::ip::tcp::endpoint& endpoint);

    /// True if the port is non-zero.
    operator bool() const;

    /// The ip address of the authority.
    boost::asio::ip::address_v6 asio_ip() const;

    /// The ip address of the authority.
    messages::ip_address ip() const;

    /// The tcp port of the authority.
    uint16_t port() const;

    /// The hostname of the authority as a string.
    /// The form of the return is determined by the type of address, either:
    /// 2001:db8::2 or 1.2.240.1
    std::string to_hostname() const;

    /// The authority as a string.
    /// The form of the return is determined by the type of address.
    /// The port is optional and not included if zero-valued.
    /// The authority in one of two forms: [2001:db8::2]:port or 1.2.240.1:port
    std::string to_string() const;

    /// The authority converted to a network address.
    messages::network_address to_network_address() const;

    bool operator==(const authority& other) const;
    bool operator!=(const authority& other) const;

    friend std::istream& operator>>(std::istream& input,
        authority& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const authority& argument);

private:
    boost::asio::ip::address_v6 ip_;
    uint16_t port_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
