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
#ifndef LIBBITCOIN_NETWORK_SEEDER_HPP
#define LIBBITCOIN_NETWORK_SEEDER_HPP

#include <cstddef>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/initiator.hpp>
#include <bitcoin/bitcoin/network/protocol_version.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API seeder
  : public std::enable_shared_from_this<seeder>, track<seeder>
{
public:
    typedef std::function<void(const code&)> handler;
    typedef std::function<void(const code&, channel::ptr)> ptr;

    static const config::endpoint::list defaults;

    seeder(threadpool& pool, hosts& hosts, const timeout& timeouts,
        initiator& network, const config::endpoint::list& seeds,
        const message::network_address& self);
    ~seeder();

    /// This class is not copyable.
    seeder(const seeder&) = delete;
    void operator=(const seeder&) = delete;

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
    initiator& network_;
    const config::endpoint::list& seeds_;
    const message::network_address self_;
};

} // namespace network
} // namespace libbitcoin

#endif

