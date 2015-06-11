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
#include <memory>
#include <system_error>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/network/protocol.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>

namespace libbitcoin {
namespace network {

class BC_API seeder
  : public std::enable_shared_from_this<seeder>
{
public:
    seeder(protocol* proto, const hosts::authority_list& seeds,
        protocol::completion_handler handle_complete);

    /// This class is not copyable.
    seeder(const seeder&) = delete;
    void operator=(const seeder&) = delete;

    void start();

private:
    void contact(const authority& seed_address);
    void handle_request(const std::error_code& ec);
    void handle_store(const std::error_code& ec);
    void request(const std::error_code& ec, channel_ptr seed_node);
    void store(const std::error_code& ec, const address_type& packet,
        channel_ptr seed_node);
    void visited(const std::error_code& ec);

    async_strand& strand_;
    hosts& host_pool_;
    handshake& handshake_;
    network& network_;
    bool succeeded_;
    size_t visited_;
    const hosts::authority_list& seeds_;
    const protocol::completion_handler handle_complete_;
};

} // namespace network
} // namespace libbitcoin

#endif

