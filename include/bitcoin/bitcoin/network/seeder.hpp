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
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API seeder
{
public:
    typedef std::function<void(const std::error_code&)> completion_handler;

    static const config::endpoint::list defaults;

    seeder(threadpool& pool, hosts& hosts, handshake& shake, network& net,
        const config::endpoint::list& seeds);

    /// This class is not copyable.
    seeder(const seeder&) = delete;
    void operator=(const seeder&) = delete;

    void start(completion_handler handle_complete);

private:
    void contact(const config::endpoint& seed);
    void connect(const std::error_code& ec, const config::endpoint& seed,
        channel_ptr node);
    void visit();

    void handle_send(const std::error_code& ec);
    void handle_store_all(const std::error_code& ec,
        const address_type& message, channel_ptr node);
    void handle_store_one(const std::error_code& ec);

    async_strand strand_;
    hosts& host_pool_;
    handshake& handshake_;
    network& network_;
    const config::endpoint::list& seeds_;

    size_t visited_;
    bool success_;
    completion_handler handle_complete_;
};

} // namespace network
} // namespace libbitcoin

#endif

