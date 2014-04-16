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
#ifndef LIBBITCOIN_TRANSACTION_HPP
#define LIBBITCOIN_TRANSACTION_HPP

#include <bitcoin/primitives.hpp>

namespace libbitcoin {

typedef std::vector<uint64_t> output_value_list;

// Used by transaction_indexer
struct BC_API output_info_type
{
    output_point point;
    uint64_t value;
};

typedef std::vector<output_info_type> output_info_list;

BC_API hash_digest hash_transaction(const transaction_type& tx);
// hash_type_code is used by OP_CHECKSIG
BC_API hash_digest hash_transaction(const transaction_type& tx,
    uint32_t hash_type_code);

BC_API hash_digest generate_merkle_root(const transaction_list& transactions);

BC_API std::string pretty(const transaction_type& transaction);

BC_API bool previous_output_is_null(const output_point& previous_output);
BC_API bool is_coinbase(const transaction_type& tx);

BC_API uint64_t total_output_value(const transaction_type& tx);

BC_API bool operator==(
    const output_point& output_a, const output_point& output_b);
BC_API bool operator!=(
    const output_point& output_a, const output_point& output_b);

BC_API bool is_final(const transaction_type& tx,
    size_t block_height, uint32_t block_time);

} // namespace libbitcoin

#endif

