/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/network/settings.hpp>

#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace network {

using namespace boost::filesystem;

static const settings mainnet_defaults()
{
    settings value;
    value.threads = 4;
    value.identifier = 3652501241;
    value.inbound_port = 8333;
    value.connection_limit = 16;
    value.outbound_connections = 8;
    value.manual_retry_limit = 0;
    value.connect_batch_size = 5;
    value.connect_timeout_seconds = 5;
    value.channel_handshake_seconds = 30;
    value.channel_poll_seconds = 1;
    value.channel_heartbeat_minutes = 5;
    value.channel_inactivity_minutes = 10;
    value.channel_expiration_minutes = 1440;
    value.channel_germination_seconds = 30;
    value.host_pool_capacity = 1000;
    value.relay_transactions = true;
    value.hosts_file = path("hosts.cache");
    value.debug_file = path("debug.log");
    value.error_file = path("error.log");
    value.self = unspecified_network_address;
    value.blacklists = {};

    // Seeds based on bitcoinstats.com/network/dns-servers
    value.seeds = 
    {
        { "seed.bitchannels.io", 8333 },
        { "seed.bitcoinstats.com", 8333 },
        { "seed.bitcoin.sipa.be", 8333 },
        { "dnsseed.bluematt.me", 8333 },
        { "seed.bitcoin.jonasschnelli.ch", 8333 },
        { "dnsseed.bitcoin.dashjr.org", 8333 }
    };
    return value;
};

static const settings testnet_defaults()
{
    auto value = mainnet_defaults();
    value.identifier = 118034699;
    value.inbound_port = 18333;
    value.seeds =
    {
        { "testnet-seed.bitcoin.petertodd.org", 18333 },
        { "testnet-seed.bitcoin.schildbach.de", 18333 },
        { "testnet-seed.bluematt.me", 18333 }
    };
    return value;
};

const settings settings::mainnet = mainnet_defaults();
const settings settings::testnet = testnet_defaults();


asio::duration settings::connect_timeout() const
{
    return asio::seconds(connect_timeout_seconds);
}

asio::duration settings::channel_handshake() const
{
    return asio::seconds(channel_handshake_seconds);
}

asio::duration settings::channel_poll() const
{
    return asio::seconds(channel_poll_seconds);
}

asio::duration settings::channel_heartbeat() const
{
    return asio::minutes(channel_heartbeat_minutes);
}

asio::duration settings::channel_inactivity() const
{
    return asio::minutes(channel_inactivity_minutes);
}

asio::duration settings::channel_expiration() const
{
    return asio::minutes(channel_expiration_minutes);
}

asio::duration settings::channel_germination() const
{
    return asio::seconds(channel_germination_seconds);
} 

} // namespace network
} // namespace libbitcoin
