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
#include <bitcoin/bitcoin/transaction.hpp>

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/satoshi_serialize.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

hash_digest hash_transaction_impl(const transaction_type& tx,
    uint32_t* hash_type_code)
{
    data_chunk serialized_tx(satoshi_raw_size(tx));
    satoshi_save(tx, serialized_tx.begin());
    if (hash_type_code != nullptr)
        extend_data(serialized_tx, to_little_endian(*hash_type_code));
    return bitcoin_hash(serialized_tx);
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
    // Stop if hash list is empty.
    if (merkle.empty())
        return null_hash;

    if (merkle.size() == 1)
        return merkle[0];

    // While there is more than 1 hash in the list, keep looping.
    while (merkle.size() > 1)
    {
        // If number of hashes is odd, duplicate last hash in the list.
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        // List size is now even.
        BITCOIN_ASSERT(merkle.size() % 2 == 0);

        // New hash list.
        hash_list new_merkle;

        // Loop through hashes 2 at a time.
        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
        {
            // Join both current hashes together (concatenate).
            data_chunk concat_data(hash_size * 2);
            auto concat = make_serializer(concat_data.begin());
            concat.write_hash(*it);
            concat.write_hash(*(it + 1));
            BITCOIN_ASSERT(concat.iterator() == concat_data.end());

            // Hash both of the hashes.
            const auto new_root = bitcoin_hash(concat_data);

            // Add this to the new list.
            new_merkle.push_back(new_root);
        }

        // This is the new list.
        merkle = new_merkle;
    }

    // Finally we end up with a single item.
    return merkle[0];
}

hash_digest generate_merkle_root(const transaction_list& transactions)
{
    // Generate list of transaction hashes.
    hash_list tx_hashes;
    for (const auto& tx: transactions)
        tx_hashes.push_back(hash_transaction(tx));

    // Build merkle tree.
    return build_merkle_tree(tx_hashes);
}

std::string pretty(const transaction_input_type& input)
{
    std::ostringstream ss;
    ss << "\thash = " << encode_hash(input.previous_output.hash) << "\n"
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
    for (const auto& input: tx.inputs)
        ss << pretty(input);

    ss << "Outputs:\n";
    for (const auto& output: tx.outputs)
        ss << pretty(output);

    ss << "\n";
    return ss.str();
}

bool previous_output_is_null(const output_point& previous_output)
{
    return previous_output.index == max_input_sequence &&
        previous_output.hash == null_hash;
}

bool is_coinbase(const transaction_type& tx)
{
    return tx.inputs.size() == 1 &&
        previous_output_is_null(tx.inputs[0].previous_output);
}

bool is_final(const transaction_input_type& tx_input)
{
    return tx_input.sequence == max_sequence;
}

bool is_final(const transaction_type& tx,
    size_t block_height, uint32_t block_time)
{
    if (tx.locktime == 0)
        return true;

    auto max_locktime = block_time;
    if (tx.locktime < locktime_threshold)
        max_locktime = static_cast<uint32_t>(block_height);

    if (tx.locktime < max_locktime)
        return true;

    for (const auto& tx_input: tx.inputs)
        if (!is_final(tx_input))
            return false;

    return true;
}

bool is_locktime_conflict(const transaction_type& tx)
{
    auto locktime_set = tx.locktime != 0;
    if (locktime_set)
        for (const auto& input: tx.inputs)
            if (input.sequence < max_sequence)
                return false;

    return locktime_set;
}

uint64_t total_output_value(const transaction_type& tx)
{
    uint64_t total = 0;
    for (const auto& output: tx.outputs)
        total += output.value;

    return total;
}

select_outputs_result select_outputs(output_info_list unspent,
    uint64_t min_value, select_outputs_algorithm DEBUG_ONLY(algorithm))
{
    // Just one default implementation for now.
    // Consider a switch case with greedy_select_outputs(min_value) .etc
    // if this is ever extended with more algorithms.
    BITCOIN_ASSERT(algorithm == select_outputs_algorithm::greedy);

    // Fail if empty.
    if (unspent.empty())
        return select_outputs_result();

    const auto lesser = [min_value](const output_info_type& info)
    {
        return info.value < min_value;
    };

    const auto less_than = [](const output_info_type& a, const output_info_type& b)
    {
        return a.value < b.value;
    };

    const auto more_than = [](const output_info_type& a, const output_info_type& b)
    {
        return a.value > b.value;
    };

    auto lesser_begin = unspent.begin();
    auto lesser_end = std::partition(unspent.begin(), unspent.end(), lesser);
    auto greater_begin = lesser_end;
    auto greater_end = unspent.end();
    auto min_greater = std::min_element(greater_begin, greater_end, less_than);

    select_outputs_result result;
    if (min_greater != greater_end)
    {
        result.change = min_greater->value - min_value;
        result.points.push_back(min_greater->point);
        return result;
    }

    // Not found in greaters. Try several lessers instead.
    // Rearrange them from biggest to smallest. We want to use the least
    // amount of inputs as possible.
    std::sort(lesser_begin, lesser_end, more_than);

    uint64_t accum = 0;
    for (auto it = lesser_begin; it != lesser_end; ++it)
    {
        result.points.push_back(it->point);
        accum += it->value;
        if (accum >= min_value)
        {
            result.change = accum - min_value;
            return result;
        }
    }

    return select_outputs_result();
}

bool operator==(const output_point& output_a, const output_point& output_b)
{
    return output_a.hash == output_b.hash && output_a.index == output_b.index;
}
bool operator!=(const output_point& output_a, const output_point& output_b)
{
    return !(output_a == output_b);
}

} // namespace libbitcoin

