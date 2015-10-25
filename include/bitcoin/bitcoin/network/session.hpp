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
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

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
    template <class Protocol, typename... Args>
    void attach(Args&&... args)
    {
        std::make_shared<Protocol>(pool_, network_, settings_,
            std::forward<Args>(args)...)->start();
    }

    template <class Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
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

    dispatcher dispatch_;
    const settings& settings_;

private:
    void handle_stop();
    void handle_channel(const code& ec, channel::ptr channel,
        stop_handler handler);

    void handle_pend(const code& ec, channel::ptr channel,
        result_handler handle_started, result_handler handle_stopped);
    void handle_handshake(const code& ec, channel::ptr channel,
        result_handler handle_started, result_handler handle_stopped);
    void handle_is_pending(bool pending, channel::ptr channel,
        result_handler handle_started, result_handler handle_stopped);
    void handle_stored(const code& ec, channel::ptr channel,
        result_handler handle_started, result_handler handle_stopped);

    void handle_started(const code& ec, result_handler handle_started);
    void unpend(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_unpend(const code& ec);

    void handle_stopped(const code& reason, result_handler handle_stopped);
    void remove(const code& ec, channel::ptr channel,
        result_handler handle_started);
    void handle_remove(const code& ec);

    bool stopped_;
    bool incoming_;
    bool temporary_;
    threadpool& pool_;
    p2p& network_;
};

} // namespace network
} // namespace libbitcoin

#endif
