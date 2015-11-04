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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_DISPATCH_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_DISPATCH_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Base class for protocol implementation.
 * This simplifies calling bind, send and subscribe.
 * Instances of this class are not copyable.
 */
template <class Protocol>
class protocol_dispatch
  : public std::enable_shared_from_this<Protocol>
{
protected:
    /**
     * Construct a base protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  name     The instance name for logging purposes.
     */
    protocol_dispatch(threadpool& pool, channel::ptr channel,
        const std::string& name)
      : pool_(pool), dispatch_(pool), channel_(channel), name_(name)
    {
    }
    
    /// Get a shared pointer to the derived instance from this.
    /// Used by implementations to obtain a shared pointer of the derived type.
    /// Required because enable_shared_from_this doesn't support inheritance.
    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /// Get the address of the channel.
    config::authority authority() const
    {
        return channel_->authority();
    }

    /// Get the protocol name, for logging purposes.
    const std::string& name()
    {
        return name_;
    }

    /// Get the channel nonce.
    uint64_t nonce()
    {
        return channel_->nonce();
    }

    /// Get the threadpool.
    threadpool& pool()
    {
        return pool_;
    }

    /// Set the channel version. This method is not thread safe and must
    /// complete before any other protocol might read the version.
    void set_version(const message::version& value)
    {
        channel_->set_version(value);
    }
    
    /**
     * Stop the channel.
     * @param[in]  ec  The error code indicating the reason for stopping.
     */
    void stop(const code& ec)
    {
        channel_->stop(ec);
    }
    
    /**
     * Determine if the channel is stopped.
     */
    bool stopped() const
    {
        return channel_->stopped();
    }

    /// Bind a method in the derived class.
    template <typename Handler, typename... Args>
    auto bind(Handler&& handler, Args&&... args) ->
        decltype(std::bind(std::forward<Handler>(handler),
            std::shared_ptr<Protocol>(), std::forward<Args>(args)...))
    {
        return std::bind(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    /// Call a method in the derived class.
    template <typename Handler, typename... Args>
    void call(Handler&& handler, Args&&... args)
    {
        dispatch_.ordered(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    /// Send a message on the channel and handle the result.
    template <class Message, typename Handler, typename... Args>
    void send(Message&& packet, Handler&& handler, Args&&... args)
    {
        channel_->send(std::forward<Message>(packet),
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

    /// Subscribed to a channel message.
    template <class Message, typename Handler, typename... Args>
    void subscribe(Handler&& handler, Args&&... args)
    {
        channel_->template subscribe<Message>(
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

    /// Subscribed to a channel stop.
    template <typename Handler, typename... Args>
    void subscribe_stop(Handler&& handler, Args&&... args)
    {
        channel_->subscribe_stop(
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

private:
    threadpool& pool_;
    dispatcher dispatch_;
    channel::ptr channel_;
    const std::string name_;
};

} // namespace network
} // namespace libbitcoin

#endif
