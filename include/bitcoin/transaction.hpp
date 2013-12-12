/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
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

// Used by both transaction_indexer and select_outputs()
struct output_info_type
{
    output_point point;
    uint64_t value;
};

typedef std::vector<output_info_type> output_info_list;

// Result for call to select_outputs()
struct select_outputs_result
{
    output_point_list points;
    uint64_t change;
};

// Algorithm for call to select_outputs()
enum class select_outputs_algorithm
{
    greedy
};

hash_digest hash_transaction(const transaction_type& tx);
// hash_type_code is used by OP_CHECKSIG
hash_digest hash_transaction(const transaction_type& tx,
    uint32_t hash_type_code);

hash_digest generate_merkle_root(const transaction_list& transactions);

std::string pretty(const transaction_type& transaction);

bool previous_output_is_null(const output_point& previous_output);
bool is_coinbase(const transaction_type& tx);

uint64_t total_output_value(const transaction_type& tx);

bool operator==(const output_point& output_a, const output_point& output_b);
bool operator!=(const output_point& output_a, const output_point& output_b);

bool is_final(const transaction_type& tx,
    size_t block_height, uint32_t block_time);

/**
 * Select optimal outputs for a send from unspent outputs list.
 * Returns output list and remaining change to be sent to
 * a change address.
 */
select_outputs_result select_outputs(
    output_info_list unspent, uint64_t min_value,
    select_outputs_algorithm alg=select_outputs_algorithm::greedy);

} // namespace libbitcoin

#endif

