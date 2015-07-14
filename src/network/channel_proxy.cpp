/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <boost/iostreams/stream.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/message/announce_version.hpp>
#include <bitcoin/bitcoin/message/nonce.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel_loader_module.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
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
using boost::posix_time::minutes;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;

// TODO: parameterize for config access.
// Connection timeout (these could be safely adjusted by accessors at runtime).
static const auto initial_timeout = seconds(0) + minutes(1);
static const auto disconnect_timeout = seconds(0) + minutes(90);
static const auto heartbeat_time = seconds(0) + minutes(30);
static const auto revival_time = seconds(0) + minutes(1);

channel_proxy::channel_proxy(threadpool& pool, socket_ptr socket)
  : strand_(pool),
    socket_(socket),
    timeout_(pool.service()),
    heartbeat_(pool.service()),
    revival_(pool.service()),
    revival_handler_(nullptr),
    stopped_(false),
    raw_subscriber_(std::make_shared<raw_subscriber_type>(pool)),
    stop_subscriber_(std::make_shared<stop_subscriber_type>(pool))
{
#define CHANNEL_TRANSPORT_MECHANISM(SUBSCRIBER_PREFIX, MESSAGE_TYPE) \
    SUBSCRIBER_PREFIX##_subscriber_ = \
        std::make_shared<SUBSCRIBER_PREFIX##_subscriber_type>(pool); \
    loader_.add(new channel_loader_module<MESSAGE_TYPE>( \
        std::bind(&SUBSCRIBER_PREFIX##_subscriber_type::relay, \
            SUBSCRIBER_PREFIX##_subscriber_, _1, _2)));

    CHANNEL_TRANSPORT_MECHANISM(version, message::announce_version);
    CHANNEL_TRANSPORT_MECHANISM(verack, message::verack);
    CHANNEL_TRANSPORT_MECHANISM(address, message::address);
    CHANNEL_TRANSPORT_MECHANISM(get_address, message::get_address);
    CHANNEL_TRANSPORT_MECHANISM(inventory, message::inventory);
    CHANNEL_TRANSPORT_MECHANISM(get_data, message::get_data);
    CHANNEL_TRANSPORT_MECHANISM(get_blocks, message::get_blocks);
    CHANNEL_TRANSPORT_MECHANISM(transaction, chain::transaction);
    CHANNEL_TRANSPORT_MECHANISM(block, chain::block);

#undef CHANNEL_TRANSPORT_MECHANISM
}

channel_proxy::~channel_proxy()
{
    do_stop();
}

void channel_proxy::start()
{
    read_header();
    set_timeout(initial_timeout);
    set_heartbeat(heartbeat_time);
    set_revival(revival_time);
}

void channel_proxy::stop(const std::error_code& ec)
{
    const auto self = shared_from_this();
    const auto stop_service = [self, ec]()
    {
        self->do_stop(ec);
    };
    strand_.queue(stop_service);
}

void channel_proxy::stop(const boost::system::error_code& ec)
{
    stop(error::boost_to_error_code(ec));
}

void channel_proxy::do_stop(const std::error_code& ec)
{
    stop_impl();
    stop_subscriber_->relay(ec);
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

authority channel_proxy::address() const
{
    boost::system::error_code ec;
    const auto endpoint = socket_->remote_endpoint(ec);

    // The endpoint may have become disconnected.
    if (ec)
        return authority();

    return authority(endpoint);
}

void channel_proxy::clear_subscriptions()
{
#define CHANNEL_CLEAR_SUBSCRIPTION(SUBSCRIBER_PREFIX, MESSAGE_TYPE) \
    SUBSCRIBER_PREFIX##_subscriber_->relay(error::service_stopped, \
        MESSAGE_TYPE());

    CHANNEL_CLEAR_SUBSCRIPTION(version, message::announce_version);
    CHANNEL_CLEAR_SUBSCRIPTION(verack, message::verack);
    CHANNEL_CLEAR_SUBSCRIPTION(address, message::address);
    CHANNEL_CLEAR_SUBSCRIPTION(get_address, message::get_address);
    CHANNEL_CLEAR_SUBSCRIPTION(inventory, message::inventory);
    CHANNEL_CLEAR_SUBSCRIPTION(get_data, message::get_data);
    CHANNEL_CLEAR_SUBSCRIPTION(get_blocks, message::get_blocks);
    CHANNEL_CLEAR_SUBSCRIPTION(transaction, chain::transaction);
    CHANNEL_CLEAR_SUBSCRIPTION(block, chain::block);

#undef CHANNEL_CLEAR_SUBSCRIPTION

    raw_subscriber_->relay(error::service_stopped, message::header(),
        data_chunk());
}

bool channel_proxy::stopped() const
{
    return stopped_;
}

static inline bool aborted(const boost::system::error_code& ec)
{
    return ec == boost::asio::error::operation_aborted;
}

bool channel_proxy::failed(const boost::system::error_code& ec)
{
    // Suppress error if we are already stopped or the call has been aborted.
    if (stopped() || aborted(ec))
        return true;

    if (ec)
    {
        log_info(LOG_NETWORK)
            << "Channel failure [" << address().to_string() << "] "
            << std::error_code(error::boost_to_error_code(ec)).message()
            << " (boost code " << ec.value() << ")";
        stop(ec);
        return true;
    }

    return false;
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

void channel_proxy::handle_heartbeat(const boost::system::error_code& ec)
{
    // Failure condition not handled, just abort.
    if (aborted(ec))
        return;

    const auto handle_ping = [this](const std::error_code& ec)
    {
        if (ec)
        {
            log_debug(LOG_NETWORK)
                << "Ping failure [" << address().to_string() << "] "
                << ec.message();
            return;
        }

        log_debug(LOG_NETWORK)
            << "Ping sent [" << address().to_string() << "] ";
    };

    // TODO: match our sent ping.nonce with the returned pong.nonce.
    message::ping random_ping;
    random_ping.nonce = pseudo_random();
    send(random_ping, handle_ping);
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

void channel_proxy::handle_timeout(const boost::system::error_code& ec)
{
    // Failure condition not handled, just abort.
    if (aborted(ec))
        return;

    log_info(LOG_NETWORK)
        << "Channel timeout [" << address().to_string() << "]";

    stop(error::channel_timeout);
}

void channel_proxy::reset_timers()
{
    set_timeout(disconnect_timeout);
    set_heartbeat(heartbeat_time);
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

void channel_proxy::set_revival_handler(revivial_handler handler)
{
    revival_handler_ = handler;
}

void channel_proxy::handle_revival(const boost::system::error_code& ec)
{
    if (aborted(ec))
        return;

    strand_.queue(
        std::bind(&channel_proxy::do_revivial,
            shared_from_this(), ec));
}

void channel_proxy::do_revivial(const boost::system::error_code& ec)
{
    // Nothing to do, no handler registered.
    if (revival_handler_ == nullptr)
        return;

    log_info(LOG_NETWORK)
        << "Channel revival [" << address().to_string() << "]";

    // Convert to standard code for public interface.
    const auto code = bc::error::boost_to_error_code(ec);

    // Invoke the revival handler.
    revival_handler_(code);
}

void channel_proxy::reset_revival()
{
    set_revival(revival_time);
}

// This is a subscription.
void channel_proxy::read_header()
{
    async_read(*socket_, buffer(inbound_header_),
        strand_.wrap(&channel_proxy::handle_read_header,
            shared_from_this(), _1, _2));
}

// This is a subscription.
void channel_proxy::read_checksum(const message::header& header)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_.wrap(&channel_proxy::handle_read_checksum,
            shared_from_this(), _1, _2, header));
}

// This is a subscription.
void channel_proxy::read_payload(const message::header& header)
{
    inbound_payload_.resize(header.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header.payload_length),
        strand_.wrap(&channel_proxy::handle_read_payload,
            shared_from_this(), _1, _2, header));
}

void channel_proxy::handle_read_header(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred))
{
    if (failed(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    BITCOIN_ASSERT(bytes_transferred == inbound_header_.size());

    container_source<boost::array<uint8_t, header_chunk_size>, uint8_t, char> source(inbound_header_);
    boost::iostreams::stream<container_source<boost::array<uint8_t, header_chunk_size>, uint8_t, char>> header_stream(source);
    message::header header;

    auto valid_parse = header.from_data(header_stream);

    if (!valid_parse || header.magic != magic_value())
    {
        log_warning(LOG_NETWORK) 
            << "Invalid header received [" << address().to_string() << "]";
        stop(error::bad_stream);
        return;
    }

    log_debug(LOG_NETWORK)
        << "Receive " << header.command << " [" 
        << address().to_string() << "] ("
        << header.payload_length << " bytes)";

    read_checksum(header);
    reset_timers();
}

void channel_proxy::handle_read_checksum(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), message::header& header)
{
    if (failed(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    BITCOIN_ASSERT(bytes_transferred == inbound_checksum_.size());

    header.checksum = from_little_endian<uint32_t>(
        inbound_checksum_.begin(), inbound_checksum_.end());

    read_payload(header);
    reset_timers();
}

void channel_proxy::handle_read_payload(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), const message::header& header)
{
    if (failed(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header.payload_length);
    BITCOIN_ASSERT(bytes_transferred == inbound_payload_.size());

    const data_chunk payload(inbound_payload_);
    if (header.checksum != bitcoin_checksum(payload))
    {
        log_warning(LOG_NETWORK) 
            << "Invalid checksum from [" << address().to_string() << "]";

        raw_subscriber_->relay(error::bad_stream, message::header(), data_chunk());
        stop(error::bad_stream);
        return;
    }

    raw_subscriber_->relay(error::success, header, inbound_payload_);

    // This must happen before calling subscribe notification handlers
    // In case user tries to stop() this channel.
    read_header();
    reset_timers();

    byte_source<data_chunk> source(payload);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);

    if (loader_.load_lookup(header.command, istream))
    {
        if (istream.peek() != std::istream::traits_type::eof())
        {
            log_warning(LOG_NETWORK)
                << "Valid message [" << header.command
                << "] handled, unused bytes remain in payload.";
        }
    }
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
    generic_subscribe<message::announce_version>(
        handle_receive, version_subscriber_);
}

void channel_proxy::subscribe_verack(
    receive_verack_handler handle_receive)
{
    generic_subscribe<message::verack>(
        handle_receive, verack_subscriber_);
}

void channel_proxy::subscribe_address(
    receive_address_handler handle_receive)
{
    generic_subscribe<message::address>(
        handle_receive, address_subscriber_);
}

void channel_proxy::subscribe_inventory(
    receive_inventory_handler handle_receive)
{
    generic_subscribe<message::inventory>(
        handle_receive, inventory_subscriber_);
}

void channel_proxy::subscribe_get_data(
    receive_get_data_handler handle_receive)
{
    generic_subscribe<message::get_data>(
        handle_receive, get_data_subscriber_);
}

void channel_proxy::subscribe_get_blocks(
    receive_get_blocks_handler handle_receive)
{
    generic_subscribe<message::get_blocks>(
        handle_receive, get_blocks_subscriber_);
}

void channel_proxy::subscribe_transaction(
    receive_transaction_handler handle_receive)
{
    generic_subscribe<chain::transaction>(
        handle_receive, transaction_subscriber_);
}

void channel_proxy::subscribe_block(
    receive_block_handler handle_receive)
{
    generic_subscribe<chain::block>(
        handle_receive, block_subscriber_);
}

void channel_proxy::subscribe_get_address(
    receive_get_address_handler handle_receive)
{
    generic_subscribe<message::get_address>(
        handle_receive, get_address_subscriber_);
}

void channel_proxy::subscribe_raw(receive_raw_handler handle_receive)
{
    if (stopped())
        handle_receive(error::service_stopped, message::header(), data_chunk());
    else
        raw_subscriber_->subscribe(handle_receive);
}

void channel_proxy::subscribe_stop(stop_handler handle_stop)
{
    if (stopped())
        handle_stop(error::service_stopped);
    else
        stop_subscriber_->subscribe(handle_stop);
}

void channel_proxy::send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    if (stopped())
        handle_send(error::service_stopped);
    else
        strand_.queue(
            std::bind(&channel_proxy::do_send_raw,
                shared_from_this(), packet_header, payload, handle_send));
}

void channel_proxy::do_send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    // TODO: eliminate unnecessary copying.
    data_chunk raw_header = packet_header.to_data();

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
        << "Send " << command << " [" << address().to_string()
        << "] (" << message.size() << " bytes)";

    const shared_const_buffer buffer(message);
    async_write(*socket_, buffer,
        std::bind(&channel_proxy::call_handle_send,
            shared_from_this(), _1, handle_send));
}

} // namespace network
} // namespace libbitcoin
