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
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/message_subscriber.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

// This must be declared in the global namespace.
INITIALIZE_TRACK(bc::network::proxy::stop_subscriber)

namespace libbitcoin {
namespace network {

#define NAME "proxy"

using namespace message;
using std::placeholders::_1;
using std::placeholders::_2;
using boost::format;
using boost::posix_time::time_duration;

// TODO: this is made up, configure payload size guard for DoS protection.
static constexpr size_t max_payload_size = 10 * 1024 * 1024;

// Cache the address for logging after stop.
config::authority proxy::authority_factory(asio::socket_ptr socket)
{
    boost_code ec;
    const auto endpoint = socket->remote_endpoint(ec);
    return ec ? config::authority() : config::authority(endpoint);
}

proxy::proxy(threadpool& pool, asio::socket_ptr socket, uint32_t magic)
  : stopped_(true),
    starting_(true),
    magic_(magic),
    authority_(authority_factory(socket)),
    dispatch_(pool, NAME),
    socket_(socket),
    message_subscriber_(pool),
    stop_subscriber_(std::make_shared<stop_subscriber>(pool, NAME "_sub"))
{
}

proxy::~proxy()
{
    do_stop(error::channel_stopped);
}

// Properties.
// ----------------------------------------------------------------------------

// public:
const config::authority& proxy::authority() const
{
    return authority_;
}

// Start sequence.
// ----------------------------------------------------------------------------

// public:
void proxy::start(result_handler handler)
{
    // Start must execute sequentially.

    if (!stopped())
    {
        handler(error::operation_failed);
        return;
    }

    // Must start before invoking start handler, to avoid blocking.
    stopped_ = false;

    // Allow for subscription before first read, so no messages are missed.
    handler(error::success);

    // Must guard subscriptions after start but cannot during.
    starting_ = false;

    // Start the read cycle.
    read_heading();
}

// Stop sequence.
// ----------------------------------------------------------------------------

// public:
void proxy::stop(const code& ec)
{
    BITCOIN_ASSERT_MSG(ec, "The stop code must be an error code.");

    dispatch_.unordered(&proxy::do_stop,
        shared_from_this(), ec);
}

void proxy::do_stop(const code& ec)
{
    if (stopped())
        return;

    stopped_ = true;
    starting_ = true;
    handle_stopping();

    // Close the socket, ignore the error code.
    boost_code ignore;
    socket_->shutdown(asio::socket::shutdown_both, ignore);
    socket_->close(ignore);

    // All message subscribers relay the channel stop code.
    // This results in all message subscriptions fired with the same code.
    message_subscriber_.broadcast(error::channel_stopped);

    // All stop subscriptions are fired with the channel stop reason code.
    stop_subscriber_->relay(ec);
}

void proxy::stop(const boost_code& ec)
{
    stop(error::boost_to_error_code(ec));
}

bool proxy::stopped() const
{
    return stopped_;
}

// Stop subscription sequence.
// ----------------------------------------------------------------------------
// Stop and message subscription must be ordered, as otherwise the stopped
// state of the proxy is subject to a race. The race results in subscriptions
// being registered after stop, never clearing, resulting in socket leaks. The
// race is prevented by subscribing from an ordered call. There are two ways to
// accomplish this: (1) strand all methods that call subscribe, or (2) strand
// all calls to stop. The latter is preferrable as it minimizes the size
// of the commuting region and also allows for cleaner logical decomposition.
// We bypass this ordering during startup by making startup sequential.
// This simplifies subscriptions during and after startup as they do not
// require completion handlers, and prevents startup deadlocked startups.

// public:
void proxy::subscribe_stop(result_handler handler)
{
    if (starting_)
        do_subscribe_stop(handler);
    else
        dispatch_.unordered(&proxy::do_subscribe_stop,
            shared_from_this(), handler);
}

void proxy::do_subscribe_stop(result_handler handler)
{
    if (stopped())
        handler(error::channel_stopped);
    else
        stop_subscriber_->subscribe(handler);
}

// Read cycle (read continues until stop).
// ----------------------------------------------------------------------------

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
        log::debug(LOG_NETWORK)
            << "Channel failure [" << authority() << "] "
            << code(error::boost_to_error_code(ec)).message();
        stop(ec);
        return;
    }

    heading head;
    heading_stream istream(heading_buffer_);
    const auto parsed = head.from_data(istream);
    if (!parsed || head.magic != magic_)
    {
        log::warning(LOG_NETWORK) 
            << "Invalid heading received [" << authority() << "]";
        stop(error::bad_stream);
        return;
    }

    if (head.payload_size > max_payload_size)
    {
        log::warning(LOG_NETWORK)
            << "Oversized payload indicated [" << authority() << "] ("
            << head.payload_size << " bytes)";
        stop(error::bad_stream);
        return;
    }

    log::debug(LOG_NETWORK)
        << "Receive " << head.command << " [" << authority() << "] ("
        << head.payload_size << " bytes)";

    read_payload(head);
    handle_activity();
}

void proxy::handle_read_payload(const boost_code& ec, size_t,
    const heading& heading)
{
    if (stopped())
        return;

    // Ignore read error here, client may have disconnected.

    if (heading.checksum != bitcoin_checksum(payload_buffer_))
    {
        log::warning(LOG_NETWORK) 
            << "Invalid bitcoin checksum from [" << authority() << "]";
        stop(error::bad_stream);
        return;
    }

    // We must copy the payload before restarting the reader.
    const auto payload_copy = payload_buffer_;

    // We must restart the reader before firing subscription events.
    if (!ec)
        read_heading();

    handle_activity();

    // Parse and publish the payload to message subscribers.
    payload_source source(payload_copy);
    payload_stream istream(source);
    const auto parse_error = message_subscriber_.load(heading.type(), istream);
    const auto unconsumed = istream.peek() != std::istream::traits_type::eof();

    if (!parse_error && unconsumed)
    {
        log::warning(LOG_NETWORK)
            << "Valid message [" << heading.command
            << "] handled, unused bytes remain in payload.";
    }

    if (ec)
    {
        log::warning(LOG_NETWORK)
            << "Invalid payload of " << heading.command
            << " from [" << authority() << "] (deferred)"
            << code(error::boost_to_error_code(ec)).message();
        stop(ec);
        return;
    }

    if (parse_error)
    {
        log::warning(LOG_NETWORK)
            << "Invalid stream load of " << heading.command
            << " from [" << authority() << "] " << parse_error.message();
        stop(parse_error);
    }
}

// Message send sequence (public template calls do_send with order).
// ----------------------------------------------------------------------------

// Send handlers are invoked in parallel (unstranded).
void proxy::do_send(const data_chunk& message, result_handler handler,
    const std::string& command)
{
    if (stopped())
    {
        handler(error::channel_stopped);
        return;
    }

    log::debug(LOG_NETWORK)
        << "Send " << command << " [" << authority() << "] ("
        << message.size() << " bytes)";

    const shared_const_buffer buffer(message);
    async_write(*socket_, buffer,
        std::bind(&proxy::handle_send,
            shared_from_this(), _1, handler));
}

// This just allows us to translate th boost error code.
void proxy::handle_send(const boost_code& ec, result_handler handler)
{
    handler(error::boost_to_error_code(ec));
}

} // namespace network
} // namespace libbitcoin
