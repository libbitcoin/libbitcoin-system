/*
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
#include <bitcoin/bitcoin/chain/block.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {
namespace chain {

block block::factory_from_data(const data_chunk& data)
{
    block instance;
    instance.from_data(data);
    return instance;
}

block block::factory_from_data(std::istream& stream)
{
    block instance;
    instance.from_data(stream);
    return instance;
}

bool block::is_valid() const
{
    return !transactions.empty() || header.is_valid();
}

void block::reset()
{
    header.reset();
    transactions.clear();
}

bool block::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool block::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    result = header.from_data(stream);

    uint64_t tx_count = 0;

    if (result)
    {
        tx_count = read_variable_uint(stream);
        result = stream;
    }

    for (uint64_t i = 0; (i < tx_count) && result; ++i)
    {
        transactions.emplace_back();
        result = transactions.back().from_data(stream);
    }

    if (!result)
        reset();

    return result;
}

data_chunk block::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    BOOST_ASSERT(data.size() == satoshi_size());
    return data;
}

void block::to_data(std::ostream& stream) const
{
    header.to_data(stream);
    write_variable_uint(stream, transactions.size());

    for (const transaction& tx : transactions)
        tx.to_data(stream);
}

uint64_t block::satoshi_size() const
{
    uint64_t block_size = header.satoshi_size()
        + variable_uint_size(transactions.size());

    for (const transaction& tx : transactions)
        block_size += tx.satoshi_size();

    return block_size;
}

hash_digest build_merkle_tree(hash_list& merkle)
{
    // Stop if hash list is empty.
    if (merkle.empty())
        return null_hash;

    // While there is more than 1 hash in the list, keep looping...
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
            data_chunk concat_data;
            boost::iostreams::stream<byte_sink<data_chunk>> concat_stream(concat_data);
            write_hash(concat_stream, *it);
            write_hash(concat_stream, *(it + 1));
            BITCOIN_ASSERT(concat_data.size() == (2 * hash_size));

            // Hash both of the hashes.
            hash_digest new_root = bitcoin_hash(concat_data);

            // Add this to the new list.
            new_merkle.push_back(new_root);
        }

        // This is the new list.
        merkle = new_merkle;
    }

    // Finally we end up with a single item.
    return merkle[0];
}

hash_digest block::generate_merkle_root(
    const transaction_list& transactions)
{
    // Generate list of transaction hashes.
    hash_list tx_hashes;

    for (transaction tx : transactions)
        tx_hashes.push_back(tx.hash());

    // Build merkle tree.
    return build_merkle_tree(tx_hashes);
}

} // end chain
} // end libbitcoin
