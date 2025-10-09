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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_URL_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_URL_HPP

#include <memory>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {
    
/// Container for a [scheme, host, port] tuple (URLs).
/// IPv6 URIs encoded with literal host (en.wikipedia.org/wiki/IPv6_address).
/// Provided for serialization of network URLs such as for ZeroMQ.
class BC_API url
{
public:
    typedef std::shared_ptr<url> ptr;

    DEFAULT_COPY_MOVE_DESTRUCT(url);

    url() NOEXCEPT;

    /// The scheme and port may be undefined, in which case the port is
    /// reported as zero and the scheme is reported as an empty string.
    /// The value is of the form: [scheme://]host[:port] (dns name or ip).
    url(const std::string& uri) THROWS;
    url(const std::string& host, uint16_t port) NOEXCEPT;
    url(const std::string& scheme, const std::string& host,
        uint16_t port) NOEXCEPT;
    url(const asio::endpoint& uri) NOEXCEPT;
    url(const asio::address& ip, uint16_t port) NOEXCEPT;
    url(const config::authority& authority) NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// The scheme of the url or empty string.
    const std::string& scheme() const NOEXCEPT;

    /// The host name or ip address of the url.
    const std::string& host() const NOEXCEPT;

    /// The tcp port of the url.
    uint16_t port() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// An empty scheme and/or empty (zero) port is omitted.
    /// The url is of the form: [scheme://]host[:port]
    std::string to_uri() const NOEXCEPT;

    /// Return a new url that replaces host instances of "*" with "localhost".
    /// This is intended for clients that wish to connect to a service that has
    /// been configured to bind to all interfaces.
    url to_local() const NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    friend std::istream& operator>>(std::istream& input,
        url& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& output,
        const url& argument) NOEXCEPT;

protected:
    std::string to_authority() const NOEXCEPT;

private:
    // These are not thread safe.
    std::string scheme_;
    std::string host_;
    uint16_t port_;
};

/// Equality considers all properties (scheme, host, port).
BC_API bool operator==(const url& left, const url& right) NOEXCEPT;
BC_API bool operator!=(const url& left, const url& right) NOEXCEPT;

typedef std::vector<url> urls;

} // namespace config
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::config::url>
{
    size_t operator()(const bc::system::config::url& value) const NOEXCEPT
    {
        return std::hash<std::string>{}(value.to_uri());
    }
};
} // namespace std

#endif
