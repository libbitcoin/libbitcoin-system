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
#include <bitcoin/bitcoin/network/channel_proxy.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/channel_loader_module.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/satoshi_serialize.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::format;
using boost::posix_time::time_duration;

static inline bool aborted(const std::error_code& ec)
{
    return ec == error::service_stopped;
}

static inline bool aborted(const boost::system::error_code& ec)
{
    return ec == boost::asio::error::operation_aborted;
}

template<typename Message, class Subscriber>
void channel_proxy::subscribe(Subscriber& subscriber)
{
    loader_.add(new channel_loader_module<Message>(
        std::bind(&Subscriber::relay,
            subscriber, _1, _2)));
}

template <typename Message, class Subscriber, typename Callback>
void channel_proxy::subscribe(Subscriber& subscriber, Callback handler) const
{
    // Subscribing must be immediate, we cannot switch thread contexts.
    if (stopped())
        handler(error::service_stopped, Message());
    else
        subscriber.subscribe(handler);
}

channel_proxy::channel_proxy(threadpool& pool, socket_ptr socket,
    const timeout& timeouts=timeout::defaults)
  : strand_(pool),
    socket_(socket),
    timeouts_(timeouts),
    expiration_(pool.service()),
    timeout_(pool.service()),
    heartbeat_(pool.service()),
    revival_(pool.service()),
    revival_handler_(nullptr),
    nonce_(0),
    stopped_(false),
    version_subscriber_(pool),
    verack_subscriber_(pool),
    address_subscriber_(pool),
    get_address_subscriber_(pool),
    inventory_subscriber_(pool),
    get_data_subscriber_(pool),
    get_blocks_subscriber_(pool),
    transaction_subscriber_(pool),
    block_subscriber_(pool),
    ping_subscriber_(pool),
    pong_subscriber_(pool),
    raw_subscriber_(pool),
    stop_subscriber_(pool)
{
    subscribe<version_type>(version_subscriber_);
    subscribe<verack_type>(verack_subscriber_);
    subscribe<address_type>(address_subscriber_);
    subscribe<get_address_type>(get_address_subscriber_);
    subscribe<inventory_type>(inventory_subscriber_);
    subscribe<get_data_type>(get_data_subscriber_);
    subscribe<get_blocks_type>(get_blocks_subscriber_);
    subscribe<transaction_type>(transaction_subscriber_);
    subscribe<block_type>(block_subscriber_);
    subscribe<ping_type>(ping_subscriber_);
    subscribe<pong_type>(pong_subscriber_);
}

channel_proxy::~channel_proxy()
{
    do_stop();
}

void channel_proxy::start()
{
    read_header();
    const auto expiration = pseudo_randomize(timeouts_.expiration);
    set_expiration(expiration);
    set_heartbeat(timeouts_.heartbeat);
    set_revival(timeouts_.revival);
    set_timeout(timeouts_.inactivity);

    // Subscribe to ping messages.
    subscribe_ping(
        strand_.wrap(&channel_proxy::handle_ping_message,
            shared_from_this(), _1, _2));
}

void channel_proxy::handle_ping_message(const std::error_code& ec,
    const ping_type& ping)
{
    if (stopped() || aborted(ec))
        return;

    // Resubscribe to ping messages.
    subscribe_ping(
        strand_.wrap(&channel_proxy::handle_ping_message,
            shared_from_this(), _1, _2));

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting pong from [" << address() << "]";
        return;
    }

    const auto pong = [this](const std::error_code& ec)
    {
        if (ec)
        {
            log_debug(LOG_NETWORK)
                << "Pong send failure [" << address() << "] "
                << ec.message();
            return;
        }

        log_debug(LOG_NETWORK)
            << "Pong sent [" << address() << "] ";
    };

    const pong_type reply_pong = { ping.nonce };
    send(reply_pong, pong);
}

void channel_proxy::stop(const std::error_code& ec)
{
    if (stopped())
        return;

    const auto self = shared_from_this();
    const auto stop_service = [self, ec]()
    {
        self->do_stop(ec);
    };
    strand_.queue(stop_service);
}

void channel_proxy::stop(const boost::system::error_code& ec)
{
    if (stopped())
        return;

    stop(error::boost_to_error_code(ec));
}

void channel_proxy::do_stop(const std::error_code& ec)
{
    if (stopped())
        return;

    stop_impl();
    stop_subscriber_.relay(ec);
}

