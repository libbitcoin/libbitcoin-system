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

#include <bitcoin/types.hpp>
#include <bitcoin/threadpool.hpp>
#include <bitcoin/primitives.hpp>

namespace libbitcoin {

class blockchain;
class transaction_pool;

class getx_responder
{
public:
    getx_responder(threadpool& pool,
        blockchain& chain, transaction_pool& txpool);
    void monitor(channel_ptr node);

private:
    void receive_get_data(const std::error_code& ec,
        const get_data_type packet, channel_ptr node);

    void pool_tx(const std::error_code& ec, const transaction_type& tx,
        const hash_digest& tx_hash, channel_ptr node);
    void chain_tx(const std::error_code& ec,
        const transaction_type& tx, channel_ptr node);

    void send_block(const std::error_code& ec,
        const block_type blk, channel_ptr node);

    io_service& service_;
    blockchain& chain_;
    transaction_pool& txpool_;
};

} // libbitcoin

#endif

