/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_NET_CHANNEL_HPP
#define LIBBITCOIN_NET_CHANNEL_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stack>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/asio/streambuf.hpp>
#include <bitcoin/define.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/primitives.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/hash.hpp>
#include <bitcoin/utility/subscriber.hpp>

namespace libbitcoin {

template <typename Message>
data_chunk create_raw_message(const Message& packet)
{
    data_chunk payload(satoshi_raw_size(packet));
    satoshi_save(packet, payload.begin());
    // Make the header packet and serialise it
    header_type head;
    head.magic = magic_value();
    head.command = satoshi_command(packet);
    head.payload_length = payload.size();
    head.checksum = bitcoin_checksum(payload);
    data_chunk raw_header(satoshi_raw_size(head));
    satoshi_save(head, raw_header.begin());
    // Construct completed packet with header + payload
    data_chunk whole_message = raw_header;
    extend_data(whole_message, payload);
    // Probably not the right place for this
    // Networking output in an exporter
    log_debug(LOG_NETWORK) << "s: " << head.command
        << " (" << payload.size() << " bytes)";
    return whole_message;
}

class channel_loader_module_base
{
public:
    virtual ~channel_loader_module_base() {}
    virtual void attempt_load(const data_chunk& stream) const = 0;
    virtual const std::string lookup_symbol() const = 0;
};

class channel_stream_loader
{
public:
    ~channel_stream_loader();
    void add(channel_loader_module_base* module);
    void load_lookup(const std::string& symbol,
        const data_chunk& stream) const;
private:
    typedef std::vector<channel_loader_module_base*> module_list;

    module_list modules_;
};

class channel_proxy
  : public std::enable_shared_from_this<channel_proxy>
{
public:
    typedef std::function<void (const std::error_code&)> send_handler;

    typedef std::function<void (const std::error_code&,
        const version_type&)> receive_version_handler;

    typedef std::function<void (const std::error_code&,
        const verack_type&)> receive_verack_handler;

    typedef std::function<void (const std::error_code&,
        const address_type&)> receive_address_handler;

    typedef std::function<void (const std::error_code&,
        const get_address_type&)> receive_get_address_handler;

    typedef std::function<void (const std::error_code&,
        const inventory_type&)> receive_inventory_handler;

    typedef std::function<void (const std::error_code&,
        const get_data_type&)> receive_get_data_handler;

    typedef std::function<void (const std::error_code&,
        const get_blocks_type&)> receive_get_blocks_handler;

    typedef std::function<void (const std::error_code&,
        const transaction_type&)> receive_transaction_handler;

    typedef std::function<void (const std::error_code&,
        const block_type&)> receive_block_handler;

    typedef std::function<void (const std::error_code&,
        const header_type&, const data_chunk&)> receive_raw_handler;

    typedef std::function<void (const std::error_code&)> stop_handler;

    channel_proxy(threadpool& pool, socket_ptr socket);
    ~channel_proxy();

    channel_proxy(const channel_proxy&) = delete;
    void operator=(const channel_proxy&) = delete;

    void start();
    void stop();
    bool stopped() const;

    // List of bitcoin messages
    // version
    // verack
    // addr
    // getaddr
    // inv
    // getdata
    // getblocks
    // tx
    // block
    // getheaders   [unused]
    // headers      [unused]
    // checkorder   [deprecated]
    // submitorder  [deprecated]
    // reply        [deprecated]
    // ping         [internal]
    // alert        [not supported]

    template <typename Message>
    void send(const Message& packet, send_handler handle_send)
    {
        if (stopped_)
            handle_send(error::service_stopped);
        else
        {
            auto this_ptr = shared_from_this();
            strand_.post(
                [this, this_ptr, packet, handle_send]
                {
                    do_send_common(create_raw_message(packet), handle_send);
                });
        }
    }
    void send_raw(const header_type& packet_header,
        const data_chunk& payload, send_handler handle_send);

    void subscribe_version(receive_version_handler handle_receive);
    void subscribe_verack(receive_verack_handler handle_receive);
    void subscribe_address(receive_address_handler handle_receive);
    void subscribe_get_address(receive_get_address_handler handle_receive);
    void subscribe_inventory(receive_inventory_handler handle_receive);
    void subscribe_get_data(receive_get_data_handler handle_receive);
    void subscribe_get_blocks(receive_get_blocks_handler handle_receive);
    void subscribe_transaction(receive_transaction_handler handle_receive);
    void subscribe_block(receive_block_handler handle_receive);
    void subscribe_raw(receive_raw_handler handle_receive);

    void subscribe_stop(stop_handler handle_stop);

private:
    typedef subscriber<const std::error_code&, const version_type&>
        version_subscriber_type;
    typedef subscriber<const std::error_code&, const verack_type&>
        verack_subscriber_type;
    typedef subscriber<const std::error_code&, const address_type&>
        address_subscriber_type;
    typedef subscriber<const std::error_code&, const get_address_type&>
        get_address_subscriber_type;
    typedef subscriber<const std::error_code&, const inventory_type&>
        inventory_subscriber_type;
    typedef subscriber<const std::error_code&, const get_data_type&>
        get_data_subscriber_type;
    typedef subscriber<const std::error_code&, const get_blocks_type&>
        get_blocks_subscriber_type;
    typedef subscriber<const std::error_code&, const transaction_type&>
        transaction_subscriber_type;
    typedef subscriber<const std::error_code&, const block_type&>
        block_subscriber_type;

    typedef subscriber<const std::error_code&,
        const header_type&, const data_chunk&> raw_subscriber_type;
    typedef subscriber<const std::error_code&> stop_subscriber_type;

    void do_send_raw(const header_type& packet_header,
        const data_chunk& payload, send_handler handle_send);
    BC_API void do_send_common(const data_chunk& whole_message,
        send_handler handle_send);

    template <typename Message, typename Callback, typename SubscriberPtr>
    void generic_subscribe(Callback handle_message,
        SubscriberPtr message_subscribe)
    {
        // Subscribing must be immediate. We cannot switch thread contexts
        if (stopped_)
            handle_message(error::service_stopped, Message());
        else
            message_subscribe->subscribe(handle_message);
    }

    void read_header();
    void read_checksum(const header_type& header_msg);
    void read_payload(const header_type& header_msg);

    void handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred);
    void handle_read_checksum(const boost::system::error_code& ec,
        size_t bytes_transferred, header_type& header_msg);
    void handle_read_payload(const boost::system::error_code& ec,
        size_t bytes_transferred, const header_type& header_msg);