void channel_proxy::stop_impl()
{
    if (stopped())
        return;

    // We need this because the timeout timer shares this code with stop()
    // But sends a different error_code
    stopped_ = true;

    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ec;
    timeout_.cancel(ec);
    heartbeat_.cancel(ec);
    revival_.cancel(ec);
    revival_handler_ = nullptr;

    // Force the socket closed
    // Should we do something with these error_codes?
    socket_->shutdown(tcp::socket::shutdown_both, ec);
    socket_->close(ec);
    clear_subscriptions();
}

config::authority channel_proxy::address() const
{
    boost::system::error_code ec;
    const auto endpoint = socket_->remote_endpoint(ec);

    // The endpoint may have become disconnected.
    if (ec)
        return config::authority();

    return config::authority(endpoint);
}

void channel_proxy::clear_subscriptions()
{
    version_subscriber_.relay(error::service_stopped, version_type());
    verack_subscriber_.relay(error::service_stopped, verack_type());
    address_subscriber_.relay(error::service_stopped, address_type());
    get_address_subscriber_.relay(error::service_stopped, get_address_type());
    inventory_subscriber_.relay(error::service_stopped, inventory_type());
    get_data_subscriber_.relay(error::service_stopped, get_data_type());
    get_blocks_subscriber_.relay(error::service_stopped, get_blocks_type());
    transaction_subscriber_.relay(error::service_stopped, transaction_type());
    block_subscriber_.relay(error::service_stopped, block_type());
    ping_subscriber_.relay(error::service_stopped, ping_type());
    pong_subscriber_.relay(error::service_stopped, pong_type());
    raw_subscriber_.relay(error::service_stopped, header_type(), data_chunk());
}

bool channel_proxy::stopped() const
{
    return stopped_;
}

void channel_proxy::set_nonce(uint64_t nonce)
{
    nonce_ = nonce;
}

void channel_proxy::reset_timers()
{
    set_timeout(timeouts_.inactivity);
    set_heartbeat(timeouts_.heartbeat);
}

void channel_proxy::reset_revival()
{
    set_revival(timeouts_.revival);
}

void channel_proxy::set_revival_handler(revival_handler handler)
{
    revival_handler_ = handler;
}

void channel_proxy::set_expiration(const time_duration& timeout)
{
    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ec;
    expiration_.cancel(ec);
    expiration_.expires_from_now(timeout, ec);
    expiration_.async_wait(
        std::bind(&channel_proxy::handle_expiration,
            shared_from_this(), _1));
}

void channel_proxy::set_timeout(const time_duration& timeout)
{
    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ec;
    timeout_.cancel(ec);
    timeout_.expires_from_now(timeout, ec);
    timeout_.async_wait(
        std::bind(&channel_proxy::handle_timeout,
            shared_from_this(), _1));
}

void channel_proxy::set_heartbeat(const time_duration& timeout)
{
    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ec;
    heartbeat_.cancel(ec);
    heartbeat_.expires_from_now(timeout, ec);
    heartbeat_.async_wait(
        std::bind(&channel_proxy::handle_heartbeat,
            shared_from_this(), _1));
}

void channel_proxy::set_revival(const time_duration& timeout)
{
    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ec;
    revival_.cancel(ec);
    revival_.expires_from_now(timeout, ec);
    revival_.async_wait(
        std::bind(&channel_proxy::handle_revival,
            shared_from_this(), _1));
}

void channel_proxy::handle_expiration(const boost::system::error_code& ec)
{
    // Did the timer fire because of cancelation?
    if (aborted(ec))
        return;

    log_info(LOG_NETWORK)
        << "Channel expired [" << address() << "]";

    stop(error::service_stopped);
}

void channel_proxy::handle_timeout(const boost::system::error_code& ec)
{
    // Did the timer fire because of cancelation?
    if (aborted(ec))
        return;

    log_info(LOG_NETWORK)
        << "Channel timeout [" << address() << "]";

    stop(error::channel_timeout);
}

void channel_proxy::handle_heartbeat(const boost::system::error_code& ec)
{
    // Did the timer fire because of cancelation?
    if (aborted(ec))
        return;

    const auto ping = [this](const std::error_code& ec)
    {
        if (ec)
        {
            log_debug(LOG_NETWORK)
                << "Ping send failure [" << address() << "] "
                << ec.message();
            return;
        }

        log_debug(LOG_NETWORK)
            << "Ping sent [" << address() << "] ";
    };

    const auto nonce = pseudo_random();

    // Subscribe to pong messages.
    subscribe_pong(
        strand_.wrap(&channel_proxy::handle_pong_message,
            shared_from_this(), _1, _2, nonce));

    const ping_type random_ping = { nonce };
    send(random_ping, ping);
}

