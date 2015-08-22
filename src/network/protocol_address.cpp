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

#include <memory>
#include <functional>
#include <system_error>
#include <boost/date_time.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using std::placeholders::_1;
using std::placeholders::_2;
using boost::posix_time::time_duration;

protocol_address::protocol_address(channel_ptr peer, threadpool& pool,
    hosts& hosts, const network_address_type& self)
  : peer_(peer),
    pool_(pool),
    dispatch_(pool),
    hosts_(hosts),
    self_({ { self } })
{
}

network_address_type protocol_address::address() const
{
    return peer_->address().to_network_address();
}

bool protocol_address::publish_self_address() const
{
    BITCOIN_ASSERT(!self_.addresses.empty());
    return self_.addresses.front().port != 0;
}

// This should be started immediately following handshake and ping start.
void protocol_address::start()
{
    // Send our address, if the port is configured.
    if (publish_self_address())
        peer_->send(self_,
            std::bind(&protocol_address::handle_send_address,
                shared_from_this(), _1, nullptr));

    // Ignore and don't request address messages if we can't store them.
    if (hosts_.capacity() == 0)
        return;

    // Subscribe to stop messages.
    peer_->subscribe_stop(
        dispatch_.sync(&protocol_address::handle_stop,
            shared_from_this(), _1, nullptr));

    // Subscribe to address messages.
    peer_->subscribe_address(
        dispatch_.sync(&protocol_address::handle_receive_address,
            shared_from_this(), _1, _2, nullptr));

    // Ask for addresses.
    peer_->send(get_address_type(),
        std::bind(&protocol_address::handle_send_get_address,
            shared_from_this(), _1, nullptr));
}

void protocol_address::start(handler handle_seeded,
    const time_duration& timeout)
{
    // Require [three] callbacks (or any error) before calling handle_handshake.
    auto complete = synchronize(handle_seeded, 3, "address");

    // Subscribe to stop messages.
    peer_->subscribe_stop(
        dispatch_.sync(&protocol_address::handle_stop,
            shared_from_this(), _1, complete));

    // 1 of 3
    // Send our address, if the port is configured.
    if (!publish_self_address())
        complete(error::success);
    else
        peer_->send(self_,
            std::bind(&protocol_address::handle_send_address,
                shared_from_this(), _1, complete));

    // Exit if we can't store addresses.
    if (hosts_.capacity() == 0)
    {
        complete(error::not_found);
        return;
    }

    // 2 of 3
    // Subscribe to address messages.
    peer_->subscribe_address(
        dispatch_.sync(&protocol_address::handle_receive_address,
            shared_from_this(), _1, _2, complete));

    // 3 of 3
    // Ask for addresses.
    peer_->send(get_address_type(),
        std::bind(&protocol_address::handle_send_get_address,
            shared_from_this(), _1, complete));

    // Subscribe to timeout event.
    deadline_ = std::make_shared<deadline>(pool_, timeout);
    deadline_->start(
        std::bind(&protocol_address::handle_timer,
            shared_from_this(), _1, complete));
}

bool protocol_address::stopped() const
{
    return stopped_;
}

void protocol_address::handle_stop(const std::error_code& ec, handler complete)
{
    if (stopped())
        return;

    stopped_ = true;

    if (deadline_)
        deadline_->cancel();

    if (complete != nullptr)
        complete(ec);
}

void protocol_address::handle_timer(const std::error_code& ec,
    handler complete) const
{
    if (stopped())
        return;

    if (!deadline::canceled(ec))
        complete(error::channel_timeout);
}

void protocol_address::handle_receive_address(const std::error_code& ec,
    const address_type& message, handler complete)
{
    if (ec)
    {
        log_debug(LOG_PROTOCOL)
            << "Failure getting addresses from ["
            << peer_->address() << "] " << ec.message();
        peer_->stop(error::bad_stream);
        return;
    }

    // Resubscribe to address messages.
    peer_->subscribe_address(
        dispatch_.sync(&protocol_address::handle_receive_address,
            shared_from_this(), _1, _2, complete));

    log_debug(LOG_PROTOCOL)
        << "Storing addresses from [" << peer_->address() << "] ("
        << message.addresses.size() << ")";

    // TODO: manage host timestamps.
    hosts_.store(message.addresses,
        std::bind(&protocol_address::handle_store_addresses,
            shared_from_this(), _1, complete));
}

void protocol_address::handle_send_address(const std::error_code& ec,
    handler complete) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending address message ["
            << peer_->address() << "] " << ec.message();

    if (complete != nullptr)
        complete(ec);
}

void protocol_address::handle_send_get_address(const std::error_code& ec,
    handler complete) const
{
    if (ec)
        log_debug(LOG_PROTOCOL)
            << "Failure sending get address message ["
            << peer_->address() << "] " << ec.message();

    if (complete != nullptr)
        complete(ec);
}

void protocol_address::handle_store_addresses(const std::error_code& ec,
    handler complete) const
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failure storing addresses from ["
            << peer_->address() << "] " << ec.message();

    if (complete != nullptr)
        complete(ec);
}

} // namespace network
} // namespace libbitcoin
