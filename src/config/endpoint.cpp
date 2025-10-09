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
#include <bitcoin/system/config/url.hpp>
#include <bitcoin/system/config/utilities.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace config {

// Contructors.
// ----------------------------------------------------------------------------

endpoint::endpoint() NOEXCEPT
  : endpoint("localhost", {})
{
}

endpoint::endpoint(const std::string& uri) THROWS
  : endpoint()
{
    std::stringstream(uri) >> *this;
}

endpoint::endpoint(const std::string& host, uint16_t port) NOEXCEPT
  : host_(host), port_(port)
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

endpoint endpoint::to_local() const NOEXCEPT
{
    const auto host = (host_ == "*" ? "localhost" : host_);
    return { host, port_ };
}

std::string endpoint::to_string(uint16_t default_port) const NOEXCEPT
{
    const auto value = is_zero(port()) ? default_port : port();
    return is_zero(value) ? host() : host() + ":" + serialize(value);
}

std::string endpoint::to_lower(uint16_t default_port) const NOEXCEPT
{
    return system::ascii_to_lower(to_string(default_port));
}

// Operators.
// ----------------------------------------------------------------------------

bool operator==(const endpoint& left, const endpoint& right) NOEXCEPT
{
    return left.host() == right.host()
        && left.port() == right.port();
}

bool operator!=(const endpoint& left, const endpoint& right) NOEXCEPT
{
    return !(left == right);
}

std::istream& operator>>(std::istream& input, endpoint& argument) THROWS
{
    std::string value{};
    input >> value;

    if (!parse_endpoint(argument.host_, argument.port_, value))
        throw istream_exception(value);

    return input;
}

std::ostream& operator<<(std::ostream& output,
    const endpoint& argument) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    output << argument.to_string();
    BC_POP_WARNING()
    return output;
}

} // namespace config
} // namespace system
} // namespace libbitcoin
