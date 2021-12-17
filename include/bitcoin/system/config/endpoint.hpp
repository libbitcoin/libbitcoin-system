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

#ifdef MOVE_TO_NETWORK

#ifndef LIBBITCOIN_SYSTEM_CONFIG_ENDPOINT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_ENDPOINT_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Serialization helper for a network endpoint in URI format.
/// This is a container for a {scheme, host, port} tuple.
class BC_API endpoint
{
public:
    typedef std::vector<endpoint> list;

    endpoint();
    endpoint(const endpoint& other);

    /// The scheme and port may be undefined, in which case the port is
    // reported as zero and the scheme is reported as an empty string.
    /// The value is of the form: [scheme://]host[:port]
    endpoint(const std::string& value);
    endpoint(const authority& authority);

    /// host may be host name or ip address.
    endpoint(const std::string& host, uint16_t port);
    endpoint(const std::string& scheme, const std::string& host,
        uint16_t port);

    endpoint(const boost::asio::ip::tcp::endpoint& host);
    endpoint(const boost::asio::ip::address& ip, uint16_t port);

    /// True if the endpoint is initialized.
    operator bool() const;

    /// The scheme of the endpoint or empty string.
    const std::string& scheme() const;

    /// The host name or ip address of the endpoint.
    const std::string& host() const;

    /// The tcp port of the endpoint.
    uint16_t port() const;

    /// An empty scheme and/or empty port is omitted.
    /// The endpoint is of the form: [scheme://]host[:port]
    std::string to_string() const;

    /// Return a new endpoint that replaces host instances of "*" with
    /// "localhost". This is intended for clients that wish to connect
    /// to a service that has been configured to bind to all interfaces.
    /// The endpoint is of the form: [scheme://]host[:port]
    endpoint to_local() const;
    bool operator==(const endpoint& other) const;

    friend std::istream& operator>>(std::istream& input,
        endpoint& argument);
    friend std::ostream& operator<<(std::ostream& output,
        const endpoint& argument);

private:
    std::string scheme_;
    std::string host_;
    uint16_t port_;
};

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif

#endif // MOVE_TO_NETWORK
