/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_BLOCKS_POLLER_HPP
#define LIBBITCOIN_BLOCKS_POLLER_HPP

#include <bitcoin/blockchain/blockchain.hpp>
#include <bitcoin/define.hpp>
#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

class poller
{
public:
    BC_API poller(threadpool& pool, blockchain& chain);
    BC_API void query(channel_ptr node);
    BC_API void monitor(channel_ptr node);

private:
    void initial_ask_blocks(const std::error_code& ec,
        const block_locator_type& locator, channel_ptr node);

    void receive_inv(const std::error_code& ec,
        const inventory_type& packet, channel_ptr node);
    void receive_block(const std::error_code& ec,
        const block_type& blk, channel_ptr node);

    void handle_store(const std::error_code& ec, block_info info,
        const hash_digest& block_hash, channel_ptr node);
    void ask_blocks(const std::error_code& ec,
        const block_locator_type& locator,
        const hash_digest& hash_stop, channel_ptr node);

    io_service::strand strand_;
    blockchain& chain_;

    // Last hash from a block locator
    hash_digest last_locator_begin_, last_hash_stop_;
    // Last hash from an inventory packet
    hash_digest last_block_hash_;
};

} // namespace libbitcoin

#endif

