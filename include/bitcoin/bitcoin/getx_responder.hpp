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
#ifndef LIBBITCOIN_GETX_RESPONDER_HPP
#define LIBBITCOIN_GETX_RESPONDER_HPP

#include <system_error>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/threadpool.hpp>
#include <bitcoin/bitcoin/types.hpp>

namespace libbitcoin {

class blockchain;
class transaction_pool;

class getx_responder
{
public:
    BC_API getx_responder(blockchain& chain, transaction_pool& txpool);
    BC_API void monitor(channel_ptr node);

private:
    // We use this object to keep track of the hash_continue and add
    // a shared state to our channel object.
    struct channel_with_state
    {
        channel_ptr node;
        hash_digest hash_continue = null_hash;
    };

    void receive_get_data(const std::error_code& ec,
        const get_data_type& packet, channel_with_state special);

    void pool_tx(const std::error_code& ec, const transaction_type& tx,
        const hash_digest& tx_hash, channel_ptr node);
    void chain_tx(const std::error_code& ec,
        const transaction_type& tx, channel_ptr node);

    void send_block(const std::error_code& ec,
        const block_type& blk, channel_ptr node);

    blockchain& chain_;
    transaction_pool& txpool_;
};

} // libbitcoin

#endif

