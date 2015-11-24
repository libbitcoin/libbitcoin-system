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
#include <bitcoin/bitcoin/network/message_subscriber.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// Manages all socket communication, thread safe.
class BC_API proxy
  : public std::enable_shared_from_this<proxy>
{
public:
    template <class Message>
    using message_handler = std::function<void(const code&, const Message&)>;
    typedef std::function<void(const code&)> result_handler;
    typedef std::function<void()> completion_handler;
    typedef subscriber<const code&> stop_subscriber;
    typedef std::shared_ptr<proxy> ptr;

    /// Construct an instance.
    proxy(threadpool& pool, asio::socket_ptr socket, uint32_t magic);
    ~proxy();

    /// This class is not copyable.
    proxy(const proxy&) = delete;
    void operator=(const proxy&) = delete;

    /// Send a message on the socket.
    template <class Message>
    void send(const Message& packet, result_handler handler)
    {
        const auto command = packet.command;
        const auto data = message::serialize(packet, magic_);
        const auto self = shared_from_this();
        auto call = [self, command, data](result_handler handler)
        {
            self->do_send(data, handler, command);
        };

        dispatch_.unordered(call, handler);
    }

    /// Subscribe to messages of the specified type on the socket.
    template <class Message>
    void subscribe(message_handler<Message> handler)
    {
        const auto self = shared_from_this();
        auto call = [self](message_handler<Message> handler)
        {
            if (self->stopped())
                handler(error::channel_stopped, Message());
            else
                self->message_subscriber_.subscribe<Message>(handler);
        };

        // See comments in proxy::subscribe_stop.
        if (starting_)
            call(handler);
        else
            dispatch_.unordered(call, handler);
    }

    /// Subscribe to the stop event.
    virtual void subscribe_stop(result_handler handler);

    /// Get the authority of the far end of this socket.
    virtual const config::authority& authority() const;

    /// Read messages from this socket.
    virtual void start(result_handler handler);

    /// Stop reading or sending messages on this socket.
    virtual void stop(const code& ec);

protected:
    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    virtual bool stopped() const;
    virtual void handle_activity() = 0;
    virtual void handle_stopping() = 0;

private:
    typedef byte_source<message::heading::buffer> heading_source;
    typedef boost::iostreams::stream<heading_source> heading_stream;
    typedef byte_source<data_chunk> payload_source;
    typedef boost::iostreams::stream<payload_source> payload_stream;

    static config::authority authority_factory(asio::socket_ptr socket);

    // Unordered, to protect registration.
    void stop(const boost_code& ec);
    void do_stop(const code& ec);
    void do_subscribe_stop(result_handler handler);

    // Ordered, to protect socket and read sequence.
    void read_heading();
    void handle_read_heading(const boost_code& ec, size_t);

    // Ordered, to protect socket and read sequence.
    void read_payload(const message::heading& head);
    void handle_read_payload(const boost_code& ec, size_t,
        const message::heading& heading);

    // Unordered, to protect socket and limit shutdown log messages.
    void call_handle_send(const boost_code& ec, result_handler handler);
    void do_send(const data_chunk& message, result_handler handler,
        const std::string& command);
    
    bool stopped_;
    bool starting_;

    const uint32_t magic_;
    const config::authority authority_;

    dispatcher dispatch_;
    asio::socket_ptr socket_;

    message_subscriber message_subscriber_;
    stop_subscriber::ptr stop_subscriber_;

    message::heading::buffer heading_buffer_;
    data_chunk payload_buffer_;
};

} // namespace network
} // namespace libbitcoin

#endif

