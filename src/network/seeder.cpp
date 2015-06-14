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
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

seeder::seeder(protocol* proto, const hosts::authority_list& seeds,
    protocol::completion_handler handle_complete)
  : strand_(proto->strand_), 
    host_pool_(proto->host_pool_),
    handshake_(proto->handshake_),
    network_(proto->network_),
    succeeded_(false),
    visited_(0),
    seeds_(seeds),
    handle_complete_(handle_complete)
{
    BITCOIN_ASSERT(proto != nullptr);
}

void seeder::start()
{
    BITCOIN_ASSERT(!succeeded_ && visited_ == 0);
    for (const auto& address: seeds_)
        contact(address);
}

void seeder::contact(const authority& seed_address)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed_address.to_string() << "]";

    connect(handshake_, network_, seed_address.host, seed_address.port,
        strand_.wrap(&seeder::request,
            shared_from_this(), _1, _2));
}

void seeder::request(const std::error_code& ec, channel::pointer seed_node)
{
    if (!seed_node)
    {
        visited(ec);
        return;
    }

    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure contacting seed [" 
            << seed_node->address().to_string() << "] " << ec.message();

        visited(ec);
        return;
    }

    log_debug(LOG_PROTOCOL)
        << "Getting addresses from seed ["
        << seed_node->address().to_string() << "]";

    seed_node->send(message::get_address(),
        strand_.wrap(&seeder::handle_request,
            shared_from_this(), _1));

    seed_node->subscribe_address(
        strand_.wrap(&seeder::store,
            shared_from_this(), _1, _2, seed_node));

}

void seeder::handle_request(const std::error_code& ec)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address message: " << ec.message();

        visited(ec);
    }
}

void seeder::store(const std::error_code& ec, const message::address& packet,
    channel::pointer seed_node)
{
    if (ec)
    {
        if (seed_node)
            log_debug(LOG_PROTOCOL)
                << "Failure getting addresses from seed [" 
                << seed_node->address().to_string() << "] " << ec.message();
        else
            log_debug(LOG_PROTOCOL)
                << "Failure getting addresses from seed: " << ec.message();

        visited(ec);
        return;
    }

    if (seed_node)
        log_info(LOG_PROTOCOL)
            << "Storing " << packet.addresses.size() << " addresses from seed ["
            << seed_node->address().to_string() << "] ";

    for (const auto& address: packet.addresses)
        host_pool_.store(address,
            strand_.wrap(&seeder::handle_store,
                shared_from_this(), _1));

    if (!packet.addresses.empty())
        succeeded_ = true;

    visited(ec);
}

// This is called for each individual accress in the packet.
void seeder::handle_store(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing address from seed: " << ec.message();
}

void seeder::visited(const std::error_code& ec)
{
    BITCOIN_ASSERT(visited_ < seeds_.size());

    // We block session start until all seeds are populated. This provides
    // greater assurance of a random pool of address at session startup.
    if (++visited_ == seeds_.size())
        handle_complete_(succeeded_ ? std::error_code () : ec);
}

} // namespace network
} // namespace libbitcoin
