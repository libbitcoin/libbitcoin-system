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
#include <memory>
#include <system_error>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/initiator.hpp>
#include <bitcoin/bitcoin/network/protocol_address.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
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

// This is not currently stoppable.
seeder::seeder(threadpool& pool, hosts& hosts, handshake& shake,
    initiator& network, const config::endpoint::list& seeds,
    const network_address_type& self)
  : sequence_(pool),
    hosts_(hosts),
    pool_(pool),
    handshake_(shake),
    network_(network),
    seeds_(seeds),
    self_(self)
{
}

seeder::~seeder()
{
    log_info(LOG_PROTOCOL)
        << "Closed seeder";
}

void seeder::start(completion_handler handle_complete)
{
    if (seeds_.empty() || hosts_.capacity() == 0)
    {
        log_info(LOG_PROTOCOL)
            << "No seeds and/or host capacity configured.";
        handle_complete(error::success);
        return;
    }

    const auto multiple = 
        std::bind(&seeder::handle_seeded,
            shared_from_this(), _1, hosts_.size(), handle_complete);

    using syncronize = synchronizer<completion_handler>;

    // Require all node callbacks before calling seeder::handle_synced.
    const auto single = syncronize(multiple, seeds_.size(), "seeder");

    // Require one callback per node before calling node_complete.
    for (const auto& seed: seeds_)
        start_connect(seed, syncronize(single, 1, seed.to_string()));
}

void seeder::handle_seeded(const std::error_code& ec, size_t host_start_size,
    completion_handler handle_complete)
{
    // We succeed only by adding seeds.
    if (hosts_.size() > host_start_size)
        handle_complete(error::success);
    else
        handle_complete(error::operation_failed);
}

void seeder::start_connect(const config::endpoint& seed,
    completion_handler handle_complete)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed << "]";

    // OUTBOUND CONNECT (concurrent)
    network_.connect(seed.host(), seed.port(),
        std::bind(&seeder::handle_connected,
            shared_from_this(), _1, _2, seed, handle_complete));
}

void seeder::handle_connected(const std::error_code& ec, channel_ptr node,
    const config::endpoint& seed, completion_handler handle_complete)
{
    if (ec)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "] "
            << ec.message();
        handle_complete(error::success);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Connected seed [" << seed << "] as " << node->address();

    // Subscribe to events and start talking on the socket.
    static const bool relay = false;
    handshake_.start(node, 
        sequence_.sync(&seeder::handle_handshake,
            shared_from_this(), _1, node, seed, handle_complete), relay);

    // Start reading from the socket (causing subscription events).
    node->start();
}

void seeder::handle_handshake(const std::error_code& ec, channel_ptr node,
    const config::endpoint& seed, completion_handler handle_complete)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL) << "Failure in seed handshake ["
            << node->address() << "] " << ec.message();
        handle_complete(error::success);
        return;
    }

    // Attach address protocol to the new connection.
    std::make_shared<protocol_address>(node, pool_, hosts_, self_)->start();
};

// This is called for each individual address in the packet.
void seeder::handle_store(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing address from seed: " << ec.message();
}

} // namespace network
} // namespace libbitcoin
