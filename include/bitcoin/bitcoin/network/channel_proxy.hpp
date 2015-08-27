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
#ifndef LIBBITCOIN_NETWORK_CHANNEL_PROXY_HPP
#define LIBBITCOIN_NETWORK_CHANNEL_PROXY_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <boost/array.hpp>
#include <boost/date_time.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/message/address.hpp>
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/message/get_blocks.hpp>
#include <bitcoin/bitcoin/message/get_data.hpp>
#include <bitcoin/bitcoin/message/header.hpp>
#include <bitcoin/bitcoin/message/inventory.hpp>
//#include <bitcoin/bitcoin/message/not_found.hpp>
#include <bitcoin/bitcoin/message/ping_pong.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/network/channel_stream_loader.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API channel_proxy
  : public std::enable_shared_from_this<channel_proxy>, track<channel_proxy>
{
public:
    typedef std::shared_ptr<channel_proxy> ptr;

    typedef std::function<void(const code&, const message::version&)>
        receive_version_handler;
    typedef std::function<void(const code&, const message::verack&)>
        receive_verack_handler;
    typedef std::function<void(const code&, const message::address&)>
        receive_address_handler;
    typedef std::function<void(const code&, const message::get_address&)>
        receive_get_address_handler;
    typedef std::function<void(const code&, const message::inventory&)>
        receive_inventory_handler;
    typedef std::function<void(const code&,  const message::get_data&)>
        receive_get_data_handler;
    typedef std::function<void(const code&, const message::get_blocks&)>
        receive_get_blocks_handler;
    typedef std::function<void(const code&, const chain::transaction&)>
        receive_transaction_handler;
    typedef std::function<void(const code&, const chain::block&)>
        receive_block_handler;
    typedef std::function<void(const code&, const message::ping&)>
        receive_ping_handler;
    typedef std::function<void(const code&, const message::pong&)>
        receive_pong_handler;

    typedef std::function<void(const code&)> send_handler;
    typedef std::function<void(const code&, const message::header&,
        const data_chunk&)> receive_raw_handler;

    typedef std::function<void(const code&)> stop_handler;
    typedef std::function<void(const code&)> revival_handler;
    typedef std::function<void(const code&)> expiration_handler;

    channel_proxy(asio::socket_ptr socket, threadpool& pool,
        const timeout& timeouts=timeout::defaults);
    ~channel_proxy();

    /// This class is not copyable.
    channel_proxy(const channel_proxy&) = delete;
    void operator=(const channel_proxy&) = delete;

    void start();
    void stop(const code& ec);
    bool stopped() const;
    config::authority address() const;
    void reset_revival();
    void set_revival_handler(revival_handler handler);
    void set_nonce(uint64_t nonce);

    template <typename Message>
    void send(const Message& packet, send_handler handle_send)
    {
        if (stopped())
        {
            handle_send(error::channel_stopped);
            return;
        }

        const auto& command = Message::satoshi_command;
        const auto message = message::create_raw_message(packet);
        dispatch_.ordered(
            std::bind(&channel_proxy::do_send,
                shared_from_this(), message, handle_send, command));
    }

    void send_raw(const message::header& packet_header,
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
    void subscribe_ping(receive_ping_handler handle_receive);
    void subscribe_pong(receive_pong_handler handle_receive);
    void subscribe_raw(receive_raw_handler handle_receive);
    void subscribe_stop(stop_handler handle_stop);

    typedef subscriber<const code&, const message::version&>
        version_subscriber;
    typedef subscriber<const code&, const message::verack&>
        verack_subscriber;
    typedef subscriber<const code&, const message::address&>
        address_subscriber;
    typedef subscriber<const code&, const message::get_address&>
        get_address_subscriber;
    typedef subscriber<const code&, const message::inventory&>
        inventory_subscriber;
    typedef subscriber<const code&, const message::get_data&>
        get_data_subscriber;
    typedef subscriber<const code&, const message::get_blocks&>
        get_blocks_subscriber;
    typedef subscriber<const code&, const chain::transaction&>
        transaction_subscriber;
    typedef subscriber<const code&, const chain::block&>
        block_subscriber;
    typedef subscriber<const code&, const message::ping&>
        ping_subscriber;
    typedef subscriber<const code&, const message::pong&>
        pong_subscriber;
    typedef subscriber<const code&, const message::header&,
        const data_chunk&> raw_subscriber;
    typedef subscriber<const code&> stop_subscriber;

private:
    template<typename Message, class Subscriber>
    void establish_relay(Subscriber subscriber);
    template <typename Message, class Subscriber, typename Callback>
    void subscribe(Subscriber subscriber, Callback handler) const;
    template <typename Message, class Subscriber>
    void notify_stop(Subscriber subscriber) const;

    void stop(const boost_code& ec);
    void do_stop(const code& ec);
    void clear_subscriptions(const code& ec);
    void clear_timers();

    void start_timers();
    void start_expiration();
    void start_inactivity();
    void start_revival();

    void handle_expiration(const code& ec);
    void handle_inactivity(const code& ec);
    void handle_revival(const code& ec);
    
    void read_header();
    void read_checksum(const message::header& header);
    void read_payload(const message::header& header);

    void handle_read_header(const boost_code& ec, size_t bytes_transferred);
    void handle_read_checksum(const boost_code& ec, size_t bytes_transferred,
        message::header& header);
    void handle_read_payload(const boost_code& ec, size_t bytes_transferred,
        const message::header& header);

    void do_send(const data_chunk& message, send_handler handle_send,
        const std::string& command);
    void do_send_raw(const message::header& packet_header,
        const data_chunk& payload, send_handler handle_send);
    void call_handle_send(const boost_code& ec, send_handler handle_send);

    asio::socket_ptr socket_;
    dispatcher dispatch_;
    const timeout& timeouts_;

    deadline::ptr expiration_;
    deadline::ptr inactivity_;
    deadline::ptr revival_;

    revival_handler revival_handler_;
    bool stopped_;
    uint64_t nonce_;
    channel_stream_loader stream_loader_;

    message::header::header_bytes inbound_header_;
    message::header::checksum_bytes inbound_checksum_;
    data_chunk inbound_payload_;

    version_subscriber::ptr version_subscriber_;
    verack_subscriber::ptr verack_subscriber_;
    address_subscriber::ptr address_subscriber_;
    get_address_subscriber::ptr get_address_subscriber_;
    inventory_subscriber::ptr inventory_subscriber_;
    get_data_subscriber::ptr get_data_subscriber_;
    get_blocks_subscriber::ptr get_blocks_subscriber_;
    transaction_subscriber::ptr transaction_subscriber_;
    block_subscriber::ptr block_subscriber_;
    ping_subscriber::ptr ping_subscriber_;
    pong_subscriber::ptr pong_subscriber_;
    raw_subscriber::ptr raw_subscriber_;
    stop_subscriber::ptr stop_subscriber_;
};

} // namespace network
} // namespace libbitcoin

#endif

