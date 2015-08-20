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
#include <system_error>
#include <boost/date_time.hpp>
#include <boost/system/error_code.hpp>
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

// ping-pong operates on an independent strand per channel.
protocol_ping::protocol_ping(channel_ptr node, threadpool& pool,
    const timeout& timeouts)
  : node_(node),
    timer_(pool.service()),
    timeouts_(timeouts),
    dispatch_(pool),
    stopped_(false)
{
}

// This should be started immediately following handshake.
void protocol_ping::start()
{
    // Subscribe to stop messages.
    node_->subscribe_stop(
        dispatch_.sync(&protocol_ping::stop,
            shared_from_this(), _1));

    // Subscribe to ping messages.
    node_->subscribe_ping(
        dispatch_.sync(&protocol_ping::handle_receive_ping,
            shared_from_this(), _1, _2));

    // Send initial ping message by simulating first heartbeat.
    dispatch_.queue(
        std::bind(&protocol_ping::handle_timer,
            shared_from_this(), boost::system::error_code()));
}

bool protocol_ping::stopped() const
{
    return stopped_;
}

// This should only be invoked by the stop subscription.
void protocol_ping::stop(const std::error_code& ec)
{
    if (stopped())
        return;

    stopped_ = true;
    clear_timer();
}

void protocol_ping::clear_timer()
{
    // Ignore the error_code.
    boost::system::error_code ec;
    timer_.cancel(ec);
}

void protocol_ping::reset_timer()
{
    set_timer(timeouts_.heartbeat);
}

void protocol_ping::set_timer(const time_duration& timeout)
{
    // Ignore the error_code.
    boost::system::error_code ec;
    timer_.cancel(ec);
    timer_.expires_from_now(timeout, ec);
    timer_.async_wait(
        std::bind(&protocol_ping::handle_timer,
            shared_from_this(), _1));
}

void protocol_ping::handle_timer(const boost::system::error_code& ec)
{
    if (timeout::canceled(ec))
        return;

    const auto nonce = pseudo_random();

    // Subscribe to pong messages.
    node_->subscribe_pong(
        dispatch_.sync(&protocol_ping::handle_receive_pong,
            shared_from_this(), _1, _2, nonce));

    const ping_type random_ping = { nonce };
    node_->send(random_ping,
        std::bind(&protocol_ping::handle_send_ping,
            shared_from_this(), _1));

    reset_timer();
}

void protocol_ping::handle_send_ping(const std::error_code& ec) const
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Ping send failure [" << node_->address() << "] "
            << ec.message();
        return;
    }

    log_debug(LOG_NETWORK)
        << "Ping sent [" << node_->address() << "]";
}

void protocol_ping::handle_send_pong(const std::error_code& ec) const
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Pong send failure [" << node_->address() << "] "
            << ec.message();
        return;
    }

    log_debug(LOG_NETWORK)
        << "Pong sent [" << node_->address() << "]";
}

void protocol_ping::handle_receive_ping(const std::error_code& ec,
    const ping_type& ping)
{
    if (stopped())
        return;

    // Resubscribe to ping messages.
    node_->subscribe_ping(
        dispatch_.sync(&protocol_ping::handle_receive_ping,
            shared_from_this(), _1, _2));

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting ping from [" << node_->address() << "]";
        return;
    }

    const pong_type reply_pong = { ping.nonce };
    node_->send(reply_pong,
        std::bind(&protocol_ping::handle_send_pong,
            shared_from_this(), _1));
}

void protocol_ping::handle_receive_pong(const std::error_code& ec,
    const pong_type& ping, uint64_t nonce)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting pong from [" << node_->address() << "]";
        return;
    }

    if (ping.nonce != nonce)
    {
        log_warning(LOG_NETWORK)
            << "Invalid pong nonce from [" << node_->address() << "]";

        // This could result from message overlap due to a short period.
        // But we assume the response is not as expected and terminate.
        node_->stop(error::bad_stream);
    }
}

} // namespace network
} // namespace libbitcoin
