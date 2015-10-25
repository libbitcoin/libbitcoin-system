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
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// This class provides the top level public interface to the networking layer.
/// All operations are thread safe
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

    /// Construct the p2p networking instance.
    p2p(const settings& settings=mainnet);

    /// Ensure all threads are coalesced.
    ~p2p();
    
    /// This class is not copyable.
    p2p(const p2p&) = delete;
    void operator=(const p2p&) = delete;

    /// Return the current block height.
    size_t height();

    /// Set the current block height, for use in version messages.
    void set_height(size_t value);

    /// Calls handler after hosts file load and seeding has completed.
    void start(result_handler handler);

    /// Stop after saving hosts file.
    /// Handler indicates hosts save result and is called after all threads
    /// are coalesced. This call is optional.
    void stop(result_handler handler);

    /// Stop without saving hosts file.
    /// This call is optional as desctruct will do the same, although this
    /// allows the caller to block until all threads are coalesced.
    void stop();

    // ------------------------------------------------------------------------

    /// Determine if the nonce is from a pending connection.
    void pent(uint64_t version_nonce, truth_handler handler);

    /// Pend a connection.
    void pend(channel::ptr channel, result_handler handler);

    /// Unpend a connection.
    void unpend(channel::ptr channel, result_handler handler);

    /// Get the number of pending connections.
    void pent_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Determine if there exists a connection to the address.
    void connected(const address& address, truth_handler handler);

    /// Store a connection.
    void store(channel::ptr channel, result_handler handler);

    /// Remove a connection.
    void remove(channel::ptr channel, result_handler handler);

    /// Get the number of connections.
    void connected_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Get a randomly-selected adress.
    void fetch_address(address_handler handler);

    /// Store an address.
    void store(const address& address, result_handler handler);

    /// Store a collection of addresses.
    void store(const address::list& addresses, result_handler handler);

    /// Remove an address.
    void remove(const address& address, result_handler handler);

    /// Get the number of addresses.
    void address_count(count_handler handler);

    // ------------------------------------------------------------------------

    /// Maintain a connection to hostname:port.
    void connect(const std::string& hostname, uint16_t port);

    /// Maintain a connection to hostname:port.
    /// The callback that will fire on first connection established only.
    void connect(const std::string& hostname, uint16_t port,
        channel_handler handler);

    /// Subscribe to connection creation events.
    void subscribe(channel_handler handler);

    /// Relay a connection creation or p2p stop event to subscribers.
    void relay(const code& ec, channel::ptr channel);

    /// Send a message to all connections.
    template <typename Message>
    void broadcast(const Message& message, channel_handler handler)
    {
        dispatch_.ordered(
            std::bind(&network::p2p::do_broadcast<Message>,
                this, message, handler));
    }

private:
    template <class Session, typename... Args>
    typename Session::ptr attach(Args&&... args)
    {
        auto session = std::make_shared<Session>(pool_, *this, settings_);
        session->start(std::forward<Args>(args)...);
        return session;
    }

    template <typename Message>
    void do_broadcast(const Message& message, channel_handler handler) const
    {
        connections_.broadcast(message, handler);
    }

    bool stopped() const;
    void handle_stop(const code& ec, result_handler handler);
    void handle_hosts_loaded(const code& ec, result_handler handler);
    void handle_hosts_seeded(const code& ec, result_handler handler);

    bool stopped_;
    size_t height_;
    const settings& settings_;
    threadpool pool_;
    dispatcher dispatch_;
    pending pending_;
    connections connections_;
    hosts hosts_;
    channel::channel_subscriber::ptr subscriber_;
};

} // namespace network
} // namespace libbitcoin

#endif
