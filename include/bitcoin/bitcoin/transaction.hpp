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

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>

namespace libbitcoin {

enum class select_outputs_algorithm
{
    greedy
};

struct BC_API select_outputs_result
{
    output_point_list points;
    uint64_t change;
};

BC_API hash_digest hash_transaction(const transaction_type& tx);
// hash_type_code is used by OP_CHECKSIG
BC_API hash_digest hash_transaction(const transaction_type& tx,
    uint32_t hash_type_code);

BC_API hash_digest generate_merkle_root(const transaction_list& transactions);

BC_API std::string pretty(const transaction_type& transaction);
// BC_API transaction_type unpretty(const std::string& pretty);

// TODO: rename to is_previous_output_null (API consistency)
BC_API bool previous_output_is_null(const output_point& previous_output);
BC_API bool is_coinbase(const transaction_type& tx);
BC_API bool is_final(const transaction_type& tx, size_t block_height, 
    uint32_t block_time);
BC_API bool is_locktime_conflict(const transaction_type& tx);

BC_API uint64_t total_output_value(const transaction_type& tx);

/**
 * Select optimal outputs for a send from unspent outputs list.
 * Returns output list and remaining change to be sent to
 * a change address.
 */
BC_API select_outputs_result select_outputs(
    output_info_list unspent, uint64_t min_value,
    select_outputs_algorithm algorithm=select_outputs_algorithm::greedy);

BC_API bool operator==(
    const output_point& output_a, const output_point& output_b);
BC_API bool operator!=(
    const output_point& output_a, const output_point& output_b);

} // namespace libbitcoin

#endif

