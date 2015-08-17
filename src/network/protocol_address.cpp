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
#include <bitcoin/bitcoin/network/protocol_address.hpp>

#include <functional>
#include <system_error>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using std::placeholders::_1;
using std::placeholders::_2;

// Address handling shares a strand per channel (to sync pool access).
protocol_address::protocol_address(channel_ptr node, threadpool& pool,
    hosts& hosts, const network_address_type& self)
  : node_(node),
    sequence_(pool),
    hosts_(hosts),
    self_({ { self } })
{
}

bool protocol_address::is_self_defined() const
{
    BITCOIN_ASSERT(!self_.addresses.empty());
    return self_.addresses.front().port != 0;
}

network_address_type protocol_address::get_address() const
{
    return node_->address().to_network_address();
}

// TODO: need a variant of this that starts with callback and has timeout.
// This should be started immediately following handshake and ping start.
void protocol_address::start()
{
    // Send our address, if the port is configured.
    if (is_self_defined())
        node_->send(self_,
            std::bind(&protocol_address::handle_send_address,
                shared_from_this(), _1));

    // Ignore and don't request address messages if we can't store them.
    if (hosts_.capacity() == 0)
        return;

    // Subscribe to address messages.
    node_->subscribe_address(
        sequence_.sync(&protocol_address::handle_receive_address,
            shared_from_this(), _1, _2));

    // Ask for addresses.
    node_->send(get_address_type(),
        std::bind(&protocol_address::handle_send_get_address,
            shared_from_this(), _1));

    // Store the peer's address.
    hosts_.store(get_address(),
        sequence_.sync(&protocol_address::handle_store_address,
            this, _1));
}

void protocol_address::handle_receive_address(const std::error_code& ec,
    const address_type& message)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure getting addresses from ["
            << node_->address() << "] " << ec.message();
        node_->stop(error::bad_stream);
        return;
    }

    // Resubscribe to address messages.
    node_->subscribe_address(
        sequence_.sync(&protocol_address::handle_receive_address,
            shared_from_this(), _1, _2));

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from [" << node_->address() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage host timestamps.
    hosts_.store(message.addresses,
        std::bind(&protocol_address::handle_store_address,
            shared_from_this(), _1));
}

void protocol_address::handle_send_address(const std::error_code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending address message ["
            << node_->address() << "] " << ec.message();
}

void protocol_address::handle_send_get_address(const std::error_code& ec) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address message ["
            << node_->address() << "] " << ec.message();
}

void protocol_address::handle_store_address(const std::error_code& ec) const
{
    if (ec && ec != error::duplicate)
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from ["
            << node_->address() << "] " << ec.message();
}

} // namespace network
} // namespace libbitcoin
