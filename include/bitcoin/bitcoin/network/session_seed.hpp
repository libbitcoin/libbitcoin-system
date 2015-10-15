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
#ifndef LIBBITCOIN_NETWORK_SESSION_SEED_HPP
#define LIBBITCOIN_NETWORK_SESSION_SEED_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API session_seed
  : public std::enable_shared_from_this<session_seed>, track<session_seed>
{
public:
    typedef std::function<void(const code&)> handler;
    typedef std::function<void(const code&, channel::ptr)> ptr;

    static const config::endpoint::list mainnet;
    static const config::endpoint::list testnet;

    session_seed(threadpool& pool, hosts& hosts, const timeout& timeouts,
        connector& network, const config::endpoint::list& seeds,
        const message::network_address& self);
    ~session_seed();

    /// This class is not copyable.
    session_seed(const session_seed&) = delete;
    void operator=(const session_seed&) = delete;

    void start(handler handle_seeded);

private:
    void start_connect(const config::endpoint& seed, handler complete);
    void handle_stopped(size_t host_start_size, handler complete);
    void handle_connected(const code& ec, channel::ptr peer,
        const config::endpoint& seed, handler complete);
    void handle_handshake(const code& ec, channel::ptr peer,
        const config::endpoint& seed, handler complete);

    dispatcher dispatch_;
    threadpool& pool_;
    hosts& hosts_;
    const timeout& timeouts_;
    connector& network_;
    const config::endpoint::list& seeds_;
    const message::network_address self_;
};

} // namespace network
} // namespace libbitcoin

#endif

