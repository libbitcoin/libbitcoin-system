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
#include <bitcoin/bitcoin/utility/asio.hpp>

namespace libbitcoin {
namespace network {

/// Common network configuration settings, thread safe.
struct BC_API settings
{
    /// Default instances.
    static const settings mainnet;
    static const settings testnet;

    /// Properties.
    uint32_t threads;
    uint32_t identifier;
    uint16_t inbound_port;
    uint32_t connection_limit;
    uint32_t outbound_connections;
    uint32_t manual_retry_limit;
    uint32_t connect_batch_size;
    uint32_t connect_timeout_seconds;
    uint32_t channel_handshake_seconds;
    uint32_t channel_poll_seconds;
    uint32_t channel_heartbeat_minutes;
    uint32_t channel_inactivity_minutes;
    uint32_t channel_expiration_minutes;
    uint32_t channel_germination_seconds;
    uint32_t host_pool_capacity;
    bool relay_transactions;
    boost::filesystem::path hosts_file;
    boost::filesystem::path debug_file;
    boost::filesystem::path error_file;
    config::authority self;
    config::authority::list blacklists;
    config::endpoint::list seeds;

    /// Helpers.
    asio::duration connect_timeout() const;
    asio::duration channel_handshake() const;
    asio::duration channel_poll() const;
    asio::duration channel_heartbeat() const;
    asio::duration channel_inactivity() const;
    asio::duration channel_expiration() const;
    asio::duration channel_germination() const;
};

} // namespace network
} // namespace libbitcoin

#endif
