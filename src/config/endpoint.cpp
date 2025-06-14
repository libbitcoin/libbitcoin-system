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
#include <bitcoin/system/config/endpoint.hpp>

#include <sstream>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/config/utilities.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace config {

// Contructors.
// ----------------------------------------------------------------------------

endpoint::endpoint() NOEXCEPT
  : endpoint({}, "localhost", {})
{
}

endpoint::endpoint(const std::string& uri) THROWS
  : endpoint()
{
    std::stringstream(uri) >> *this;
}

endpoint::endpoint(const std::string& host, uint16_t port) NOEXCEPT
  : endpoint({}, host, port)
{
}

endpoint::endpoint(const std::string& scheme, const std::string& host,
    uint16_t port) NOEXCEPT
  : scheme_(scheme), host_(host), port_(port)
{
}

endpoint::endpoint(const asio::endpoint& uri) NOEXCEPT
  : endpoint(uri.address(), uri.port())
{
}

endpoint::endpoint(const asio::address& ip, uint16_t port) NOEXCEPT
  : endpoint(config::to_host(ip), port)
{
}

endpoint::endpoint(const config::authority& authority) NOEXCEPT
  : endpoint(authority.ip(), authority.port())
{
}

// Properties.
// ----------------------------------------------------------------------------

const std::string& endpoint::scheme() const NOEXCEPT
{
    return scheme_;
}

const std::string& endpoint::host() const NOEXCEPT
{
    return host_;
}

uint16_t endpoint::port() const NOEXCEPT
{
    return port_;
}

// Methods.
// ----------------------------------------------------------------------------

std::string endpoint::to_uri() const NOEXCEPT
{
    std::stringstream value{};
    value << *this;
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return value.str();
    BC_POP_WARNING()
}

endpoint endpoint::to_local() const NOEXCEPT
{
    const auto host = (host_ == "*" ? "localhost" : host_);
    return endpoint(scheme_, host, port_);
}

// protected
std::string endpoint::to_authority() const NOEXCEPT
{
    return is_zero(port()) ? host() : host() + ":" + serialize(port());
}

// Operators.
// ----------------------------------------------------------------------------

bool operator==(const endpoint& left, const endpoint& right) NOEXCEPT
{
    return left.host() == right.host()
        && left.port() == right.port()
        && left.scheme() == right.scheme();
}

bool operator!=(const endpoint& left, const endpoint& right) NOEXCEPT
{
    return !(left == right);
}

std::istream& operator>>(std::istream& input,
    endpoint& argument) THROWS
{
    std::string value{};
    input >> value;

    if (!parse_endpoint(argument.scheme_, argument.host_, argument.port_, value))
        throw istream_exception(value);

    return input;
}

std::ostream& operator<<(std::ostream& output,
    const endpoint& argument) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    output
        << (argument.scheme().empty() ? "" : argument.scheme() + "://")
        << (argument.host())
        << (is_zero(argument.port()) ? "" : ":" + serialize(argument.port()));
    BC_POP_WARNING()
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
