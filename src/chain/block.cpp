/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/block.hpp>

#include <algorithm>
#include <numeric>
#include <utility>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

block block::factory_from_data(const data_chunk& data,
    bool with_transaction_count)
{
    block instance;
    instance.from_data(data, with_transaction_count);
    return instance;
}

block block::factory_from_data(std::istream& stream,
    bool with_transaction_count)
{
    block instance;
    instance.from_data(stream, with_transaction_count);
    return instance;
}

block block::factory_from_data(reader& source,
    bool with_transaction_count)
{
    block instance;
    instance.from_data(source, with_transaction_count);
    return instance;
}

block::block()
  : sigops_(0)
{
}

block::block(const block& other)
  : block(other.header, other.transactions)
{
}

block::block(const chain::header& header,
    const chain::transaction::list& transactions)
  : header(header),
    transactions(transactions),
    sigops_(0)
{
}

block::block(block&& other)
  : block(std::forward<chain::header>(other.header),
        std::forward<chain::transaction::list>(other.transactions))
{
}

block::block(chain::header&& header, chain::transaction::list&& transactions)
  : header(std::forward<chain::header>(header)),
    transactions(std::forward<chain::transaction::list>(transactions)),
    sigops_(0)
{
}

block& block::operator=(block&& other)
{
    header = std::move(other.header);
    transactions = std::move(other.transactions);
    sigops_ = other.sigops_;
    return *this;
}

bool block::is_valid() const
{
    return !transactions.empty() || header.is_valid();
}

void block::reset()
{
    header.reset();
    transactions.clear();
    transactions.shrink_to_fit();
    sigops_ = 0;
}

bool block::from_data(const data_chunk& data, bool with_transaction_count)
{
    data_source istream(data);
    return from_data(istream, with_transaction_count);
}

bool block::from_data(std::istream& stream, bool with_transaction_count)
{
    istream_reader source(stream);
    return from_data(source, with_transaction_count);
}

bool block::from_data(reader& source, bool with_transaction_count)
{
    reset();

    auto result = header.from_data(source, with_transaction_count);

    if (result)
    {
        transactions.resize(header.transaction_count);

        for (auto& tx: transactions)
        {
            result = tx.from_data(source);

            if (!result)
                break;
        }
    }

    if (!result)
        reset();

    return result;
}

data_chunk block::to_data(bool with_transaction_count) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream, with_transaction_count);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(with_transaction_count));
    return data;
}

void block::to_data(std::ostream& stream, bool with_transaction_count) const
{
    ostream_writer sink(stream);
    to_data(sink, with_transaction_count);
}

void block::to_data(writer& sink, bool with_transaction_count) const
{
    header.to_data(sink, with_transaction_count);

    for (const auto& tx: transactions)
        tx.to_data(sink);
}

// overflow returns max_uint64
uint64_t block::serialized_size(bool with_transaction_count) const
{
    auto block_size = header.serialized_size(with_transaction_count);
    const auto value = [](uint64_t total, const transaction& tx)
    {
        const auto size = tx.serialized_size();
        return total >= max_uint64 - size ? max_uint64 : total + size;
    };

    const auto& txs = transactions;
    return std::accumulate(txs.begin(), txs.end(), block_size, value);
    return block_size;
}

