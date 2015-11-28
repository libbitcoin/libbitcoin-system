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
#ifndef LIBBITCOIN_NETWORK_SETTINGS_HPP
#define LIBBITCOIN_NETWORK_SETTINGS_HPP

#include <cstdint>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>

namespace libbitcoin {
namespace network {

/// default settings
#define NETWORK_THREADS                     4
#define NETWORK_CONNECTION_LIMIT            16
#define NETWORK_OUTBOUND_CONNECTIONS        8
#define NETWORK_MANUAL_RETRY_LIMIT          0
#define NETWORK_CONNECT_BATCH_SIZE          5
#define NETWORK_CONNECT_TIMEOUT_SECONDS     5
#define NETWORK_CHANNEL_HANDSHAKE_SECONDS   30
#define NETWORK_CHANNEL_REVIVAL_MINUTES     5
#define NETWORK_CHANNEL_HEARTBEAT_MINUTES   5
#define NETWORK_CHANNEL_INACTIVITY_MINUTES  30
#define NETWORK_CHANNEL_EXPIRATION_MINUTES  90
#define NETWORK_CHANNEL_GERMINATION_SECONDS 30
#define NETWORK_HOST_POOL_CAPACITY          1000
#define NETWORK_RELAY_TRANSACTIONS          true
#define NETWORK_HOSTS_FILE                  boost::filesystem::path("hosts.cache")
#define NETWORK_DEBUG_FILE                  boost::filesystem::path("debug.log")
#define NETWORK_ERROR_FILE                  boost::filesystem::path("error.log")
#define NETWORK_SELF                        bc::unspecified_network_address
#define NETWORK_BLACKLISTS                  config::authority::list()

/// mainnet settings
/// Seeds based on bitcoinstats.com/network/dns-servers
#define NETWORK_IDENTIFIER_MAINNET          3652501241
#define NETWORK_INBOUND_PORT_MAINNET        8333
#define NETWORK_SEEDS_MAINNET \
{ \
    { "seed.bitchannels.io", 8333 }, \
    { "seed.bitcoinstats.com", 8333 }, \
    { "seed.bitcoin.sipa.be", 8333 }, \
    { "dnsseed.bluematt.me", 8333 }, \
    { "seed.bitcoin.jonasschnelli.ch", 8333 }, \
    { "dnsseed.bitcoin.dashjr.org", 8333 } \
}

/// testnet settings
#define NETWORK_IDENTIFIER_TESTNET          118034699
#define NETWORK_INBOUND_PORT_TESTNET        18333
#define NETWORK_SEEDS_TESTNET \
{ \
    { "testnet-seed.bitcoin.petertodd.org", 18333 }, \
    { "testnet-seed.bitcoin.schildbach.de", 18333 }, \
    { "testnet-seed.bluematt.me", 18333 } \
}

// Common network configuration settings, thread safe.
// This file is network_settings.hpp to deconflict with wallet::settings.hpp.
// This is in network vs. config because plan to move to libbitcoin-network.
struct BC_API settings
{
    uint32_t threads;
    uint32_t identifier;
    uint16_t inbound_port;
    uint32_t connection_limit;
    uint32_t outbound_connections;
    uint32_t manual_retry_limit;
    uint32_t connect_batch_size;
    uint32_t connect_timeout_seconds;
    uint32_t channel_handshake_seconds;
    uint32_t channel_revival_minutes;
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

    asio::duration connect_timeout() const
    {
        return asio::duration(0, 0, connect_timeout_seconds);
    }

    asio::duration channel_handshake() const
    {
        return asio::duration(0, 0, channel_handshake_seconds);
    }

    asio::duration channel_revival() const
    {
        return asio::duration(0, channel_revival_minutes, 0);
    }

    asio::duration channel_heartbeat() const
    {
        return asio::duration(0, channel_heartbeat_minutes, 0);
    }

    asio::duration channel_inactivity() const
    {
        return asio::duration(0, channel_inactivity_minutes, 0);
    }

    asio::duration channel_expiration() const
    {
        return asio::duration(0, channel_expiration_minutes, 0);
    }

    asio::duration channel_germination() const
    {
        return asio::duration(0, 0, channel_germination_seconds);
    }
};

} // namespace network
} // namespace libbitcoin

#endif