    // Calls the send handler after a successful send, translating
    // the boost error_code to std::error_code
    void call_handle_send(const boost::system::error_code& ec,
        send_handler handle_send);

    void handle_timeout(const boost::system::error_code& ec);
    void handle_heartbeat(const boost::system::error_code& ec);

    void set_timeout(const boost::posix_time::time_duration timeout);
    void set_heartbeat(const boost::posix_time::time_duration timeout);
    void reset_timers();

    bool problems_check(const boost::system::error_code& ec);
    void stop_impl();
    void clear_subscriptions();

    io_service::strand strand_;
    socket_ptr socket_;
    // We keep the service alive for lifetime rules
    boost::asio::deadline_timer timeout_, heartbeat_;
    std::atomic<bool> stopped_;

    channel_stream_loader loader_;

    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static constexpr size_t header_chunk_size = 20;
    // Checksum size is 4 bytes
    static constexpr size_t header_checksum_size = 4;

    byte_array<header_chunk_size> inbound_header_;
    byte_array<header_checksum_size> inbound_checksum_;
    std::vector<uint8_t> inbound_payload_;

    // We should be using variadic templates for these
    version_subscriber_type::ptr version_subscriber_;
    verack_subscriber_type::ptr verack_subscriber_;
    address_subscriber_type::ptr address_subscriber_;
    get_address_subscriber_type::ptr get_address_subscriber_;
    inventory_subscriber_type::ptr inventory_subscriber_;
    get_data_subscriber_type::ptr get_data_subscriber_;
    get_blocks_subscriber_type::ptr get_blocks_subscriber_;
    transaction_subscriber_type::ptr transaction_subscriber_;
    block_subscriber_type::ptr block_subscriber_;

    raw_subscriber_type::ptr raw_subscriber_;
    stop_subscriber_type::ptr stop_subscriber_;
};

class channel
{
public:
    typedef std::shared_ptr<channel_proxy> channel_proxy_ptr;

    BC_API channel(channel_proxy_ptr proxy);
    BC_API ~channel();

    BC_API void stop();
    BC_API bool stopped() const;

    template <typename Message>
    void send(const Message& packet,
        channel_proxy::send_handler handle_send)
    {
        channel_proxy_ptr proxy = weak_proxy_.lock();
        if (!proxy)
            handle_send(error::service_stopped);
        else
            proxy->send(packet, handle_send);
    }

    BC_API void send_raw(const header_type& packet_header,
        const data_chunk& payload, channel_proxy::send_handler handle_send);

    BC_API void subscribe_version(
        channel_proxy::receive_version_handler handle_receive);
    BC_API void subscribe_verack(
        channel_proxy::receive_verack_handler handle_receive);
    BC_API void subscribe_address(
        channel_proxy::receive_address_handler handle_receive);
    BC_API void subscribe_get_address(
        channel_proxy::receive_get_address_handler handle_receive);
    BC_API void subscribe_inventory(
        channel_proxy::receive_inventory_handler handle_receive);
    BC_API void subscribe_get_data(
        channel_proxy::receive_get_data_handler handle_receive);
    BC_API void subscribe_get_blocks(
        channel_proxy::receive_get_blocks_handler handle_receive);
    BC_API void subscribe_transaction(
        channel_proxy::receive_transaction_handler handle_receive);
    BC_API void subscribe_block(
        channel_proxy::receive_block_handler handle_receive);
    BC_API void subscribe_raw(
        channel_proxy::receive_raw_handler handle_receive);

    BC_API void subscribe_stop(
        channel_proxy::stop_handler handle_stop);

private:
    std::weak_ptr<channel_proxy> weak_proxy_;
};

} // namespace libbitcoin

#endif

