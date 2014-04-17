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
#include <bitcoin/transaction.hpp>

#include <bitcoin/types.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/satoshi_serialize.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/hash.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> hash_list;

hash_digest hash_transaction_impl(const transaction_type& tx,
    uint32_t* hash_type_code)
{
    data_chunk serialized_tx(satoshi_raw_size(tx));
    satoshi_save(tx, serialized_tx.begin());
    if (hash_type_code != nullptr)
        extend_data(serialized_tx, uncast_type(*hash_type_code));
    return generate_hash(serialized_tx);
}

hash_digest hash_transaction(const transaction_type& tx)
{
    return hash_transaction_impl(tx, nullptr);
}
hash_digest hash_transaction(const transaction_type& tx,
    uint32_t hash_type_code)
{
    return hash_transaction_impl(tx, &hash_type_code);
}

hash_digest build_merkle_tree(hash_list& merkle)
{
    if (merkle.empty())
        return null_hash;
    else if (merkle.size() == 1)
        return merkle[0];

    while (merkle.size() > 1)
    {
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        hash_list new_merkle;
        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            data_chunk concat_data(hash_digest_size * 2);
            auto concat = make_serializer(concat_data.begin());
            concat.write_hash(*it);
            concat.write_hash(*(it + 1));
            BITCOIN_ASSERT(
                std::distance(concat_data.begin(), concat.iterator()) ==
                hash_digest_size * 2);
            hash_digest new_root = generate_hash(concat_data);
            new_merkle.push_back(new_root);
        }
        merkle = new_merkle;
    }
    return merkle[0];
}

hash_digest generate_merkle_root(const transaction_list& transactions)
{
    hash_list tx_hashes;
    for (transaction_type tx: transactions)
        tx_hashes.push_back(hash_transaction(tx));
    return build_merkle_tree(tx_hashes);
}

std::string pretty(const transaction_input_type& input)
{
    std::ostringstream ss;
    ss << "\thash = " << input.previous_output.hash << "\n"
        << "\tindex = " << input.previous_output.index << "\n"
        << "\t" << input.script << "\n"
        << "\tsequence = " << input.sequence << "\n";
    return ss.str();
}

std::string pretty(const transaction_output_type& output)
{
    std::ostringstream ss;
    ss << "\tvalue = " << output.value << "\n"
        << "\t" << output.script << "\n";
    return ss.str();
}

std::string pretty(const transaction_type& tx)
{
    std::ostringstream ss;
    ss << "Transaction:\n"
        << "\tversion = " << tx.version << "\n"
        << "\tlocktime = " << tx.locktime << "\n"
        << "Inputs:\n";
    for (transaction_input_type input: tx.inputs)
        ss << pretty(input);
    ss << "Outputs:\n";
    for (transaction_output_type output: tx.outputs)
        ss << pretty(output);
    ss << "\n";
    return ss.str();
}

bool previous_output_is_null(const output_point& previous_output)
{
    return previous_output.index == std::numeric_limits<uint32_t>::max() &&
        previous_output.hash == null_hash;
}

bool is_coinbase(const transaction_type& tx)
{
    return tx.inputs.size() == 1 &&
        previous_output_is_null(tx.inputs[0].previous_output);
}

uint64_t total_output_value(const transaction_type& tx)
{
    uint64_t total = 0;
    for (const transaction_output_type& output: tx.outputs)
        total += output.value;
    return total;
}

bool operator==(const output_point& output_a, const output_point& output_b)
{
    return output_a.hash == output_b.hash && output_a.index == output_b.index;
}
bool operator!=(const output_point& output_a, const output_point& output_b)
{
    return !(output_a == output_b);
}

bool is_final(const transaction_input_type& tx_input)
{
    return tx_input.sequence == std::numeric_limits<uint32_t>::max();
}

bool is_final(const transaction_type& tx,
    size_t block_height, uint32_t block_time)
{
    if (tx.locktime == 0)
        return true;
    uint32_t max_locktime = block_time;
    if (tx.locktime < locktime_threshold)
        max_locktime = block_height;
    if (tx.locktime < max_locktime)
        return true;
    for (const transaction_input_type& tx_input: tx.inputs)
        if (!is_final(tx_input))
            return false;
    return true;
}

} // namespace libbitcoin

