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
#ifndef LIBBITCOIN_NETWORK_P2P_HPP
#define LIBBITCOIN_NETWORK_P2P_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connections.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/pending.hpp>
#include <bitcoin/bitcoin/network/session_manual.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// This class provides the top level public interface to the networking layer.
/// All methods except start, run, stop and close are thread safe.
class BC_API p2p
{
public:
    static const settings mainnet;
    static const settings testnet;

    typedef message::network_address address;
    typedef std::function<void(bool)> truth_handler;
    typedef std::function<void(size_t)> count_handler;
    typedef std::function<void(const code&)> result_handler;
    typedef std::function<void(const code&, channel::ptr)> channel_handler;
    typedef std::function<void(const code&, const address&)> address_handler;

    // ------------------------------------------------------------------------

    /// Construct the p2p networking instance.
    p2p(const settings& settings=mainnet);

    /// Ensure all threads are coalesced.
    ~p2p();
    
    /// This class is not copyable.
    p2p(const p2p&) = delete;
    void operator=(const p2p&) = delete;

    // ------------------------------------------------------------------------

    /// Send message to all connections, handler invoked for each channel.
    template <typename Message>
    void broadcast(const Message& message, channel_handler handle_channel,
        result_handler handle_complete)
    {
        dispatch_.ordered(
            std::bind(&network::p2p::do_broadcast<Message>,
                this, message, handle_channel, handle_complete));
    }

    // ------------------------------------------------------------------------

    /// Return the current block height.
    virtual size_t height();

    /// Set the current block height, for use in version messages.
    virtual void set_height(size_t value);

    // ------------------------------------------------------------------------

    /// Invoke startup sequence, call from constructing thread.
    virtual void start(result_handler handler);

    /// Begin long running sessions, call from start result handler.
    virtual void run();

    // ------------------------------------------------------------------------

    /// Non-blocking call to coalesce all work, start may be reinvoked after.
    /// Handler returns the result of host file save operation.
    virtual void stop(result_handler handler);

    /// Non-blocking call to coalesce all work, start may be reinvoked after.
    virtual void stop();

    /// Blocking call to coalesce all work and then terminate all threads.
    /// Call from thread that constructed this class, or don't call at all.
    /// This calls stop, and start may be reinvoked after calling this.
    virtual void close();

    // ------------------------------------------------------------------------

    /// Determine if the nonce is from a pending connection.
    virtual void pent(uint64_t version_nonce, truth_handler handler);

    /// Pend a connection.
    virtual void pend(channel::ptr channel, result_handler handler);

    /// Unpend a connection.
    virtual void unpend(channel::ptr channel, result_handler handler);

    /// Get the number of pending connections.
    virtual void pent_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Determine if there exists a connection to the address.
    virtual void connected(const address& address, truth_handler handler);

    /// Store a connection.
    virtual void store(channel::ptr channel, result_handler handler);

    /// Remove a connection.
    virtual void remove(channel::ptr channel, result_handler handler);

    /// Get the number of connections.
    virtual void connected_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Get a randomly-selected adress.
    virtual void fetch_address(address_handler handler);

    /// Store an address.
    virtual void store(const address& address, result_handler handler);

    /// Store a collection of addresses.
    virtual void store(const address::list& addresses, result_handler handler);

    /// Remove an address.
    virtual void remove(const address& address, result_handler handler);

    /// Get the number of addresses.
    virtual void address_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Maintain a connection to hostname:port.
    virtual void connect(const std::string& hostname, uint16_t port);

    /// Maintain a connection to hostname:port.
    /// The callback is invoked by the first connection creation only.
    virtual void connect(const std::string& hostname, uint16_t port,
        channel_handler handler);

    /// Subscribe to connection creation and service stop events.
    virtual void subscribe(channel_handler handler);

    /// Relay a connection creation or service stop event to subscribers.
    virtual void relay(const code& ec, channel::ptr channel);

private:
    threadpool pool_;

protected:

    /// Attach a session to the network, caller must start the session.
    template <class Session, typename... Args>
    typename Session::ptr attach(Args&&... args)
    {
        return std::make_shared<Session>(pool_, *this,
            std::forward<Args>(args)...);
    }

    virtual bool stopped() const;

    dispatcher dispatch_;

private:
    template <typename Message>
    void do_broadcast(const Message& message, channel_handler handle_channel,
        result_handler handle_complete) const
    {
        connections_.broadcast(message, handle_channel, handle_complete);
    }

    void start();
    void handle_hosts_loaded(const code& ec, result_handler handler);
    void handle_hosts_seeded(const code& ec, result_handler handler);
    void handle_hosts_saved(const code& ec, result_handler handler);

    bool stopped_;
    size_t height_;
    const settings& settings_;
    pending pending_;
    connections connections_;
    hosts hosts_;
    channel::channel_subscriber::ptr subscriber_;
    session_manual::ptr manual_;
};

} // namespace network
} // namespace libbitcoin

#endif
