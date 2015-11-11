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

class BC_API proxy
  : public std::enable_shared_from_this<proxy>
{
public:
    typedef subscriber<const code&> stop_subscriber;
    typedef std::function<void(const code&)> result_handler;

    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    proxy(threadpool& pool, asio::socket_ptr socket, uint32_t magic);
    ~proxy();

    /// This class is not copyable.
    proxy(const proxy&) = delete;
    void operator=(const proxy&) = delete;

    template <class Message, typename Handler>
    void send(Message&& packet, Handler&& handler)
    {
        if (stopped())
        {
            handler(error::channel_stopped);
            return;
        }

        using namespace message;
        const auto bytes = serialize(std::forward<Message>(packet), magic_);
        const auto handle_send = std::forward<Handler>(handler);
        dispatch_.ordered(&proxy::do_send,
            shared_from_this(), bytes, handle_send, packet.command);
    }

    template <class Message, typename Handler>
    void subscribe(Handler&& handler)
    {
        if (stopped())
        {
            handler(error::channel_stopped, Message());
            return;
        }

        // Subscribing must be immediate, we cannot switch thread contexts.
        message_subscriber_.subscribe<Message>(std::forward<Handler>(handler));
    }

    void talk();
    void start();
    bool stopped() const;
    void stop(const code& ec);
    void subscribe_stop(result_handler handler);
    const config::authority& authority() const;

protected:
    virtual void handle_activity() = 0;
    virtual void handle_stopping() = 0;

private:
    typedef byte_source<message::heading::buffer> heading_source;
    typedef boost::iostreams::stream<heading_source> heading_stream;

    typedef byte_source<data_chunk> payload_source;
    typedef boost::iostreams::stream<payload_source> payload_stream;

    static config::authority authority_factory(asio::socket_ptr socket);

    void stop(const boost_code& ec);
    void do_stop(const code& ec);

    void read_heading();
    void handle_read_heading(const boost_code& ec, size_t);

    void read_payload(const message::heading& head);
    void handle_read_payload(const boost_code& ec, size_t,
        const message::heading& heading);

    void call_handle_send(const boost_code& ec, result_handler handler);
    void do_send(const data_chunk& message, result_handler handler,
        const std::string& command);

    bool stopped_;
    uint32_t magic_;
    dispatcher dispatch_;
    asio::socket_ptr socket_;
    config::authority authority_;
    message_subscriber message_subscriber_;
    stop_subscriber::ptr stop_subscriber_;
    message::heading::buffer heading_buffer_;
    data_chunk payload_buffer_;
};

} // namespace network
} // namespace libbitcoin

#endif

