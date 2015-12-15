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
#include <bitcoin/bitcoin/network/seeder.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <system_error>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

// Based on bitcoinstats.com/network/dns-servers
#ifdef ENABLE_TESTNET
const config::endpoint::list seeder::defaults
{
    { "testnet-seed.alexykot.me", 18333 },
    { "testnet-seed.bitcoin.petertodd.org", 18333 },
    { "testnet-seed.bluematt.me", 18333 },
    { "testnet-seed.bitcoin.schildbach.de", 18333 }
};
#else
const config::endpoint::list seeder::defaults
{
    { "seed.bitnodes.io", 8333 },
    { "seed.bitcoinstats.com", 8333 },
    { "seed.bitcoin.sipa.be", 8333 },
    { "dnsseed.bluematt.me", 8333 },
    { "seed.bitcoin.jonasschnelli.ch", 8333 },
    { "dnsseed.bitcoin.dashjr.org", 8333 }
};
#endif

seeder::seeder(threadpool& pool, hosts& hosts, handshake& shake, peer& network,
    const config::endpoint::list& seeds)
  : strand_(pool),
    host_pool_(hosts),
    handshake_(shake),
    network_(network),
    seeds_(seeds)
{
}

void seeder::start(seeded_handler handle_seeded)
{
    BITCOIN_ASSERT_MSG(handle_seeded, "null completion handler");

    if (seeds_.empty())
    {
        log_info(LOG_PROTOCOL)
            << "No seeds configured.";
        handle_seeded(error::success);
    }

    const auto seed_count = seeds_.size();
    const auto host_count = host_pool_.size();

    // Require all node callbacks before calling seeder::handle_synced.
    // We can expect to get exactly one completion handler call per node.
    const auto complete = synchronizer<seeded_handler>(
        std::bind(&seeder::handle_synced,
            this, _1, host_count, handle_seeded), seed_count, "seeder");

    // connect is asynchronous, so this will not delay.
    // Require one callback per node before calling node_complete.
    // A second call is otherwise possible due to concurrent disconnection.
    for (const auto& seed: seeds_)
        connect(seed, synchronizer<seeded_handler>(complete, 1, 
            seed.to_string()));
}

void seeder::handle_synced(const std::error_code& ec, size_t host_start_count,
    seeded_handler handle_seeded)
{
    // Error implies a full stop, so instead we send an error only if no seeds.
    const auto success = host_pool_.size() > host_start_count;
    const auto result = success ? error::success : error::operation_failed;
    handle_seeded(result);
}

void seeder::connect(const config::endpoint& seed,
    seeded_handler completion_callback)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed << "]";

    network_.connect(seed.host(), seed.port(),
        std::bind(&seeder::handle_connected,
            this, _1, _2, seed, completion_callback));
}

void seeder::handle_connected(const std::error_code& ec, channel_ptr node,
    const config::endpoint& seed, seeded_handler completion_callback)
{
    if (ec)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "] "
            << ec.message();
        completion_callback(error::success);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Get seed [" << seed << "] as [" << node->address() << "]";

    node->subscribe_stop(
        strand_.wrap(&seeder::handle_stop,
            this, _1, seed, completion_callback));

    // Subscribe to events and start talking on the socket.
    static const bool relay = false;
    handshake_.start(node, 
        strand_.wrap(&seeder::handle_handshake,
            this, _1, node, seed, completion_callback), relay);

    node->start();
}

void seeder::handle_stop(const std::error_code& ec,
    const config::endpoint& seed, seeded_handler completion_callback)
{
    if (ec && ec != error::channel_stopped)
        log_debug(LOG_PROTOCOL)
            << "Seed channel stopped [" << seed << "] " << ec.message();

    completion_callback(error::success);
}

void seeder::handle_handshake(const std::error_code& ec, channel_ptr node,
    const config::endpoint& seed, seeded_handler completion_callback)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL) << "Failure in handshake with seed ["
            << node->address() << "] " << ec.message();
        completion_callback(error::success);
        return;
    }

    // We could start ping-pong here but probabaly not important.

    node->subscribe_address(
        std::bind(&seeder::handle_receive,
            this, _1, _2, seed, node, completion_callback));

    node->send(get_address_type(),
        strand_.wrap(&seeder::handle_send,
            this, _1, seed, completion_callback));
};

void seeder::handle_send(const std::error_code& ec,
    const config::endpoint& seed, seeded_handler completion_callback)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address to seed [" << seed << "] "
            << ec.message();
        completion_callback(error::success);
    }
}

bool seeder::handle_receive(const std::error_code& ec,
    const address_type& message, const config::endpoint& seed,
    channel_ptr node, seeded_handler completion_callback)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure getting addresses from seed [" << seed << "] "
            << ec.message();
        completion_callback(error::success);
        return false;
    }

    log_info(LOG_PROTOCOL)
        << "Storing addresses from seed [" << seed << "] ("
        << message.addresses.size() << ")";

    // TODO: have host pool process address list internally.
    for (const auto& address: message.addresses)
        host_pool_.store(address,
            strand_.wrap(&seeder::handle_store,
                this, _1));

    completion_callback(error::success);

    // We are using this call to keep node in scope until receive.
    node->stop(error::channel_stopped);
    return false;
}

// This is called for each individual address in the packet.
void seeder::handle_store(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing address from seed: " << ec.message();
}

} // namespace network
} // namespace libbitcoin