void channel_proxy::handle_pong_message(const std::error_code& ec,
    const pong_type& ping, uint64_t nonce)
{
    if (stopped() || aborted(ec))
        return;

    if (ec)
    {
        log_debug(LOG_NETWORK)
            << "Failure getting pong from [" << address() << "]";
        return;
    }

    if (ping.nonce != nonce)
    {
        log_warning(LOG_NETWORK)
            << "Invalid pong nonce from [" << address() << "]";
        stop(error::channel_timeout);
    }
}

void channel_proxy::handle_revival(const boost::system::error_code& ec)
{
    // Failure condition not handled, just abort.
    if (aborted(ec))
        return;

    // Nothing to do, no handler registered.
    if (revival_handler_ == nullptr)
        return;

    revival_handler_(bc::error::boost_to_error_code(ec));
}

// This is a subscription.
void channel_proxy::read_header()
{
    async_read(*socket_, buffer(inbound_header_),
        strand_.wrap(&channel_proxy::handle_read_header,
            shared_from_this(), _1, _2));
}

// This is a subscription.
void channel_proxy::read_checksum(const header_type& header)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_.wrap(&channel_proxy::handle_read_checksum,
            shared_from_this(), _1, _2, header));
}

// This is a subscription.
void channel_proxy::read_payload(const header_type& header)
{
    inbound_payload_.resize(header.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header.payload_length),
        strand_.wrap(&channel_proxy::handle_read_payload,
            shared_from_this(), _1, _2, header));
}

void channel_proxy::handle_read_header(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred))
{
    if (stopped() || aborted(ec))
        return;

    if (ec)
    {
        log_info(LOG_NETWORK)
            << "Channel failure [" << address() << "] "
            << std::error_code(error::boost_to_error_code(ec)).message();

        stop(ec);
        return;
    }

    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    BITCOIN_ASSERT(bytes_transferred == inbound_header_.size());

    auto valid_parse = true;
    header_type header;
    const data_slice buffer(inbound_header_);

    try
    {
        satoshi_load(buffer.begin(), buffer.end(), header);
    }
    catch (bc::end_of_stream)
    {
        valid_parse = false;
    }

    if (!valid_parse || header.magic != magic_value())
    {
        log_warning(LOG_NETWORK) 
            << "Invalid header received [" << address() << "]";
        stop(error::bad_stream);
        return;
    }

    log_debug(LOG_NETWORK)
        << "Receive " << header.command << " [" << address() << "] ("
        << header.payload_length << " bytes)";

    read_checksum(header);
    reset_timers();
}

void channel_proxy::handle_read_checksum(const boost::system::error_code& ec,
    size_t bytes_transferred, header_type& header)
{
    if (stopped())
        return;

    // Client may have disconnected after sending, so allow bad channel.
    if (ec)
    {
        // But make sure we got the required data, as this may fail depending 
        // when the client disconnected.
        if (bytes_transferred != header_checksum_size ||
            bytes_transferred != inbound_checksum_.size())
        {
            // No log error if we aborted, causing the invalid data.
            if (aborted(ec))
                return;

            log_warning(LOG_NETWORK)
                << "Invalid checksum from [" << address() << "] "
                << std::error_code(error::boost_to_error_code(ec)).message();
            stop(ec);
            return;
        }
    }

    header.checksum = from_little_endian<uint32_t>(
        inbound_checksum_.begin(), inbound_checksum_.end());

    read_payload(header);
    reset_timers();
}

void channel_proxy::handle_read_payload(const boost::system::error_code& ec,
    size_t bytes_transferred, const header_type& header)
{
    if (stopped())
        return;

    // Client may have disconnected, so allow bad channel.
    if (ec)
    {
        // But make sure we got the required data, as this may fail depending 
        // when the client disconnected.
        if (bytes_transferred != header.payload_length ||
            bytes_transferred != inbound_payload_.size())
        {
            // No log error if we aborted, causing the invalid data.
            if (aborted(ec))
                return;

            log_warning(LOG_NETWORK)
                << "Invalid payload from [" << address() << "] "
                << std::error_code(error::boost_to_error_code(ec)).message();
            stop(ec);
            return;
        }
    }

    const data_chunk payload(inbound_payload_);
    if (header.checksum != bitcoin_checksum(payload))
    {
        log_warning(LOG_NETWORK) 
            << "Invalid bitcoin checksum from [" << address() << "]";
        stop(error::bad_stream);
        return;
    }

    raw_subscriber_.relay(error::success, header, inbound_payload_);

    // This must happen before calling subscribe notification handlers
    // In case user tries to stop() this channel.
    if (!ec)
        read_header();

    reset_timers();
    loader_.load_lookup(header.command, payload);

    // Now we stop the channel if there was an error and we aren't yet stopped.
    if (ec)
        stop(ec);
}

