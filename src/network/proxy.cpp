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
#include <bitcoin/bitcoin/network/proxy.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <memory>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/loader.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

// These must be declared in the global namespace.
////INITIALIZE_PROXY_MESSAGE_SUBSCRIBER_TRACKS()
INITIALIZE_TRACK(bc::network::proxy::stop_subscriber)
INITIALIZE_TRACK(bc::network::proxy)

namespace libbitcoin {
namespace network {

using namespace message;
using std::placeholders::_1;
using std::placeholders::_2;
using boost::format;
using boost::posix_time::time_duration;

// The proxy will have no config with timers moved to channel.
proxy::proxy(asio::socket_ptr socket, threadpool& pool,
    const timeout& timeouts)
  : socket_(socket),
    dispatch_(pool),
    timeouts_(timeouts),
    expiration_(std::make_shared<deadline>(pool, timeouts.expiration)),
    inactivity_(std::make_shared<deadline>(pool, timeouts.inactivity)),
    revival_(std::make_shared<deadline>(pool, timeouts.revival)),
    revival_handler_(nullptr),
    stopped_(false),
    nonce_(0),
    ////INITIALIZE_PROXY_MESSAGE_SUBSCRIBERS(),
    stop_subscriber_(MAKE_SUBSCRIBER(stop, pool, LOG_NETWORK)),
    CONSTRUCT_TRACK(proxy, LOG_NETWORK)
{
    ///ESTABLISH_PROXY_MESSAGE_RELAYS();
}

proxy::~proxy()
{
    BITCOIN_ASSERT_MSG(stopped_, "The channel is not stopped.");
}

void proxy::start()
{
    read_heading();
    start_timers();
}

// TODO: cache the endpoint value at accept|connect time.
config::authority proxy::address() const
{
    boost_code ec;
    const auto endpoint = socket_->remote_endpoint(ec);

    // The endpoint may have become disconnected.
    return ec ? config::authority() : config::authority(endpoint);
}

bool proxy::stopped() const
{
    return stopped_;
}

void proxy::stop(const boost_code& ec)
{
    stop(error::boost_to_error_code(ec));
}

void proxy::stop(const code& ec)
{
    BITCOIN_ASSERT_MSG(ec, "The stop code must be an error code.");

    if (stopped())
        return;

    dispatch_.ordered(&proxy::do_stop,
        shared_from_this(), ec);
}

void proxy::do_stop(const code& ec)
{
    if (stopped())
        return;

    stopped_ = true;
    clear_timers();

    // Shutter the socket, ignore the error code.
    boost_code code;
    socket_->shutdown(asio::socket::shutdown_both, code);
    socket_->close(code);

    // Clear all message subscriptions and notify with stop reason code.
    clear_subscriptions(ec);
}

void proxy::clear_subscriptions(const code& ec)
{
    ////CLEAR_PROXY_MESSAGE_SUBSCRIPTIONS();
    stop_subscriber_->relay(ec);
}

void proxy::clear_timers()
{
    expiration_->cancel();
    inactivity_->cancel();
    revival_->cancel();
    revival_handler_ = nullptr;
}

void proxy::start_timers()
{
    if (stopped())
        return;

    start_expiration();
    start_revival();
    start_inactivity();
}

// public
void proxy::reset_revival()
{
    if (stopped())
        return;

    start_revival();
}

// public
void proxy::set_revival_handler(handler handler)
{
    if (stopped())
        return;

    revival_handler_ = handler;
}

void proxy::start_expiration()
{
    if (stopped())
        return;

    const auto timeout = pseudo_randomize(timeouts_.expiration);

    expiration_->start(
        std::bind(&proxy::handle_expiration,
            shared_from_this(), _1), timeout);
}

void proxy::start_inactivity()
{
    if (stopped())
        return;

    inactivity_->start(
        std::bind(&proxy::handle_inactivity,
            shared_from_this(), _1));
}

void proxy::start_revival()
{
    if (stopped())
        return;

    revival_->start(
        std::bind(&proxy::handle_revival,
            shared_from_this(), _1));
}

void proxy::handle_expiration(const code& ec)
{
    if (stopped())
        return;

    if (deadline::canceled(ec))
        return;

    log_info(LOG_NETWORK)
        << "Channel lifetime expired [" << address() << "]";

    stop(error::channel_timeout);
}

void proxy::handle_inactivity(const code& ec)
{
    if (stopped())
        return;

    if (deadline::canceled(ec))
        return;

    log_info(LOG_NETWORK)
        << "Channel inactivity timeout [" << address() << "]";

    stop(error::channel_timeout);
}

void proxy::handle_revival(const code& ec)
{
    if (stopped())
        return;

    if (deadline::canceled(ec))
        return;

    // Nothing to do, no handler registered.
    if (revival_handler_ == nullptr)
        return;

    revival_handler_(ec);
}

void proxy::read_heading()
{
    if (stopped())
        return;

    using namespace boost::asio;
    async_read(*socket_, buffer(heading_buffer_),
        dispatch_.ordered_delegate(&proxy::handle_read_heading,
            shared_from_this(), _1, _2));
}

void proxy::read_payload(const heading& head)
{
    if (stopped())
        return;

    payload_buffer_.resize(head.payload_size);

    using namespace boost::asio;
    async_read(*socket_, buffer(payload_buffer_, head.payload_size),
        dispatch_.ordered_delegate(&proxy::handle_read_payload,
            shared_from_this(), _1, _2, head));
}

void proxy::handle_read_heading(const boost_code& ec, size_t)
{
    if (stopped())
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Channel failure [" << address() << "] "
            << code(error::boost_to_error_code(ec)).message();
        stop(ec);
        return;
    }

