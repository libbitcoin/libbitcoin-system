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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_AUTHORITY_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_AUTHORITY_HPP

#include <memory>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Container for an [ip-address, port, CIDR] tuple.
/// Subnet matching is employed when nonzero CIDR suffix is present.
/// Internal storage always denormalized to native IPv4/IPv6 (no mapped IPv6).
/// Provided for connection management (not p2p network messaging).
class BC_API authority
{
public:
    typedef std::shared_ptr<authority> ptr;

    DEFAULT_COPY_MOVE_DESTRUCT(authority);

    authority() NOEXCEPT;

    /// Deserialize [IPv6]|IPv4[:port][/cidr] (IPv6 [literal]).
    authority(const std::string& authority) THROWS;
    authority(const asio::address& ip, uint16_t port, uint8_t cidr=0) NOEXCEPT;
    authority(const asio::endpoint& endpoint) NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// The IPv4 or IPv6 address, denormalized (no mapped IPv6).
    const asio::address& ip() const NOEXCEPT;

    /// The ip port of the authority.
    uint16_t port() const NOEXCEPT;

    /// The ip subnet mask in cidr format (zero implies none).
    uint8_t cidr() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------
    /// All serializations are denormalized (IPv6 or IPv4).

    /// The IPv4 or IPv6 address and port as an asio endpoint.
    asio::endpoint to_endpoint() const NOEXCEPT;

    /// IPv6|IPv4
    std::string to_host() const NOEXCEPT;

    /// [IPv6]|IPv4
    std::string to_literal() const NOEXCEPT;

    /// Serialize [IPv6]|IPv4[:port][/cidr] (IPv6 [literal]).
    std::string to_string() const NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    /// Same format as construct(string) and to_string().
    friend std::istream& operator>>(std::istream& input,
        authority& argument) THROWS;
    friend std::ostream& operator<<(std::ostream& output,
        const authority& argument) NOEXCEPT;

private:
    // These are not thread safe.
    asio::address ip_;
    uint16_t port_;
    uint8_t cidr_;
};

/// Equality treats zero port as * and non-zero CIDR as subnet identifier.
/// Equality is subnet containment when one subnet identifier is present.
/// Distinct subnets are unequal even if intersecting, same subnets equal.
BC_API bool operator==(const authority& left, const authority& right) NOEXCEPT;
BC_API bool operator!=(const authority& left, const authority& right) NOEXCEPT;

typedef std::vector<authority> authorities;

} // namespace config
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::config::authority>
{
    size_t operator()(const bc::system::config::authority& value) const NOEXCEPT
    {
        return std::hash<std::string>{}(value.to_literal());
    }
};
} // namespace std

#endif
