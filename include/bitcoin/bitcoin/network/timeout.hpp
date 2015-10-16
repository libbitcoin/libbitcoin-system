/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_NETWORK_TIMEOUT_HPP
#define LIBBITCOIN_NETWORK_TIMEOUT_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>

namespace libbitcoin {
namespace network {

class BC_API timeout
{
public:
    static const timeout defaults;

    timeout(
        uint32_t connect_timeout_seconds=5,
        uint32_t channel_handshake_seconds=30,
        uint32_t channel_revival_minutes=5,
        uint32_t channel_heartbeat_minutes=5,
        uint32_t channel_inactivity_minutes=30,
        uint32_t channel_expiration_minutes=90,
        uint32_t channel_germination_seconds=30);

    asio::duration connect;
    asio::duration handshake;
    asio::duration revival;
    asio::duration heartbeat;
    asio::duration inactivity;
    asio::duration expiration;
    asio::duration germination;
};

} // namespace network
} // namespace libbitcoin

#endif

