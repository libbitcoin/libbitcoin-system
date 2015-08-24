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
#include <bitcoin/bitcoin/network/protocol_ping.hpp>

#include <functional>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/message/ping_pong.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using std::placeholders::_1;
using std::placeholders::_2;
using boost::posix_time::time_duration;

protocol_ping::protocol_ping(channel::ptr peer, threadpool& pool,
    const time_duration& period)
  : peer_(peer),
    deadline_(std::make_shared<deadline>(pool, period)),
    dispatch_(pool),
    stopped_(false)
{
}

// This should be started immediately following handshake.
void protocol_ping::start()
{
    // Subscribe to stop messages.
    peer_->subscribe_stop(
        dispatch_.sync(&protocol_ping::handle_stop,
            shared_from_this(), _1));

    // Subscribe to ping messages.
    peer_->subscribe_ping(
        dispatch_.sync(&protocol_ping::handle_receive_ping,
            shared_from_this(), _1, _2));

    // Send initial ping message by simulating first heartbeat.
    dispatch_.queue(
        std::bind(&protocol_ping::handle_timer,
            shared_from_this(), error::success));
}

void protocol_ping::handle_stop(const code& ec)
{
    if (stopped())
        return;

    stopped_ = true;

    if (deadline_)
        deadline_->cancel();
}

bool protocol_ping::stopped() const
{
    return stopped_;
}

void protocol_ping::handle_timer(const code& ec)
{
    if (deadline::canceled(ec))
        return;

    const auto nonce = pseudo_random();

    // Subscribe to pong messages.
    peer_->subscribe_pong(
        dispatch_.sync(&protocol_ping::handle_receive_pong,
            shared_from_this(), _1, _2, nonce));

    peer_->send(message::ping(nonce),
        std::bind(&protocol_ping::handle_send_ping,
            shared_from_this(), _1));

    deadline_->start(
        std::bind(&protocol_ping::handle_timer,
            shared_from_this(), _1));
}

void protocol_ping::handle_send_ping(const code& ec) const
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Ping send failure [" << peer_->address() << "] "
            << ec.message();
        return;
    }

    log_debug(LOG_NETWORK)
        << "Ping sent [" << peer_->address() << "]";
}

void protocol_ping::handle_send_pong(const code& ec) const
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Pong send failure [" << peer_->address() << "] "
            << ec.message();
        return;
    }

    log_debug(LOG_NETWORK)
        << "Pong sent [" << peer_->address() << "]";
}

void protocol_ping::handle_receive_ping(const code& ec,
    const message::ping& ping)
{
    if (stopped())
        return;

    // Resubscribe to ping messages.
    peer_->subscribe_ping(
        dispatch_.sync(&protocol_ping::handle_receive_ping,
            shared_from_this(), _1, _2));

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting ping from [" << peer_->address() << "]";
        return;
    }

    peer_->send(message::pong(ping.nonce),
        std::bind(&protocol_ping::handle_send_pong,
            shared_from_this(), _1));
}

void protocol_ping::handle_receive_pong(const code& ec,
    const message::pong& ping, uint64_t nonce)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting pong from [" << peer_->address() << "]";
        return;
    }

    if (ping.nonce != nonce)
    {
        log_warning(LOG_NETWORK)
            << "Invalid pong nonce from [" << peer_->address() << "]";

        // This could result from message overlap due to a short period.
        // But we assume the response is not as expected and terminate.
        peer_->stop(error::bad_stream);
    }
}

} // namespace network
} // namespace libbitcoin
