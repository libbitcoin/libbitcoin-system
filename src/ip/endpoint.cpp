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
#include <bitcoin/system/ip/endpoint.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/ip/address.hpp>

namespace libbitcoin {
namespace system {
namespace ip {

endpoint::endpoint() NOEXCEPT
  : address_{}, port_{}
{
}

endpoint::endpoint(const ip::address& ip, uint16_t port) NOEXCEPT
  : address_{ ip }, port_{ port }
{
}

const ip::address& endpoint::address() const NOEXCEPT
{
    return address_;
}

uint16_t endpoint::port() const NOEXCEPT
{
    return port_;
}

} // namespace ip
} // namespace system
} // namespace libbitcoin
