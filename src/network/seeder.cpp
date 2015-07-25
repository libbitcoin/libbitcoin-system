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
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>

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

seeder::seeder(threadpool& pool, hosts& hosts, handshake& shake, network& net,
    const config::endpoint::list& seeds)
  : strand_(pool),
    host_pool_(hosts),
    handshake_(shake),
    network_(net),
    seeds_(seeds),
    visited_(0),
    success_(false),
    handle_complete_(nullptr)
{
}

// TODO: set error if no seeds are configured or no hosts can be loaded.
void seeder::start(completion_handler handle_complete)
{
    BITCOIN_ASSERT(!success_ && visited_ == 0 &&
        handle_complete_ == nullptr);

    // Don't call completion handler until ass seeds have been visited.
    handle_complete_ = handle_complete;

    if (seeds_.empty())
    {
        log_info(LOG_PROTOCOL) << "No seeds configured.";
        handle_complete_(error::success);
    }

    for (const auto& address: seeds_)
        contact(address);
}

void seeder::contact(const config::endpoint& seed)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed.to_string() << "]";

    const auto connect_handler = [this, &seed](const std::error_code& ec,
        channel_ptr node)
    {
        connect(ec, seed, node);
    };

    bc::network::connect(handshake_, network_, seed.host(), seed.port(),
        connect_handler);
}

void seeder::connect(const std::error_code& ec, const config::endpoint& seed,
    channel_ptr node)
{
    if (!node)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "]";
        visit();
        return;
    }

    if (ec)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "]: "
            << ec.message();
        visit();
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Getting addresses from [" << seed << "] as ["
        << node->address() << "]";

    node->send(get_address_type(),
        strand_.wrap(&seeder::handle_send,
            this, _1));

    node->subscribe_address(
        strand_.wrap(&seeder::handle_store_all,
            this, _1, _2, node));
}

void seeder::handle_send(const std::error_code& ec)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address message: " << ec.message();
        visit();
    }
}

void seeder::handle_store_all(const std::error_code& ec, 
    const address_type& message, channel_ptr node)
{
    if (ec)
    {
        if (node)
            log_debug(LOG_PROTOCOL)
                << "Failure getting addresses from seed [" 
                << node->address() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Failure getting addresses from seed: " << ec.message();

        visit();
        return;
    }

    if (node)
        log_info(LOG_PROTOCOL)
            << "Storing " << message.addresses.size()
            << " addresses from seed [" << node->address() << "]";

    for (const auto& address: message.addresses)
        host_pool_.store(address,
            strand_.wrap(&seeder::handle_store_one,
                this, _1));

    if (!message.addresses.empty())
        success_ = true;

    visit();
}

// This is called for each individual accress in the packet.
void seeder::handle_store_one(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing address from seed: " << ec.message();
}

void seeder::visit()
{
    BITCOIN_ASSERT(visited_ < seeds_.size());

    // We block session start until all seeds are populated. This provides
    // greater assurance of a distributed pool of address at session startup.
    if (++visited_ == seeds_.size())
        handle_complete_(success_ ? error::success : error::operation_failed);
}

} // namespace network
} // namespace libbitcoin
