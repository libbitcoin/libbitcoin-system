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
#include <bitcoin/bitcoin/network/channel.hpp>

#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/primitives.hpp>

namespace libbitcoin {
namespace network {

using std::placeholders::_1;
using std::placeholders::_2;
using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::posix_time::minutes;
using boost::posix_time::seconds;
using boost::posix_time::time_duration;

// Connection timeout time
const time_duration initial_timeout = seconds(0) + minutes(1);
const time_duration disconnect_timeout = seconds(0) + minutes(90);
const time_duration heartbeat_time = seconds(0) + minutes(30);

template <typename Message>
class channel_loader_module
  : public channel_loader_module_base
{
public:
    typedef std::function<
        void (const std::error_code&, const Message&)> load_handler;

    channel_loader_module(load_handler handle_load)
      : handle_load_(handle_load) {}

    void attempt_load(std::istream& stream)
    {
        try
        {
            Message result(stream);
            handle_load_(std::error_code(), result);
        }
        catch (std::ios_base::failure)
        {
            handle_load_(error::bad_stream, Message());
        }
    }

    const std::string lookup_symbol() const
    {
        return Message::satoshi_command;
    }
private:
    load_handler handle_load_;
};

channel_stream_loader::~channel_stream_loader()
{
    for (channel_loader_module_base* module: modules_)
        delete module;
}

void channel_stream_loader::add(channel_loader_module_base* module)
{
    modules_.push_back(module);
}

void channel_stream_loader::load_lookup(const std::string& symbol,
    std::istream& stream) const
{
    for (channel_loader_module_base* module: modules_)
        if (module->lookup_symbol() == symbol)
            module->attempt_load(stream);
}

channel_proxy::channel_proxy(threadpool& pool, socket_ptr socket)
  : strand_(pool), socket_(socket),
    timeout_(pool.service()), heartbeat_(pool.service()), stopped_(false)
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

    raw_subscriber_ = std::make_shared<raw_subscriber_type>(pool);
    stop_subscriber_ = std::make_shared<stop_subscriber_type>(pool);
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
    auto this_ptr = shared_from_this();
    auto do_stop = [this, this_ptr]()
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
    boost::system::error_code ret_ec;
    timeout_.cancel(ret_ec);
    heartbeat_.cancel(ret_ec);
    // Force the socket closed
    // Should we do something with these error_codes?
    socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ret_ec);
    socket_->close(ret_ec);
    clear_subscriptions();
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

    raw_subscriber_->relay(error::service_stopped,
        message::header(), data_chunk());
}

bool channel_proxy::stopped() const
{
    return stopped_;
}

bool timer_errors(const boost::system::error_code& ec, bool stopped)
{
    if (ec == boost::asio::error::operation_aborted)
        return true;
    else if (ec)
    {
        log_error(LOG_NETWORK) << ec.message();
        return true;
    }
    else if (stopped)
        return true;
    return false;
}

void channel_proxy::handle_timeout(const boost::system::error_code& ec)
{
    if (timer_errors(ec, stopped_))
        return;
    log_debug(LOG_NETWORK) << "Forcing disconnect due to timeout.";
    // No response for a while so disconnect
    boost::system::error_code ret_ec;
    tcp::endpoint remote_endpoint = socket_->remote_endpoint(ret_ec);
    if (!ret_ec)
        log_debug(LOG_NETWORK) << "Closing channel "
            << remote_endpoint.address().to_string();
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
    send(message::ping(), handle_ping);
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
    else if (!ec)
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

void channel_proxy::read_checksum(const message::header& header_msg)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_.wrap(&channel_proxy::handle_read_checksum,
            shared_from_this(), _1, _2, header_msg));
}

void channel_proxy::read_payload(const message::header& header_msg)
{
    inbound_payload_.resize(header_msg.payload_length());
    async_read(*socket_, buffer(inbound_payload_, header_msg.payload_length()),
        strand_.wrap(&channel_proxy::handle_read_payload,
            shared_from_this(), _1, _2, header_msg));
}

void channel_proxy::handle_read_header(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred))
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    data_buffer<uint8_t, char> buffer(inbound_header_.data(), inbound_header_.size());
    std::istream header_stream(&buffer);
    message::header header_msg(header_stream);

    if (header_msg.magic() != magic_value())
    {
        log_debug(LOG_NETWORK) << "Bad header received.";
        stop();
        return;
    }

    log_debug(LOG_NETWORK) << "r: " << header_msg.command()
            << " (" << header_msg.payload_length() << " bytes)";
    read_checksum(header_msg);
    reset_timers();
}

void channel_proxy::handle_read_checksum(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), message::header& header_msg)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    header_msg.checksum(
        from_little_endian<uint32_t>(
            inbound_checksum_.begin(), inbound_checksum_.end()));
    read_payload(header_msg);
    reset_timers();
}

