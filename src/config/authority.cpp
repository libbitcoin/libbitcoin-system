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
#include <bitcoin/system/config/authority.hpp>

#include <sstream>
#include <bitcoin/system/config/utilities.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace config {

// Contructors.
// ----------------------------------------------------------------------------

// Default authority is IPv6 unspecified (not IPv6-mapped unspecified).
authority::authority() NOEXCEPT
  : authority(boost::asio::ip::address_v6{}, {})
{
}

// Deserialzation does not map IPv4 and does not support mapped encoding.
authority::authority(const std::string& authority) THROWS
  : ip_{}, port_{}, cidr_{}
{
    std::stringstream(authority) >> *this;
}

// This allows unusable CIDR values (ok).
// IPv6-mapped IPv4 are normalized to IPv4.
authority::authority(const asio::address& ip, uint16_t port,
    uint8_t cidr) NOEXCEPT
  : ip_(config::denormalize(ip)), port_(port), cidr_(cidr)
{
}

authority::authority(const asio::endpoint& endpoint) NOEXCEPT
  : authority(endpoint.address(), endpoint.port())
{
}

// Properties.
// ----------------------------------------------------------------------------

const asio::address& authority::ip() const NOEXCEPT
{
    return ip_;
}

uint16_t authority::port() const NOEXCEPT
{
    return port_;
}

uint8_t authority::cidr() const NOEXCEPT
{
    return cidr_;
}

// Methods.
// ----------------------------------------------------------------------------

asio::endpoint authority::to_endpoint() const NOEXCEPT
{
    return { ip(), port() };
}

std::string authority::to_host() const NOEXCEPT
{
    return config::to_host(ip());
}

std::string authority::to_literal() const NOEXCEPT
{
    return config::to_literal(ip());
}

std::string authority::to_string() const NOEXCEPT
{
    std::stringstream value{};
    value << *this;
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return value.str();
    BC_POP_WARNING()
}

// Operators.
// ----------------------------------------------------------------------------

bool operator==(const authority& left, const authority& right) NOEXCEPT
{
    // both non-zero ports must match (zero/non-zero or both zero are matched).
    if ((!is_zero(left.port()) && !is_zero(right.port())) &&
        left.port() != right.port())
        return false;

    // both non-zero cidrs must match.
    if ((!is_zero(left.cidr()) && !is_zero(right.cidr())) &&
        left.cidr() != right.cidr())
        return false;

    // same cidrs, match ips only.
    if (left.cidr() == right.cidr())
        return left.ip() == right.ip();

    // one zero (host) and one non-zero (subnet) cidr, match host to subnet.
    return is_zero(left.cidr()) ?
        config::is_member(left.ip(), right.ip(), right.cidr()) :
        config::is_member(right.ip(), left.ip(), left.cidr());
}

bool operator!=(const authority& left, const authority& right) NOEXCEPT
{
    return !(left == right);
}

// This allows unusable CIDR values (ok).
std::istream& operator>>(std::istream& input,
    authority& argument) THROWS
{
    std::string value{};
    input >> value;

    if (!parse_authority(argument.ip_, argument.port_, argument.cidr_, value))
        throw istream_exception(value);

    return input;
}

// This allows unusable CIDR values (ok).
std::ostream& operator<<(std::ostream& output,
    const authority& argument) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    output
        << argument.to_literal()
        << (!is_zero(argument.port()) ? ":" + serialize(argument.port()) : "")
        << (!is_zero(argument.cidr()) ? "/" + serialize(argument.cidr()) : "");
    BC_POP_WARNING()
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
