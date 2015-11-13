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
class BC_API protocol
  : public std::enable_shared_from_this<protocol>
{
protected:
    /**
     * Construct a base protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  name     The instance name for logging purposes.
     */
    protocol(threadpool& pool, channel::ptr channel,
        const std::string& name);

    /// This class is not copyable.
    protocol(const protocol&) = delete;
    void operator=(const protocol&) = delete;

    /// Bind a method in the derived class.
    template <class Protocol, typename Handler, typename... Args>
    auto bind(Handler&& handler, Args&&... args) ->
        decltype(std::bind(std::forward<Handler>(handler),
            std::shared_ptr<Protocol>(), std::forward<Args>(args)...))
    {
        return std::bind(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    /// Call a method in the derived class.
    template <class Protocol, typename Handler, typename... Args>
    void call(Handler&& handler, Args&&... args)
    {
        dispatch_.ordered(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    /// Send a message on the channel and handle the result.
    template <class Protocol, class Message, typename Handler, typename... Args>
    void send(Message&& packet, Handler&& handler, Args&&... args)
    {
        channel_->send(std::forward<Message>(packet),
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

    /// Subscribe to all channel messages.
    template <class Protocol, class Message, typename Handler, typename... Args>
    void subscribe(Handler&& handler, Args&&... args)
    {
        channel_->template subscribe<Message>(
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

    /// Subscribe to the channel stop.
    template <class Protocol, typename Handler, typename... Args>
    void subscribe_stop(Handler&& handler, Args&&... args)
    {
        channel_->subscribe_stop(
            dispatch_.ordered_delegate(std::forward<Handler>(handler),
                shared_from_base<Protocol>(), std::forward<Args>(args)...));
    }

    /// Get the address of the channel.
    config::authority authority() const;

    /// Get the protocol name, for logging purposes.
    const std::string& name();

    /// Get the channel nonce.
    uint64_t nonce();

    /// Get the threadpool.
    threadpool& pool();

    /// Set the channel version. This method is not thread safe and must
    /// complete before any other protocol might read the version.
    void set_version(const message::version& value);

    /// Stop the channel.
    void stop(const code& ec);

    /// Determine if the channel is stopped.
    bool stopped() const;

private:

    // Required because enable_shared_from_this doesn't support inheritance.
    template <class Protocol>
    std::shared_ptr<Protocol> shared_from_base()
    {
        return std::static_pointer_cast<Protocol>(shared_from_this());
    }

    threadpool& pool_;
    dispatcher dispatch_;
    channel::ptr channel_;
    const std::string name_;
};

} // namespace network
} // namespace libbitcoin

#endif
