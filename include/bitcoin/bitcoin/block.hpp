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
#ifndef LIBBITCOIN_BLOCK_HPP
#define LIBBITCOIN_BLOCK_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>

namespace libbitcoin {

// A list of indices. Used for creating block_locator objects or storing list 
// of unconfirmed input indexes in tx pool.
// TODO: move to libbitcoin-blockchain::transaction_pool (interface break).
typedef std::vector<size_t> index_list;

enum class block_status
{
    orphan,
    confirmed,
    rejected
};

struct block_info
{
    block_status status;
    size_t height;
};

BC_API bool operator==(
    const block_header_type& block_a, const block_header_type& block_b);

BC_API uint64_t block_value(size_t height);
BC_API hash_number block_work(uint32_t bits);
BC_API hash_digest hash_block_header(const block_header_type& header);
BC_API index_list block_locator_indexes(size_t top_height);
BC_API block_type genesis_block();

} // namespace libbitcoin

#endif

