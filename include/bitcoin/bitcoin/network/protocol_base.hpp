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
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Virtual base class for protocol implementations.
 */
class BC_API protocol_base
  : public std::enable_shared_from_this<protocol_base>
{
protected:
    typedef std::function<void(const code&)> handler;

    /**
     * Construct an address protocol instance.
     * @param[in]  peer      The channel on which to start the protocol.
     * @param[in]  pool      The thread pool used by the dispacher.
     * @param[in]  complete  Callback invoked upon stop if not null.
     */
    protocol_base(channel::ptr peer, threadpool& pool,
        handler complete=nullptr);

    /**
     * Construct an address protocol instance.
     * @param[in]  peer      The channel on which to start the protocol.
     * @param[in]  pool      The thread pool used by the dispacher.
     * @param[in]  timeout   The timer period.
     * @param[in]  complete  Callback invoked upon stop if not null.
     */
    protocol_base(channel::ptr peer, threadpool& pool,
        const asio::duration& timeout, handler complete=nullptr);

    /// Instances of this class are not copyable.
    protocol_base(const protocol_base&) = delete;
    void operator=(const protocol_base&) = delete;
    
    /**
     * Dispatch to the handler in order.
     */
    template <typename Handler, typename... Args>
    void queue(Handler&& handler, Args&&... args) const
    {
        if (stopped())
            return;

        const auto self = shared_from_this();
        const auto stoppable = [self, handler](Args&&... args)
        {
            if (!self->stopped())
                handler(args...);
        };

        dispatch_.queue(stoppable, args...);
    }
    
    /**
     * Subscribe to a message from the peer.
     */
    template <typename Message, typename Handler, typename... Args>
    void accept(Handler&& handler, Args&&... args)
    {
        if (stopped())
            return;

        const auto self = shared_from_this();
        const auto stoppable = [self, handler](Args&&... args)
        {
            if (!self->stopped())
                handler(args...);
        };
        
        peer_->subscribe<Message>(dispatch_.sync(stoppable, args...));
    }
    
    /**
     * Dispatch a message to the peer.
     * This just hides bpeer_ from implementations.
     */
    template <typename Message, typename Handler, typename... Args>
    void send(Message message, Handler&& handler, Args&&... args)
    {
        if (!stopped())
            peer_->send(message, std::bind(handler, args...));
    }
    
    /**
     * Invoke the completion callback.
     */
    void callback(const code& ec) const;
    
    /**
     * Get a shared pointer to the derived instance from this.
     * Required because enable_shared_from_this doesn't support inheritance.
     */
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /**
     * Gets the authority of the peer on this channel.
     */
    config::authority peer() const;
    
    /**
     * Stop the channel.
     */
    void stop(const code& ec);
    
    /**
     * Determine if the channel is stopped.
     */
    bool stopped() const;

private:
    void subscribe_stop();
    void subscribe_timer(threadpool& pool, const asio::duration& timeout);
    void handle_stop(const code& ec);
    void handle_timer(const code& ec) const;

    channel::ptr peer_;
    dispatcher dispatch_;
    const handler callback_;
    bool stopped_;
    deadline::ptr deadline_;
};

} // namespace network
} // namespace libbitcoin

#endif