void channel_proxy::call_handle_send(const boost::system::error_code& ec,
    send_handler handle_send)
{
    auto std_ec = error::success;
    if (stopped())
        std_ec = error::service_stopped;
    else if (ec)
        std_ec = error::boost_to_error_code(ec);

    handle_send(std_ec);
}

void channel_proxy::subscribe_version(
    receive_version_handler handle_receive)
{
    subscribe<version_type>(version_subscriber_, handle_receive);
}

void channel_proxy::subscribe_verack(
    receive_verack_handler handle_receive)
{
    subscribe<verack_type>(verack_subscriber_, handle_receive);
}

void channel_proxy::subscribe_address(
    receive_address_handler handle_receive)
{
    subscribe<address_type>(address_subscriber_, handle_receive);
}

void channel_proxy::subscribe_get_address(
    receive_get_address_handler handle_receive)
{
    subscribe<get_address_type>(get_address_subscriber_, handle_receive);
}

void channel_proxy::subscribe_inventory(
    receive_inventory_handler handle_receive)
{
    subscribe<inventory_type>(inventory_subscriber_, handle_receive);
}

void channel_proxy::subscribe_get_data(
    receive_get_data_handler handle_receive)
{
    subscribe<get_data_type>(get_data_subscriber_, handle_receive);
}

void channel_proxy::subscribe_get_blocks(
    receive_get_blocks_handler handle_receive)
{
    subscribe<get_blocks_type>(get_blocks_subscriber_, handle_receive);
}

void channel_proxy::subscribe_transaction(
    receive_transaction_handler handle_receive)
{
    subscribe<transaction_type>(transaction_subscriber_, handle_receive);
}

void channel_proxy::subscribe_block(
    receive_block_handler handle_receive)
{
    subscribe<block_type>(block_subscriber_, handle_receive);
}

void channel_proxy::subscribe_ping(
    receive_ping_handler handle_receive)
{
    subscribe<ping_type>(ping_subscriber_, handle_receive);
}

void channel_proxy::subscribe_pong(
    receive_pong_handler handle_receive)
{
    subscribe<pong_type>(pong_subscriber_, handle_receive);
}

void channel_proxy::subscribe_raw(receive_raw_handler handle_receive)
{
    if (stopped())
        handle_receive(error::service_stopped, header_type(), data_chunk());
    else
        raw_subscriber_.subscribe(handle_receive);
}

void channel_proxy::subscribe_stop(stop_handler handle_stop)
{
    if (stopped())
        handle_stop(error::service_stopped);
    else
        stop_subscriber_.subscribe(handle_stop);
}

void channel_proxy::send_raw(const header_type& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    if (stopped())
        handle_send(error::service_stopped);
    else
        strand_.queue(
            std::bind(&channel_proxy::do_send_raw,
                shared_from_this(), packet_header, payload, handle_send));
}

void channel_proxy::do_send_raw(const header_type& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    // TODO: eliminate unnecessary copying.
    data_chunk raw_header(satoshi_raw_size(packet_header));
    satoshi_save(packet_header, raw_header.begin());

    // Construct completed packet with header + payload.
    data_chunk message = raw_header;
    extend_data(message, payload);
    do_send_common(message, handle_send, packet_header.command);
}

void channel_proxy::send_common(const data_chunk& message,
    send_handler handle_send, const std::string& command)
{
    if (stopped())
    {
        handle_send(error::service_stopped);
        return;
    }

    const auto self = shared_from_this();
    const auto send_message = [self, message, handle_send, command]()
    {
        self->do_send_common(message, handle_send, command);
    };

    strand_.queue(send_message);
}

void channel_proxy::do_send_common(const data_chunk& message,
    send_handler handle_send, const std::string& command)
{
    log_debug(LOG_NETWORK)
        << "Send " << command << " [" << address() << "] ("
        << message.size() << " bytes)";

    const shared_const_buffer buffer(message);
    async_write(*socket_, buffer,
        std::bind(&channel_proxy::call_handle_send,
            shared_from_this(), _1, handle_send));
}

} // namespace network
} // namespace libbitcoin
