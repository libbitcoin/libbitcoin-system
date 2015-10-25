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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Virtual base class for protocol implementations.
 */
class BC_API protocol
  : public std::enable_shared_from_this<protocol>
{
public:
    typedef std::function<void(const code&)> completion_handler;

    /**
     * Starts the protocol, release any reference after calling.
     * A protocol instance is not restartable.
     */
    virtual void start();

protected:

    /**
     * Construct an address protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  name     The instance name for logging purposes.
     * @param[in]  handler  Callback invoked upon protocol stop.
     */
    protocol(threadpool& pool, channel::ptr channel, const std::string& name,
        completion_handler handler=nullptr);

    /**
     * Construct an address protocol instance.
     * @param[in]  pool     The thread pool used by the dispacher.
     * @param[in]  channel  The channel on which to start the protocol.
     * @param[in]  timeout  The timer period.
     * @param[in]  name     The instance name for logging purposes.
     * @param[in]  handler  Callback invoked upon timer expiration.
     */
    protocol(threadpool& pool, channel::ptr channel,
        const asio::duration& timeout, const std::string& name,
        completion_handler handler=nullptr);

    /// Instances of this class are not copyable.
    protocol(const protocol&) = delete;
    void operator=(const protocol&) = delete;
    
    /**
     * Get a shared pointer to the derived instance from this.
     * Used by implementations to obtain a shared pointer of the derived type.
     * Required because enable_shared_from_this doesn't support inheritance.
     */
    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    /**
     * Gets the authority of this channel.
     */
    config::authority authority() const;
    
    /**
     * Invoke the completion handler.
     * @param[in]  ec  The error code of the preceding operation.
     */
    void complete(const code& ec) const;
    
    /**
     * Set the completion callback. Only call in start, before subscriptions.
     * This is used to defer the callback configuration until after construct
     * so as to enable self-referential callbacks (that use shared_from_this).
     * @param[in]  handler  The completion handler.
     */
    void set_handler(completion_handler handler);

    /**
     * Set the channel version.
     * @param[in]  value  The version message.
     */
    void set_version(const message::version& value);
    
    /**
     * Stop the channel.
     * @param[in]  ec  The error code indicating the reason for stopping.
     */
    void stop(const code& ec);
    
    /**
     * Determines if the channel is stopped.
     */
    bool stopped() const;

    /// This could probably be private.
    dispatcher dispatch_;

private:
    void subscribe_stop();
    void subscribe_timer(threadpool& pool, const asio::duration& timeout);
    void handle_stop(const code& ec);
    void handle_timer(const code& ec);

    const std::string name_;
    deadline::ptr deadline_;
    channel::ptr channel_;
    completion_handler completion_handler_;

    // Deferred startup function.
    std::function<void()> start_;
};

} // namespace network
} // namespace libbitcoin

#endif
