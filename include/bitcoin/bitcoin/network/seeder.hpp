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
#ifndef LIBBITCOIN_SEEDER_HPP
#define LIBBITCOIN_SEEDER_HPP

#include <cstddef>
#include <memory>
#include <system_error>
#include <vector>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/peer_to_peer.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API seeder
    : public std::enable_shared_from_this<seeder>
{
public:
    typedef std::function<void(const std::error_code&)> completion_handler;

    static const config::endpoint::list defaults;

    seeder(threadpool& pool, hosts& hosts, handshake& shake,
        peer_to_peer& network, const config::endpoint::list& seeds,
        const network_address_type& self);
    seeder::~seeder();

    /// This class is not copyable.
    seeder(const seeder&) = delete;
    void operator=(const seeder&) = delete;

    void start(completion_handler handle_seeded);

private:
    void start_connect(const config::endpoint& seed,
        completion_handler handle_complete);
    void handle_seeded(const std::error_code& ec, size_t host_start_size,
        completion_handler handle_complete);
    void handle_connected(const std::error_code& ec, channel_ptr node,
        const config::endpoint& seed, completion_handler handle_complete);
    void handle_handshake(const std::error_code& ec, channel_ptr node,
        const config::endpoint& seed, completion_handler handle_complete);
    void handle_store(const std::error_code& ec);

    sequencer sequence_;
    threadpool& pool_;
    hosts& hosts_;
    handshake& handshake_;
    peer_to_peer& network_;
    const config::endpoint::list& seeds_;
    const network_address_type self_;
    config::endpoint::list remaining_;
};

} // namespace network
} // namespace libbitcoin

#endif

