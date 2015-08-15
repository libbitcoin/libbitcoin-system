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
#ifndef LIBBITCOIN_SEEDS_HPP
#define LIBBITCOIN_SEEDS_HPP

#include <cstddef>
#include <system_error>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/peer.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API seeder
{
public:
    typedef std::function<void(const std::error_code&)> seeded_handler;

    static const config::endpoint::list defaults;

    seeder(threadpool& pool, hosts& hosts, handshake& shake, peer& network,
        const config::endpoint::list& seeds);

    /// This class is not copyable.
    seeder(const seeder&) = delete;
    void operator=(const seeder&) = delete;

    void start(seeded_handler handle_seeded);

private:
    void connect(const config::endpoint& seed,
        seeded_handler completion_callback);
    void handle_connected(const std::error_code& ec, channel_ptr node,
        const config::endpoint& seed, seeded_handler completion_callback);
    void handle_handshake(const std::error_code& ec, channel_ptr node,
        const config::endpoint& seed, seeded_handler completion_callback);
    void handle_stop(const std::error_code& ec, const config::endpoint& seed,
        seeded_handler completion_callback);
    void handle_synced(const std::error_code& ec, size_t host_start_count,
        seeded_handler completion_callback);
    void handle_receive(const std::error_code& ec, const address_type& message,
        const config::endpoint& seed, channel_ptr node, 
        seeded_handler completion_callback);
    void handle_send(const std::error_code& ec, const config::endpoint& seed,
        seeded_handler completion_callback);
    void handle_store(const std::error_code& ec);

    sequencer strand_;
    hosts& host_pool_;
    handshake& handshake_;
    peer& network_;
    const config::endpoint::list& seeds_;

    config::endpoint::list remaining_;
};

} // namespace network
} // namespace libbitcoin

#endif

