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
#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/channel_loader_module.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/satoshi_serialize.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
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

channel_proxy::channel_proxy(threadpool& pool, socket_ptr socket)
  : strand_(pool),
    socket_(socket),
    timeout_(pool.service()), 
    heartbeat_(pool.service()),
    stopped_(false),
    raw_subscriber_(std::make_shared<raw_subscriber_type>(pool)),
    stop_subscriber_(std::make_shared<stop_subscriber_type>(pool))
{
#define CHANNEL_TRANSPORT_MECHANISM(MESSAGE_TYPE) \
    MESSAGE_TYPE##_subscriber_ = \
        std::make_shared<MESSAGE_TYPE##_subscriber_type>(pool); \
    loader_.add(new channel_loader_module<MESSAGE_TYPE##_type>( \
        std::bind(&MESSAGE_TYPE##_subscriber_type::relay, \
            MESSAGE_TYPE##_subscriber_, _1, _2)));

    CHANNEL_TRANSPORT_MECHANISM(version);
    CHANNEL_TRANSPORT_MECHANISM(verack);
    CHANNEL_TRANSPORT_MECHANISM(address);
    CHANNEL_TRANSPORT_MECHANISM(get_address);
    CHANNEL_TRANSPORT_MECHANISM(inventory);
    CHANNEL_TRANSPORT_MECHANISM(get_data);
    CHANNEL_TRANSPORT_MECHANISM(get_blocks);
    CHANNEL_TRANSPORT_MECHANISM(transaction);
    CHANNEL_TRANSPORT_MECHANISM(block);

#undef CHANNEL_TRANSPORT_MECHANISM
}

channel_proxy::~channel_proxy()
{
    stop_impl();
    stop_subscriber_->relay(error::service_stopped);
}

void channel_proxy::start()
{
    read_header();
    set_timeout(initial_timeout);
    set_heartbeat(heartbeat_time);
}

void channel_proxy::stop()
{
    // TODO: review scope.
    const auto self = shared_from_this();
    const auto do_stop = [this, self]()
    {
        stop_impl();
        stop_subscriber_->relay(error::service_stopped);
    };

    strand_.queue(do_stop);
}

void channel_proxy::stop_impl()
{
    if (stopped_)
        return;

    // We need this because the timeout timer shares this code with stop()
    // But sends a different error_code
    stopped_ = true;

    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code error;
    timeout_.cancel(error);
    heartbeat_.cancel(error);

    // Force the socket closed
    // Should we do something with these error_codes?
    socket_->shutdown(tcp::socket::shutdown_both, error);
    socket_->close(error);
    clear_subscriptions();
}

static std::string to_string(const tcp::socket::endpoint_type& endpoint)
{
    const auto ip = bc::split(endpoint.address().to_string(), ".");
    BITCOIN_ASSERT(ip.size() == 4);
    const auto host = format("%03s.%03s.%03s.%03s:%d")
        % ip[0] % ip[1] % ip[2] % ip[3] % endpoint.port();

    return host.str();
}

std::string channel_proxy::address() const
{
    boost::system::error_code error;
    const auto endpoint = socket_->remote_endpoint(error);

    // This assumes that the endpoint has since become disconnected.
    if (error)
        return "000.000.000.000:0";

    return to_string(endpoint);
}

void channel_proxy::clear_subscriptions()
{
#define CHANNEL_CLEAR_SUBSCRIPTION(MESSAGE_TYPE) \
    MESSAGE_TYPE##_subscriber_->relay(error::service_stopped, \
        MESSAGE_TYPE##_type());

    CHANNEL_CLEAR_SUBSCRIPTION(version);
    CHANNEL_CLEAR_SUBSCRIPTION(verack);
    CHANNEL_CLEAR_SUBSCRIPTION(address);
    CHANNEL_CLEAR_SUBSCRIPTION(get_address);
    CHANNEL_CLEAR_SUBSCRIPTION(inventory);
    CHANNEL_CLEAR_SUBSCRIPTION(get_data);
    CHANNEL_CLEAR_SUBSCRIPTION(get_blocks);
    CHANNEL_CLEAR_SUBSCRIPTION(transaction);
    CHANNEL_CLEAR_SUBSCRIPTION(block);

#undef CHANNEL_CLEAR_SUBSCRIPTION

    raw_subscriber_->relay(error::service_stopped, header_type(),
        data_chunk());
}

bool channel_proxy::stopped() const
{
    return stopped_;
}

static bool timer_errors(const boost::system::error_code& ec, bool stopped)
{
    if (ec == boost::asio::error::operation_aborted)
        return true;

    if (ec)
    {
        log_error(LOG_NETWORK) << ec.message();
        return true;
    }

    return stopped;
}

void channel_proxy::handle_timeout(const boost::system::error_code& ec)
{
    if (timer_errors(ec, stopped_))
        return;

    log_debug(LOG_NETWORK) 
        << "Closing channel [" << address() << "] due to timeout.";

    stop_impl();
    stop_subscriber_->relay(error::channel_timeout);
}

void handle_ping(const std::error_code&)
{
    // if there's a problem sending then this channel will be stopped
}

void channel_proxy::handle_heartbeat(const boost::system::error_code& ec)
{
    if (timer_errors(ec, stopped_))
        return;

    send(ping_type(), handle_ping);
}

void channel_proxy::set_timeout(const time_duration timeout)
{
    timeout_.cancel();
    timeout_.expires_from_now(timeout);
    timeout_.async_wait(strand_.wrap(
        &channel_proxy::handle_timeout, shared_from_this(), _1));
}

void channel_proxy::set_heartbeat(const time_duration timeout)
{
    heartbeat_.cancel();
    heartbeat_.expires_from_now(timeout);
    heartbeat_.async_wait(std::bind(
        &channel_proxy::handle_heartbeat, shared_from_this(), _1));
}

void channel_proxy::reset_timers()
{
    set_timeout(disconnect_timeout);
    set_heartbeat(heartbeat_time);
}

bool channel_proxy::problems_check(const boost::system::error_code& ec)
{
    if (stopped_)
        return true;

    if (!ec)
        return false;

    stop();
    return true;
}

void channel_proxy::read_header()
{
    async_read(*socket_, buffer(inbound_header_),
        strand_.wrap(&channel_proxy::handle_read_header,
            shared_from_this(), _1, _2));
}

void channel_proxy::read_checksum(const header_type& header)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_.wrap(&channel_proxy::handle_read_checksum,
            shared_from_this(), _1, _2, header));
}

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
    if (problems_check(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    BITCOIN_ASSERT(bytes_transferred == inbound_header_.size());

    header_type header;
    const data_slice buffer(inbound_header_);
    satoshi_load(buffer.begin(), buffer.end(), header);

    if (header.magic != magic_value())
    {
        log_warning(LOG_NETWORK) 
            << "Invalid header received [" << address() << "]";
        stop();
        return;
    }

    log_debug(LOG_NETWORK) 
        << "Receive " << header.command << " [" << address() << "] ("
        << header.payload_length << " bytes)";

    read_checksum(header);
    reset_timers();
}

void channel_proxy::handle_read_checksum(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), header_type& header)
{
    if (problems_check(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    BITCOIN_ASSERT(bytes_transferred == inbound_checksum_.size());

    header.checksum = from_little_endian<uint32_t>(
        inbound_checksum_.begin(), inbound_checksum_.end());

    read_payload(header);
    reset_timers();
}

void channel_proxy::handle_read_payload(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), const header_type& header)
{
    if (problems_check(ec))
        return;

    BITCOIN_ASSERT(bytes_transferred == header.payload_length);
    BITCOIN_ASSERT(bytes_transferred == inbound_payload_.size());

    const data_chunk payload(inbound_payload_);
    if (header.checksum != bitcoin_checksum(payload))
    {
        log_warning(LOG_NETWORK) 
            << "Invalid checksum received [" << address() << "]";

        raw_subscriber_->relay(error::bad_stream, header_type(), data_chunk());
        stop();
        return;
    }

    raw_subscriber_->relay(std::error_code(), header, inbound_payload_);

    // This must happen before calling subscribe notification handlers
    // In case user tries to stop() this channel.
    read_header();
    reset_timers();

    loader_.load_lookup(header.command, payload);
}

void channel_proxy::call_handle_send(const boost::system::error_code& ec,
    send_handler handle_send)
{
    if (problems_check(ec))
        handle_send(error::service_stopped);
    else
        handle_send(std::error_code());
}

void channel_proxy::subscribe_version(receive_version_handler handle_receive)
{
    generic_subscribe<version_type>(
        handle_receive, version_subscriber_);
}

void channel_proxy::subscribe_verack(receive_verack_handler handle_receive)
{
    generic_subscribe<verack_type>(
        handle_receive, verack_subscriber_);
}

void channel_proxy::subscribe_address(receive_address_handler handle_receive)
{
    generic_subscribe<address_type>(
        handle_receive, address_subscriber_);
}

void channel_proxy::subscribe_inventory(
    receive_inventory_handler handle_receive)
{
    generic_subscribe<inventory_type>(
        handle_receive, inventory_subscriber_);
}

void channel_proxy::subscribe_get_data(
    receive_get_data_handler handle_receive)
{
    generic_subscribe<get_data_type>(
        handle_receive, get_data_subscriber_);
}

void channel_proxy::subscribe_get_blocks(
    receive_get_blocks_handler handle_receive)
{
    generic_subscribe<get_blocks_type>(
        handle_receive, get_blocks_subscriber_);
}

void channel_proxy::subscribe_transaction(
    receive_transaction_handler handle_receive)
{
    generic_subscribe<transaction_type>(
        handle_receive, transaction_subscriber_);
}

void channel_proxy::subscribe_block(receive_block_handler handle_receive)
{
    generic_subscribe<block_type>(
        handle_receive, block_subscriber_);
}

void channel_proxy::subscribe_get_address(
    receive_get_address_handler handle_receive)
{
    generic_subscribe<get_address_type>(
        handle_receive, get_address_subscriber_);
}

void channel_proxy::subscribe_raw(receive_raw_handler handle_receive)
{
    if (stopped_)
        handle_receive(error::service_stopped, header_type(), data_chunk());
    else
        raw_subscriber_->subscribe(handle_receive);
}

void channel_proxy::subscribe_stop(stop_handler handle_stop)
{
    if (stopped_)
        handle_stop(error::service_stopped);
    else
        stop_subscriber_->subscribe(handle_stop);
}

void channel_proxy::send_raw(const header_type& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    if (stopped_)
        handle_send(error::service_stopped);
    else
        strand_.queue(&channel_proxy::do_send_raw,
            shared_from_this(), packet_header, payload, handle_send);
}

// TODO: eliminate unnecessary copying.
void channel_proxy::do_send_raw(const header_type& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    data_chunk raw_header(satoshi_raw_size(packet_header));
    satoshi_save(packet_header, raw_header.begin());

    // Construct completed packet with header + payload
    data_chunk whole_message = raw_header;
    extend_data(whole_message, payload);
    do_send_common(whole_message, handle_send, packet_header.command);
}

void channel_proxy::send_common(const data_chunk& whole_message,
    send_handler handle_send, const std::string& command)
{
    if (stopped_)
    {
        handle_send(error::service_stopped);
        return;
    }

    // TODO: review scope.
    const auto self = shared_from_this();
    const auto enqueue = [this, self, whole_message, handle_send, command]()
    {
        do_send_common(whole_message, handle_send, command);
    };

    strand_.queue(enqueue);
}

void channel_proxy::do_send_common(const data_chunk& whole_message,
    send_handler handle_send, const std::string& command)
{
    log_debug(LOG_NETWORK) << "Send " << command << " [" << address()
        << "] (" << whole_message.size() << " bytes)";

    const shared_const_buffer buffer(whole_message);
    async_write(*socket_, buffer,
        std::bind(&channel_proxy::call_handle_send, shared_from_this(),
            std::placeholders::_1, handle_send));
}

} // namespace network
} // namespace libbitcoin
