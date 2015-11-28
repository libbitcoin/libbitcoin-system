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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_BASE_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_BASE_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define PROTOCOL_ARGS(handler, args) \
    std::forward<Handler>(handler), \
    shared_from_base<Protocol>(), \
    std::forward<Args>(args)...
#define BOUND_PROTOCOL(handler, args) \
    std::bind(PROTOCOL_ARGS(handler, args))

#define PROTOCOL_ARGS_TYPE(handler, args) \
    std::forward<Handler>(handler), \
    std::shared_ptr<Protocol>(), \
    std::forward<Args>(args)...
#define BOUND_PROTOCOL_TYPE(handler, args) \
    std::bind(PROTOCOL_ARGS_TYPE(handler, args))

/// Virtual base class for protocol implementation, mostly thread safe.
class BC_API protocol
  : public enable_shared_from_base<protocol>
{
protected:
    typedef std::function<void()> completion_handler;
    typedef std::function<void(const code&)> event_handler;

    /// Construct an instance.
    protocol(threadpool& pool, channel::ptr channel,
        const std::string& name);

    /// This class is not copyable.
    protocol(const protocol&) = delete;
    void operator=(const protocol&) = delete;

    /// Bind a method in the derived class.
    template <class Protocol, typename Handler, typename... Args>
    auto bind(Handler&& handler, Args&&... args) ->
        decltype(BOUND_PROTOCOL_TYPE(handler, args))
    {
        return BOUND_PROTOCOL(handler, args);
    }

    /// Send a message on the channel and handle the result.
    template <class Protocol, class Message, typename Handler, typename... Args>
    void send(Message&& packet, Handler&& handler, Args&&... args)
    {
        channel_->send(std::forward<Message>(packet),
            BOUND_PROTOCOL(handler, args));
    }

    /// Subscribe to all channel messages, blocking until subscribed.
    template <class Protocol, class Message, typename Handler, typename... Args>
    void subscribe(Handler&& handler, Args&&... args)
    {
        channel_->template subscribe<Message>(BOUND_PROTOCOL(handler, args));
    }

    /// Subscribe to the channel stop, blocking until subscribed.
    template <class Protocol, typename Handler, typename... Args>
    void subscribe_stop(Handler&& handler, Args&&... args)
    {
        channel_->subscribe_stop(BOUND_PROTOCOL(handler, args));
    }

    /// Get the address of the channel.
    virtual config::authority authority() const;

    /// Get the protocol name, for logging purposes.
    virtual const std::string& name() const;

    /// Get the channel nonce.
    virtual uint64_t nonce();

    /// Get the threadpool.
    virtual threadpool& pool();

    /// Set the channel version. This method is NOT thread safe and must
    /// complete before any other thread could read the peer version.
    void set_peer_version(const message::version& value);

    /// Get the peer version message. This method is NOT thread safe and must
    /// not be called if any other thread could write the peer version.
    virtual const message::version& peer_version();

    /// Stop the channel (and the protocol).
    virtual void stop(const code& ec);

private:
    threadpool& pool_;
    channel::ptr channel_;
    const std::string name_;
};

#undef HANDLER_PROTOCOL_ARGS
#undef BOUND_PROTOCOL

#define BIND1(method, p1) \
    bind<CLASS>(&CLASS::method, p1)
#define BIND2(method, p1, p2) \
    bind<CLASS>(&CLASS::method, p1, p2)
#define BIND3(method, p1, p2, p3) \
    bind<CLASS>(&CLASS::method, p1, p2, p3)

#define SEND1(message, method, p1) \
    send<CLASS>(message, &CLASS::method, p1)
#define SEND2(message, method, p1, p2) \
    send<CLASS>(message, &CLASS::method, p1, p2)

#define SUBSCRIBE2(message, method, p1, p2) \
    subscribe<CLASS, message>(&CLASS::method, p1, p2)
#define SUBSCRIBE3(message, method, p1, p2, p3) \
    subscribe<CLASS, message>(&CLASS::method, p1, p2, p3)

#define SUBSCRIBE_STOP1(method, p1) \
    subscribe_stop<CLASS>(&CLASS::method, p1)

} // namespace network
} // namespace libbitcoin

#endif
