/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_NETWORK_SESSION_HPP
#define LIBBITCOIN_NETWORK_SESSION_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connections.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/delegates.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class p2p;

/// A base class for sessions, where a session maintains the lifetime of a set
/// of channels operating in a context (inbound, outbound, manual, seeding). 
class BC_API session
  : public std::enable_shared_from_this<session>
{
public:
    typedef config::authority authority;
    typedef std::function<void()> stop_handler;
    typedef std::function<void(bool)> truth_handler;
    typedef std::function<void(size_t)> count_handler;
    typedef std::function<void(const code&)> result_handler;
    typedef std::function<void(const code&, channel::ptr)> channel_handler;
    typedef std::function<void(const code&, acceptor::ptr)> accept_handler;
    typedef std::function<void(const code&, const authority&)> host_handler;

    session(threadpool& pool, p2p& network, const settings& settings,
        bool outgoing, bool persistent);
    
    /// This class is not copyable.
    session(const session&) = delete;
    void operator=(const session&) = delete;

    /// Create a new channel acceptor.
    acceptor::ptr create_acceptor();

    /// Create a new channel connector with the configured timeout.
    connector::ptr create_connector();

    /// Start creating connections.
    virtual void start();

protected:

    /// Attach a protocol to a channel.
    template <class Protocol, typename... Args>
    void attach(channel::ptr channel, Args&&... args)
    {
        std::make_shared<Protocol>(pool_, network_, channel)
            ->start(std::forward<Args>(args)...);
    }

    /// Bind a method in the derived class.
    template <class Protocol, typename Handler, typename... Args>
    auto bind(Handler&& handler, Args&&... args) ->
        decltype(std::bind(std::forward<Handler>(handler),
            std::shared_ptr<Protocol>(), std::forward<Args>(args)...))
    {
        return std::bind(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    /// Bind an ordered delegate to a method in the derived class.
    template <class Protocol, typename Handler, typename... Args>
    auto ordered(Handler&& handler, Args&&... args) ->
        delegate::ordered<decltype(std::bind(std::forward<Handler>(handler),
            std::shared_ptr<Protocol>(), std::forward<Args>(args)...))>
    {
        return dispatch_.ordered_delegate(std::forward<Handler>(handler),
            shared_from_base<Protocol>(), std::forward<Args>(args)...);
    }

    bool stopped() const;
    void subscribe_stop(stop_handler handler);

    acceptor::ptr listener();

    void address_count(count_handler handler);
    void fetch_address(host_handler handler);
    
    bool blacklisted(const authority& authority) const;
    void connection_count(count_handler handler);
    void register_channel(channel::ptr channel, result_handler handle_started,
        result_handler handle_stopped);

    const settings& settings_;

private:

    // Required because enable_shared_from_this doesn't support inheritance.
    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

    void handle_stop();
    void handle_channel(const code& ec, channel::ptr channel,
        stop_handler handler);

    void handle_pend(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_handshake(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_is_pending(bool pending, channel::ptr channel,
        result_handler handle_started);
    void handle_stored(const code& ec, channel::ptr channel,
        result_handler handle_started);

    void unpend(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_unpend(const code& ec);

    void handle_started(const code& ec, channel::ptr channel,
        result_handler handle_started, result_handler handle_stopped);

    void remove(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_remove(const code& ec);

    bool stopped_;
    bool incoming_;
    bool notify_;
    threadpool& pool_;
    p2p& network_;
    dispatcher dispatch_;
};

} // namespace network
} // namespace libbitcoin

#endif
