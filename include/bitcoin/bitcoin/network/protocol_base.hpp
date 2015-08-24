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
     * @param[in]  channel   The channel on which to start the protocol.
     * @param[in]  pool      The thread pool used by the dispacher.
     * @param[in]  complete  Callback invoked upon stop if not null.
     */
    protocol_base(channel::ptr channel, threadpool& pool,
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
     * Get a shared pointer to the derived instance from this.
     * Required because enable_shared_from_this doesn't support inheritance.
     */
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /**
     * Gets the authority of this channel.
     */
    config::authority authority() const;
    
    /**
     * Invoke the completion callback.
     */
    void callback(const code& ec) const;
    
    /**
     * Stop the channel.
     */
    void stop(const code& ec);
    
    /**
     * Determine if the channel is stopped.
     */
    bool stopped() const;

    // TODO: make these private, eliminate macros, check stopped.
    channel::ptr channel_;
    dispatcher dispatch_;

private:
    void subscribe_stop();
    void subscribe_timer(threadpool& pool, const asio::duration& timeout);
    void handle_stop(const code& ec);
    void handle_timer(const code& ec) const;

    const handler callback_;
    bool stopped_;
    deadline::ptr deadline_;
};

// For use in derived protocols for simplifying bind() calls.
#define BC_BIND0(method, Type) \
    std::bind(&Type::method, shared_from_base<Type>())
#define BC_BIND1(method, Type, a1) \
    std::bind(&Type::method, shared_from_base<Type>(), a1)
#define BC_BIND2(method, Type, a1, a2) \
    std::bind(&Type::method, shared_from_base<Type>(), a1, a2)
#define BC_BIND3(method, Type, a1, a2, a3) \
    std::bind(&Type::method, shared_from_base<Type>(), a1, a2, a3)

// For use in derived protocols for simplifying send() calls.
#define BC_SEND0(instance, method, Type) \
    channel_->send(instance, dispatch_.sync(&Type::method, \
    shared_from_base<Type>()))
#define BC_SEND1(instance, method, Type, a1) \
    channel_->send(instance, dispatch_.sync(&Type::method, \
    shared_from_base<Type>(), a1))
#define BC_SEND2(instance, method, Type, a1, a2) \
    channel_->send(instance, dispatch_.sync(&Type::method, \
    shared_from_base<Type>(), a1, a2))
#define BC_SEND3(instance, method, Type, a1, a2, a3) \
    channel_->send(instance, dispatch_.sync(&Type::method, \
shared_from_base<Type>(), a1, a2, a3))

// For use in derived protocols for simplifying subscribe() calls.
#define BC_RECEIVE0(Message, method, Type) \
    channel_->subscribe_##Message(dispatch_.sync(&Type::method, \
    shared_from_base<Type>()))
#define BC_RECEIVE1(Message, method, Type, a1) \
    channel_->subscribe_##Message(dispatch_.sync(&Type::method, \
    shared_from_base<Type>(), a1))
#define BC_RECEIVE2(Message, method, Type, a1, a2) \
    channel_->subscribe_##Message(dispatch_.sync(&Type::method, \
    shared_from_base<Type>(), a1, a2))
#define BC_RECEIVE3(Message, method, Type, a1, a2, a3) \
    channel_->subscribe_##Message(dispatch_.sync(&Type::method, \
    shared_from_base<Type>(), a1, a2, a3))

} // namespace network
} // namespace libbitcoin

#endif
