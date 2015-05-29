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
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;

seeder::seeder(protocol* proto, const hosts::name_list& seeds,
    protocol::completion_handler handle_complete)
  : strand_(proto->strand_), 
    hosts_(proto->hosts_),
    handshake_(proto->handshake_),
    network_(proto->network_),
    visited_(0),
    seeds_(seeds),
    handle_complete_(handle_complete)
{
    BITCOIN_ASSERT(proto != nullptr);
}

void seeder::start()
{
    BITCOIN_ASSERT(visited_ == 0);
    for (const auto& address: seeds_)
        contact(address);
}

void seeder::contact(const hosts::address& seed)
{
    log_info(LOG_PROTOCOL)
        << "Contacting seed [" << seed.to_string() << "]";

    connect(handshake_, network_, seed.host, seed.port,
        strand_.wrap(&seeder::request,
            shared_from_this(), _1, _2));
}

void seeder::request(const std::error_code& ec, channel_ptr seed_node)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure contacting seed [" 
            << hosts::address(seed_node).to_string() << "] " << ec.message();

        visited(ec);
        return;
    }

    log_debug(LOG_PROTOCOL) 
        << "Getting addresses from seed ["
        << hosts::address(seed_node).to_string() << "]";

    seed_node->send(get_address_type(),
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
        log_error(LOG_PROTOCOL)
            << "Failure sending 'get_address' message: " << ec.message();

        visited(ec);
    }
}

void seeder::store(const std::error_code& ec, const address_type& packet,
    channel_ptr seed_node)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failure getting addresses from seed [" 
            << hosts::address(seed_node).to_string() << "] " << ec.message();

        visited(ec);
        return;
    }

    log_info(LOG_PROTOCOL)
        << "Storing " << packet.addresses.size() << " addresses from seed ["
        << hosts::address(seed_node).to_string() << "] ";

    for (const auto& address: packet.addresses)
        hosts_.store(address,
            strand_.wrap(&seeder::handle_store,
                shared_from_this(), _1));

    visited(ec);
}

void seeder::handle_store(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from seed: " << ec.message();
}

void seeder::visited(const std::error_code& ec)
{
    BITCOIN_ASSERT(visited_ < seeds_.size());
    if (visited_ >= seeds_.size())
        return;

    ++visited_;
    if (visited_ == seeds_.size())
        handle_complete_(ec);
}

} // namespace network
} // namespace libbitcoin
