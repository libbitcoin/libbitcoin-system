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

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace chain {

block::block()
{
}

block::block(const block_header& header, const transaction_list& transactions)
    : header_(header), transactions_(transactions)
{
}

block::block(std::istream& stream)
    : header_(stream)
{
    uint64_t tx_count = read_variable_uint(stream);

    for (size_t i = 0; (i < tx_count) && !stream.fail(); ++i)
    {
        transaction tx(stream);
        transactions_.push_back(std::move(tx));
    }

    if (stream.fail())
        throw std::ios_base::failure("block");
}

//block::block(const data_chunk& value)
//    : block(value.begin(), value.end())
//{
//}

block_header& block::header()
{
    return header_;
}

const block_header& block::header() const
{
    return header_;
}

void block::header(const block_header& header)
{
    header_ = header;
}

transaction_list& block::transactions()
{
    return transactions_;
}

const transaction_list& block::transactions() const
{
    return transactions_;
}

block::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    data_chunk raw_header = header_;
    serial.write_data(raw_header);

    serial.write_variable_uint(transactions_.size());

    for (const transaction& tx : transactions_)
    {
        data_chunk raw_tx = tx;
        serial.write_data(raw_tx);
    }

    return result;
}

size_t block::satoshi_size() const
{
    size_t block_size = header_.satoshi_size()
        + variable_uint_size(transactions_.size());

    for (const transaction& tx : transactions_)
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
            data_chunk concat_data(hash_size * 2);
            auto concat = make_serializer(concat_data.begin());
            concat.write_hash(*it);
            concat.write_hash(*(it + 1));
            BITCOIN_ASSERT(concat.iterator() == concat_data.end());

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
