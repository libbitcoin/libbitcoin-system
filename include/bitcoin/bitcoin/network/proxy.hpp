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
#ifndef LIBBITCOIN_NETWORK_PROXY_HPP
#define LIBBITCOIN_NETWORK_PROXY_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <boost/array.hpp>
#include <boost/date_time.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/network/stream_loader.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API proxy
  : public std::enable_shared_from_this<proxy>, track<proxy>
{
public:
    template <class Message>
    using message_handler = std::function<void(const code&, const Message&)>;
    typedef std::function<void(const code&)> send_handler;
    typedef std::function<void(const code&)> stop_handler;
    typedef subscriber<const code&> stop_subscriber;

    typedef std::shared_ptr<proxy> ptr;
    typedef std::function<void(const code&)> handler;

    proxy(asio::socket_ptr socket, threadpool& pool,
        const timeout& timeouts=timeout::defaults);
    ~proxy();

    /// This class is not copyable.
    proxy(const proxy&) = delete;
    void operator=(const proxy&) = delete;

    void start();
    void stop(const code& ec);
    bool stopped() const;
    config::authority address() const;
    void reset_revival();
    void set_revival_handler(handler handler);
    void set_nonce(uint64_t nonce);

    template <class Message>
    void send(const Message& packet, send_handler handler)
    {
        if (stopped())
        {
            handler(error::channel_stopped);
            return;
        }

        const auto& command = Message::command;
        const auto bytes = message::serialize(packet);
        dispatch_.ordered(&proxy::do_send,
            shared_from_this(), bytes, handler, command);
    }

    template <class Message>
    void subscribe(message_handler<Message> handler)
    {
        if (stopped())
        {
            handler(error::channel_stopped, Message());
            return;
        }

        // Subscribing must be immediate, we cannot switch thread contexts.
        // message_subscriber<Message>(handler);
    }

    void subscribe_stop(stop_handler handler);

private:
    typedef byte_source<message::heading::buffer> heading_source;
    typedef boost::iostreams::stream<heading_source> heading_stream;

    typedef byte_source<data_chunk> payload_source;
    typedef boost::iostreams::stream<payload_source> payload_stream;

    ////template <typename Message, class Subscriber>
    ////void proxy::establish_relay(Subscriber subscriber)
    ////{
    ////    const auto handler = [subscriber](const code& ec, const Message& message)
    ////    {
    ////        subscriber->relay(ec, message);
    ////    };
    ////
    ////    stream_loader_.add<Message>(handler);
    ////}

    ////// Subscriber doesn't have an unsubscribe, we just send service_stopped.
    ////// The subscriber then has the option to not resubscribe in the handler.
    ////template <typename Message, class Subscriber>
    ////void proxy::notify_stop(Subscriber subscriber) const
    ////{
    ////    subscriber->relay(error::channel_stopped, Message());
    ////}

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
    
    void read_heading();
    void read_payload(const message::heading& head);

    void handle_read_heading(const boost_code& ec, size_t);
    void handle_read_payload(const boost_code& ec, size_t,
        const message::heading& heading);

    void do_send(const data_chunk& message, handler handler,
        const std::string& command);
    void call_handle_send(const boost_code& ec, handler handler);

    asio::socket_ptr socket_;
    config::authority authority_;
    message::heading::buffer heading_buffer_;
    data_chunk payload_buffer_;
    stream_loader stream_loader_;

    dispatcher dispatch_;

    const timeout& timeouts_;
    deadline::ptr expiration_;
    deadline::ptr inactivity_;
    deadline::ptr revival_;
    handler revival_handler_;

    bool stopped_;
    uint64_t nonce_;

    ////message_subscriber::ptr message_subscriber_;
    stop_subscriber::ptr stop_subscriber_;
};

} // namespace network
} // namespace libbitcoin

#endif

