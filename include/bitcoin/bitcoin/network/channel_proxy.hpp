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
#ifndef LIBBITCOIN_CHANNEL_PROXY_HPP
#define LIBBITCOIN_CHANNEL_PROXY_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <system_error>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel_stream_loader.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/satoshi_serialize.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>

namespace libbitcoin {
namespace network {

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

// Defined here because of the central position in the dependency graph.
typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;

// TODO: eliminate excess copying here.
// TODO: move to serializer|misc|message.hpp/ipp.
template <typename Message>
data_chunk create_raw_message(const Message& packet)
{
    data_chunk payload(satoshi_raw_size(packet));
    satoshi_save(packet, payload.begin());

    // Make the header packet and serialise it.
    header_type header;
    header.magic = magic_value();
    header.command = satoshi_command(packet);
    header.payload_length = static_cast<uint32_t>(payload.size());
    header.checksum = bitcoin_checksum(payload);
    data_chunk raw_header(satoshi_raw_size(header));
    satoshi_save(header, raw_header.begin());

    // Construct completed packet with header + payload.
    data_chunk message = raw_header;
    extend_data(message, payload);
    return message;
}

class BC_API channel_proxy
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
    typedef std::function<void (const std::error_code&)> revivial_handler;

    channel_proxy(threadpool& pool, socket_ptr socket);
    ~channel_proxy();

    /// This class is not copyable.
    channel_proxy(const channel_proxy&) = delete;
    void operator=(const channel_proxy&) = delete;

    void start();
    void stop(const std::error_code& ec=error::service_stopped);
    bool stopped() const;
    authority address() const;
    void reset_revival();
    void set_revival_handler(revivial_handler handler);

    template <typename Message>
    void send(const Message& packet, send_handler handle_send)
    {
        send_common(create_raw_message(packet), handle_send,
            satoshi_command(packet));
    }

    void send_raw(const header_type& packet_header,
        const data_chunk& payload, send_handler handle_send);

    // TODO: reorder args to put command first and required (interface break).
    void send_common(const data_chunk& message, send_handler handle_send,
        const std::string& command="unknown");

    void subscribe_version(receive_version_handler handle_receive);
    void subscribe_verack(receive_verack_handler handle_receive);
    void subscribe_address(receive_address_handler handle_receive);
    void subscribe_get_address(
        receive_get_address_handler handle_receive);
    void subscribe_inventory(receive_inventory_handler handle_receive);
    void subscribe_get_data(receive_get_data_handler handle_receive);
    void subscribe_get_blocks(
        receive_get_blocks_handler handle_receive);
    void subscribe_transaction(
        receive_transaction_handler handle_receive);
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
    typedef subscriber<const std::error_code&, const header_type&,
        const data_chunk&> raw_subscriber_type;
    typedef subscriber<const std::error_code&> stop_subscriber_type;

    void stop(const boost::system::error_code& ec);
    void do_stop(const std::error_code& ec=error::service_stopped);
    void do_send_raw(const header_type& packet_header,
        const data_chunk& payload, send_handler handle_send);
    void do_send_common(const data_chunk& message,
        send_handler handle_send, const std::string& command="");

    template <typename Message, typename Callback, typename SubscriberPtr>
    void generic_subscribe(Callback handle_message,
        SubscriberPtr message_subscribe)
    {
        // Subscribing must be immediate, we cannot switch thread contexts.
        if (stopped())
            handle_message(error::service_stopped, Message());
        else
            message_subscribe->subscribe(handle_message);
    }

    void read_header();
    void read_checksum(const header_type& header);
    void read_payload(const header_type& header);
    void handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred);
    void handle_read_checksum(const boost::system::error_code& ec,
        size_t bytes_transferred, header_type& header);
    void handle_read_payload(const boost::system::error_code& ec,
        size_t bytes_transferred, const header_type& header);
    void call_handle_send(const boost::system::error_code& ec,
        send_handler handle_send);

    void reset_timers();
    void stop_impl();
    void clear_subscriptions();
    bool failed(const boost::system::error_code& ec);
    void set_timeout(const boost::posix_time::time_duration& timeout);
    void handle_timeout(const boost::system::error_code& ec);
    void set_heartbeat(const boost::posix_time::time_duration& timeout);
    void handle_heartbeat(const boost::system::error_code& ec);
    void set_revival(const boost::posix_time::time_duration& timeout);
    void handle_revival(const boost::system::error_code& ec);
    void do_revivial(const boost::system::error_code& ec);

    async_strand strand_;
    socket_ptr socket_;

    // We keep the service alive for lifetime rules
    boost::asio::deadline_timer timeout_;
    boost::asio::deadline_timer heartbeat_;
    boost::asio::deadline_timer revival_;
    revivial_handler revival_handler_;

    // TODO: use lock-free std::atomic_flag?
    std::atomic<bool> stopped_;

    channel_stream_loader loader_;

    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static BC_CONSTEXPR size_t header_chunk_size = 20;
    static BC_CONSTEXPR size_t header_checksum_size = 4;

    // boost1.54/linux/clang/libstdc++-4.8 error if std::array
    // could not match 'boost::array' against 'std::array'
    boost::array<uint8_t, header_chunk_size> inbound_header_;
    boost::array<uint8_t, header_checksum_size> inbound_checksum_;

    data_chunk inbound_payload_;

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

} // namespace network
} // namespace libbitcoin

#endif

