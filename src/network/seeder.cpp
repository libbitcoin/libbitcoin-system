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
#include <vector>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/initiator.hpp>
#include <bitcoin/bitcoin/network/protocol_seed.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
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
seeder::seeder(threadpool& pool, hosts& hosts, const timeout& timeouts,
    initiator& network, const config::endpoint::list& seeds,
    const message::network_address& self)
  : dispatch_(pool),
    hosts_(hosts),
    timeouts_(timeouts),
    pool_(pool),
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

void seeder::start(handler handle_complete)
{
    if (seeds_.empty() || hosts_.capacity() == 0)
    {
        log_info(LOG_PROTOCOL)
            << "No seeds and/or host capacity configured.";
        handle_complete(error::success);
        return;
    }

    auto multiple =
        std::bind(&seeder::handle_seeded,
            shared_from_this(), _1, hosts_.size(), handle_complete);

    // Require all seed callbacks before calling seeder::handle_complete.
    auto single = synchronize(multiple, seeds_.size(), "seeder");

    // Require one callback per channel before calling single.
    for (const auto& seed: seeds_)
        start_connect(seed, synchronize(single, 1, seed.to_string()));
}

void seeder::handle_seeded(const code& ec, size_t host_start_size,
    handler handle_complete)
{
    // We succeed only by adding seeds.
    if (hosts_.size() > host_start_size)
        handle_complete(error::success);
    else
        handle_complete(error::operation_failed);
}

void seeder::start_connect(const config::endpoint& seed, handler complete)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed << "]";

    // OUTBOUND CONNECT (concurrent)
    network_.connect(seed.host(), seed.port(),
        std::bind(&seeder::handle_connected,
            shared_from_this(), _1, _2, seed, complete));
}

void seeder::handle_connected(const code& ec, channel::ptr peer,
    const config::endpoint& seed, handler complete)
{
    if (ec)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "] "
            << ec.message();
        complete(error::success);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Connected seed [" << seed << "] as " << peer->address();

    static const bool relay = false;
    const auto callback = 
        dispatch_.sync(&seeder::handle_handshake,
            shared_from_this(), _1, peer, seed, complete);

    // Attach version protocol to the new connection (until complete).
    std::make_shared<protocol_version>(peer, pool_, timeouts_.handshake,
        callback, hosts_, self_, relay);

    // Start reading from the socket (causing subscription events).
    peer->start();
}

void seeder::handle_handshake(const code& ec, channel::ptr peer,
    const config::endpoint& seed, handler complete)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL) << "Failure in seed handshake ["
            << peer->address() << "] " << ec.message();
        complete(error::success);
        return;
    }

    // Attach address seed protocol to the new connection.
    std::make_shared<protocol_seed>(peer, pool_, timeouts_.germination,
        complete, hosts_, self_);
};

} // namespace network
} // namespace libbitcoin
