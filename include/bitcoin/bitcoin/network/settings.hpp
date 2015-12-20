/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SETTINGS_HPP
#define LIBBITCOIN_SETTINGS_HPP

#include <cstdint>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace network {

// This is in network vs. config because plan to move to libbitcoin-network.
struct BC_API settings
{
    uint32_t threads;
    uint16_t inbound_port;
    uint32_t inbound_connection_limit;
    uint32_t outbound_connections;
    uint32_t connect_timeout_seconds;
    uint32_t channel_handshake_seconds;
    uint32_t channel_poll_seconds;
    uint32_t channel_heartbeat_minutes;
    uint32_t channel_inactivity_minutes;
    uint32_t channel_expiration_minutes;
    uint32_t host_pool_capacity;
    bool relay_transactions;
    boost::filesystem::path hosts_file;
    boost::filesystem::path debug_file;
    boost::filesystem::path error_file;
    config::authority self;
    config::endpoint::list seeds;
};

} // namespace network
} // namespace libbitcoin

#endif
