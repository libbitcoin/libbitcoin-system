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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_ENDPOINT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_ENDPOINT_HPP

#include <memory>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/config/url.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {
    
/// Container for a [scheme, host, port] tuple.
/// IPv6 URIs encoded with literal host (en.wikipedia.org/wiki/IPv6_address).
/// Provided for serialization of network endpoints in URI format.
class BC_API endpoint
{
public:
    typedef std::shared_ptr<endpoint> ptr;

    DEFAULT_COPY_MOVE_DESTRUCT(endpoint);

    endpoint() NOEXCEPT;

    /// The scheme and port may be undefined, in which case the port is
    /// reported as zero and the scheme is reported as an empty string.
    /// The value is of the form: [scheme://]host[:port] (dns name or ip).
    endpoint(const std::string& uri) THROWS;
    endpoint(const std::string& host, uint16_t port) NOEXCEPT;
    endpoint(const asio::endpoint& uri) NOEXCEPT;
    endpoint(const asio::address& ip, uint16_t port) NOEXCEPT;
    endpoint(const config::authority& authority) NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// The host name or ip address of the endpoint.
    const std::string& host() const NOEXCEPT;

    /// The tcp port of the endpoint.
    uint16_t port() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// The endpoint is of the form host[:port], empty port if zero (default).
    /// Specify non-zero default to explicitly serialize in case of default.
    std::string to_string(uint16_t default_port=0) const NOEXCEPT;
    std::string to_lower(uint16_t default_port=0) const NOEXCEPT;

    /// Return a new endpoint that replaces host instances of "*" with
    /// "localhost". This is intended for clients that wish to connect
    /// to a service that has been configured to bind to all interfaces.
    endpoint to_local() const NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    friend std::istream& operator>>(std::istream& input,
        endpoint& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& output,
        const endpoint& argument) NOEXCEPT;

private:
    // These are not thread safe.
    std::string scheme_;
    std::string host_;
    uint16_t port_;
};

/// Equality considers all properties (scheme, host, port).
BC_API bool operator==(const endpoint& left, const endpoint& right) NOEXCEPT;
BC_API bool operator!=(const endpoint& left, const endpoint& right) NOEXCEPT;

typedef std::vector<endpoint> endpoints;

} // namespace config
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::config::endpoint>
{
    size_t operator()(const bc::system::config::endpoint& value) const NOEXCEPT
    {
        return std::hash<std::string>{}(value.to_string());
    }
};
} // namespace std

#endif