    heading head;
    heading_stream istream(heading_buffer_);
    const auto parsed = head.from_data(istream);
    if (!parsed || head.magic != bc::magic_value)
    {
        log_warning(LOG_NETWORK) 
            << "Invalid heading received [" << address() << "]";
        stop(error::bad_stream);
        return;
    }

    log_debug(LOG_NETWORK)
        << "Receive " << head.command << " [" << address() << "] ("
        << head.payload_size << " bytes)";

    read_payload(head);
    start_inactivity();
}

void proxy::handle_read_payload(const boost_code& ec, size_t,
    const heading& heading)
{
    if (stopped())
        return;

    // Ignore read error here, client may have disconnected.

    if (heading.checksum != bitcoin_checksum(payload_buffer_))
    {
        log_warning(LOG_NETWORK) 
            << "Invalid bitcoin checksum from [" << address() << "]";
        stop(error::bad_stream);
        return;
    }

    // We must copy the payload before restarting the reader.
    const auto payload_copy = payload_buffer_;

    // This must be called before firing subscription events.
    if (!ec)
        read_heading();

    start_inactivity();

    // Parse and publish the payload to message subscribers.
    payload_source source(payload_copy);
    payload_stream istream(source);
    const auto error = stream_loader_.load(heading.command, istream);

    // Warn about unconsumed bytes in the stream.
    if (!error && istream.peek() != std::istream::traits_type::eof())
        log_warning(LOG_NETWORK)
            << "Valid message [" << heading.command
            << "] handled, unused bytes remain in payload.";

    // Stop the channel if there was a read error.
    if (ec)
    {
        log_warning(LOG_NETWORK)
            << "Invalid payload of " << heading.command
            << " from [" << address() << "] (deferred)"
            << code(error::boost_to_error_code(ec)).message();
        stop(ec);
        return;
    }

    // Stop the channel if there was an error from parse.
    if (error)
    {
        log_warning(LOG_NETWORK)
            << "Invalid stream load of " << heading.command
            << " from [" << address() << "] " << error.message();
        stop(error);
    }
}

void proxy::subscribe_stop(stop_handler handler)
{
    if (stopped())
        handler(error::channel_stopped);
    else
        stop_subscriber_->subscribe(handler);
}

void proxy::do_send(const data_chunk& message, handler handler,
    const std::string& command)
{
    if (stopped())
    {
        handler(error::channel_stopped);
        return;
    }

    log_debug(LOG_NETWORK)
        << "Send " << command << " [" << address() << "] ("
        << message.size() << " bytes)";

    const shared_const_buffer buffer(message);
    async_write(*socket_, buffer,
        std::bind(&proxy::call_handle_send,
            shared_from_this(), _1, handler));
}

void proxy::call_handle_send(const boost_code& ec, handler handler)
{
    handler(error::boost_to_error_code(ec));
}

} // namespace network
} // namespace libbitcoin