// overflow returns max_size_t
// If the actual value is zero there is no cache benefit, which is ok.
size_t block::signature_operations() const
{
    if (sigops_ != 0)
        return sigops_;

    const auto value = [](size_t total, const transaction& tx)
    {
        const auto count = tx.signature_operations();
        return total >= max_size_t - count ? max_size_t : total + count;
    };

    const auto& txs = transactions;
    return std::accumulate(txs.begin(), txs.end(), sigops_, value);
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases also returns true.
bool block::has_extra_coinbases() const
{
    const auto& txs = transactions;

    if (transactions.empty())
        return false;

    const auto value = [](const transaction& tx)
    {
        return tx.is_coinbase();
    };

    return std::none_of(txs.begin() + 1, txs.end(), value);
}

bool block::is_final(size_t height) const
{
    const auto timestamp = header.timestamp;
    const auto value = [height, timestamp](const transaction& tx)
    {
        return tx.is_final(height, timestamp);
    };

    const auto& txs = transactions;
    return std::all_of(txs.begin(), txs.end(), value);
}

// Distinctness is defined by transaction hash.
bool block::is_distinct_transaction_set() const
{
    const auto hasher = [](const transaction& transaction)
    {
        return transaction.hash();
    };

    const auto& txs = transactions;
    hash_list hashes(txs.size());
    std::transform(txs.begin(), txs.end(), hashes.begin(), hasher);
    std::sort(hashes.begin(), hashes.end());
    const auto distinct_end = std::unique(hashes.begin(), hashes.end());
    return distinct_end == hashes.end();
}

bool block::is_valid_coinbase_height(size_t height) const
{
    // There must be a transaction with an input.
    if (transactions.empty() || transactions.front().inputs.empty())
        return false;

    // Get the serialized coinbase input script as a byte vector.
    const auto& actual_tx = transactions.front();
    const auto& actual_script = actual_tx.inputs.front().script;
    const auto actual = actual_script.to_data(false);

    // Create the expected script as a byte vector.
    script expected_script;
    script_number number(height);
    expected_script.operations.push_back({ opcode::special, number.data() });
    const auto expected = expected_script.to_data(false);

    // Require that the coinbase script match the expected coinbase script.
    return std::equal(expected.begin(), expected.end(), actual.begin());
}

bool block::is_valid_merkle_root() const
{
    return generate_merkle_root() == header.merkle;
}

hash_digest block::build_merkle_tree(hash_list& merkle)
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
            data_sink concat_stream(concat_data);
            ostream_writer concat_sink(concat_stream);
            concat_sink.write_hash(*it);
            concat_sink.write_hash(*(it + 1));
            concat_stream.flush();

            BITCOIN_ASSERT(concat_data.size() == (2 * hash_size));

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

// TODO: consider caching result.
hash_digest block::generate_merkle_root() const
{
    const auto hasher = [](const transaction& transaction)
    {
        return transaction.hash();
    };

    const auto& txs = transactions;
    hash_list hashes(txs.size());
    std::transform(txs.begin(), txs.end(), hashes.begin(), hasher);
    return build_merkle_tree(hashes);
}

static const std::string encoded_mainnet_genesis_block =
    "01000000"
    "0000000000000000000000000000000000000000000000000000000000000000"
    "3ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"
    "29ab5f49"
    "ffff001d"
    "1dac2b7c"
    "01"
    "01000000"
    "01"
    "0000000000000000000000000000000000000000000000000000000000000000ffffffff"
    "4d"
    "04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73"
    "ffffffff"
    "01"
    "00f2052a01000000"
    "43"
    "4104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac"
    "00000000";

static const std::string encoded_testnet_genesis_block =
    "01000000"
    "0000000000000000000000000000000000000000000000000000000000000000"
    "3ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa4b1e5e4a"
    "dae5494d"
    "ffff001d"
    "1aa4ae18"
    "01"
    "01000000"
    "01"
    "0000000000000000000000000000000000000000000000000000000000000000ffffffff"
    "4d"
    "04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73"
    "ffffffff"
    "01"
    "00f2052a01000000"
    "43"
    "4104678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac"
    "00000000";

chain::block block::genesis_mainnet()
{
    data_chunk raw_block;
    decode_base16(raw_block, encoded_mainnet_genesis_block);
    const auto genesis = chain::block::factory_from_data(raw_block);

    BITCOIN_ASSERT(genesis.is_valid());
    BITCOIN_ASSERT(genesis.transactions.size() == 1);
    BITCOIN_ASSERT(genesis.generate_merkle_root() == genesis.header.merkle);

    return genesis;
}

chain::block block::genesis_testnet()
{
    data_chunk raw_block;
    decode_base16(raw_block, encoded_testnet_genesis_block);
    const auto genesis = chain::block::factory_from_data(raw_block);

    BITCOIN_ASSERT(genesis.is_valid());
    BITCOIN_ASSERT(genesis.transactions.size() == 1);
    BITCOIN_ASSERT(genesis.generate_merkle_root() == genesis.header.merkle);

    return genesis;
}

} // namespace chain
} // namespace libbitcoin
