/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_IP_ENDPOINT_HPP
#define LIBBITCOIN_SYSTEM_IP_ENDPOINT_HPP

#include <compare>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/address.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

/// IP address and port.
class BC_API endpoint
{
public:
    DEFAULT_COPY_MOVE(endpoint);

    /// Unspecified IPv4 address, port zero.
    endpoint() NOEXCEPT;
    endpoint(const ip::address& ip, uint16_t port) NOEXCEPT;

    const ip::address& address() const NOEXCEPT;
    uint16_t port() const NOEXCEPT;

    bool operator==(const endpoint& other) const NOEXCEPT = default;
    std::strong_ordering operator<=>(const endpoint& other) const NOEXCEPT = default;

private:
    ip::address address_;
    uint16_t port_;
};

} // namespace ip
} // namespace system
} // namespace libbitcoin

#endif