void channel_proxy::handle_read_payload(const boost::system::error_code& ec,
    size_t DEBUG_ONLY(bytes_transferred), const message::header& header_msg)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_msg.payload_length());
    data_chunk payload_data = data_chunk(
        inbound_payload_.begin(), inbound_payload_.end());
    BITCOIN_ASSERT(payload_data.size() == header_msg.payload_length());
    if (header_msg.checksum() != bitcoin_checksum(payload_data))
    {
        log_warning(LOG_NETWORK) << "Bad checksum!";
        raw_subscriber_->relay(error::bad_stream,
            message::header(), data_chunk());
        stop();
        return;
    }
    raw_subscriber_->relay(std::error_code(),
        header_msg, inbound_payload_);

    // This must happen before calling subscribe notification handlers
    // In case user tries to stop() this channel.
    read_header();
    reset_timers();

    const std::string& command = header_msg.command();
    data_buffer<uint8_t, char> payload_buffer(payload_data.data(), payload_data.size());
    std::istream payload_stream(&payload_buffer);
    loader_.load_lookup(command, payload_stream);
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
    generic_subscribe<message::announce_version>(
        handle_receive, version_subscriber_);
}
void channel_proxy::subscribe_verack(receive_verack_handler handle_receive)
{
    generic_subscribe<message::verack>(
        handle_receive, verack_subscriber_);
}
void channel_proxy::subscribe_address(receive_address_handler handle_receive)
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
void channel_proxy::subscribe_block(receive_block_handler handle_receive)
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
    if (stopped_)
        handle_receive(error::service_stopped,
            message::header(), data_chunk());
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

void channel_proxy::send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    if (stopped_)
        handle_send(error::service_stopped);
    else
        strand_.queue(&channel_proxy::do_send_raw,
            shared_from_this(), packet_header, payload, handle_send);
}

void channel_proxy::do_send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    data_chunk raw_header = packet_header;
    // Construct completed packet with header + payload
    data_chunk whole_message = raw_header;
    extend_data(whole_message, payload);
    do_send_common(whole_message, handle_send);
}

void channel_proxy::send_common(const data_chunk& whole_message,
    send_handler handle_send)
{
    if (stopped_)
    {
        handle_send(error::service_stopped);
    }
    else
    {
        auto this_ptr = shared_from_this();
        strand_.queue(
            [this, this_ptr, whole_message, handle_send]
            {
                do_send_common(whole_message, handle_send);
            });
    }
}

void channel_proxy::do_send_common(const data_chunk& whole_message,
    send_handler handle_send)
{
    shared_const_buffer buffer(whole_message);
    async_write(*socket_, buffer,
        std::bind(&channel_proxy::call_handle_send, shared_from_this(),
            std::placeholders::_1, handle_send));
}

// channel
channel::channel(channel_proxy_ptr proxy)
  : weak_proxy_(proxy)
{
}

channel::~channel()
{
    stop();
}

void channel::stop() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->stop();
}

bool channel::stopped() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->stopped();

    return true;
}

channel::address() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->address();

    return authority();
}

void channel::reset_revival()
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->reset_revival();
}

void channel::set_revival_handler(channel_proxy::revivial_handler handler)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->set_revival_handler(handler);
}

void channel::send_raw(const message::header& packet_header,
    const data_chunk& payload, channel_proxy::send_handler handle_send)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->send_raw(packet_header, payload, handle_send);
    else
        handle_send(error::service_stopped);
}

void channel::subscribe_version(
    channel_proxy::receive_version_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_version(handle_receive);
    else
        handle_receive(error::service_stopped, version_type());
}

void channel::subscribe_verack(
    channel_proxy::receive_verack_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_verack(handle_receive);
    else
        handle_receive(error::service_stopped, message::verack());
}

void channel::subscribe_address(
    channel_proxy::receive_address_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_address(handle_receive);
    else
        handle_receive(error::service_stopped, message::address());
}

void channel::subscribe_get_address(
    channel_proxy::receive_get_address_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_get_address(handle_receive);
    else
        handle_receive(error::service_stopped, message::get_address());
}

void channel::subscribe_inventory(
    channel_proxy::receive_inventory_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_inventory(handle_receive);
    else
        handle_receive(error::service_stopped, message::inventory());
}

void channel::subscribe_get_data(
    channel_proxy::receive_get_data_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_get_data(handle_receive);
    else
        handle_receive(error::service_stopped, message::get_data());
}

void channel::subscribe_get_blocks(
    channel_proxy::receive_get_blocks_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_get_blocks(handle_receive);
    else
        handle_receive(error::service_stopped, message::get_blocks());
}

void channel::subscribe_transaction(
    channel_proxy::receive_transaction_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_transaction(handle_receive);
    else
        handle_receive(error::service_stopped, chain::transaction());
}

void channel::subscribe_block(
    channel_proxy::receive_block_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_block(handle_receive);
    else
        handle_receive(error::service_stopped, chain::block());
}

void channel::subscribe_raw(
    channel_proxy::receive_raw_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_raw(handle_receive);
    else
        handle_receive(error::service_stopped, message::header(), data_chunk());
}

void channel::subscribe_stop(
    channel_proxy::stop_handler handle_stop)
{
    const auto proxy = weak_proxy_.lock();

    if (proxy)
        proxy->subscribe_stop(handle_stop);
    else
        handle_stop(error::service_stopped);
}

} // namespace network
} // namespace libbitcoin
