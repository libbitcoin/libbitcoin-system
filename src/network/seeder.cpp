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
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

seeder::seeder(protocol* proto, const config::endpoint::list& seeds,
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

void seeder::contact(const config::endpoint& seed)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed.to_string() << "]";

    const auto self = shared_from_this();
    const auto connect_handler = [self, &seed](const std::error_code& ec,
        channel_ptr node)
    {
        self->connect(ec, seed, node);
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

        visit(ec);
        return;
    }

    if (ec)
    {
        log_info(LOG_PROTOCOL)
            << "Failure contacting seed [" << seed << "]: "
            << ec.message();

        visit(ec);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Getting addresses from [" << seed << "] as ["
        << node->address() << "]";

    node->send(get_address_type(),
        strand_.wrap(&seeder::handle_send,
            shared_from_this(), _1));

    node->subscribe_address(
        strand_.wrap(&seeder::handle_store_all,
            shared_from_this(), _1, _2, node));

}

void seeder::handle_send(const std::error_code& ec)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address message: " << ec.message();

        visit(ec);
    }
}

void seeder::handle_store_all(const std::error_code& ec, const address_type& message,
    channel_ptr node)
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

        visit(ec);
        return;
    }

    if (node)
        log_info(LOG_PROTOCOL)
            << "Storing " << message.addresses.size()
            << " addresses from seed [" << node->address() << "]";

    for (const auto& address: message.addresses)
        host_pool_.store(address,
        strand_.wrap(&seeder::handle_store_one,
                shared_from_this(), _1));

    if (!message.addresses.empty())
        succeeded_ = true;

    visit(ec);
}

// This is called for each individual accress in the packet.
void seeder::handle_store_one(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing address from seed: " << ec.message();
}

void seeder::visit(const std::error_code& ec)
{
    BITCOIN_ASSERT(visited_ < seeds_.size());

    // We block session start until all seeds are populated. This provides
    // greater assurance of a distributed pool of address at session startup.
    if (++visited_ == seeds_.size())
        handle_complete_(succeeded_ ? std::error_code () : ec);
}

} // namespace network
} // namespace libbitcoin
