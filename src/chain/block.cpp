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
#include <cstddef>
#include <limits>
#include <cmath>
#include <numeric>
#include <type_traits>
#include <utility>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/hash_number.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::config;

// TODO: centralize without exposing numeric_limits to headers :/.
template <typename Integer,
    typename = std::enable_if<std::is_unsigned<Integer>::value>>
Integer ceiling_add(Integer left, Integer right)
{
    static const auto ceiling = std::numeric_limits<Integer>::max();
    return left > ceiling - right ? ceiling : left + right;
}

template <typename Integer,
    typename = std::enable_if<std::is_unsigned<Integer>::value>>
Integer floor_subtract(Integer left, Integer right)
{
    return right >= left ? 0 : left - right;
}

const size_t block::metadata::orphan_height = 0;

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

// Hash ordering matters.
inline hash_list to_hashes(const transaction::list& transactions)
{
    const auto map = [](const transaction& tx)
    {
        return tx.hash();
    };

    hash_list out(transactions.size());
    std::transform(transactions.begin(), transactions.end(), out.begin(), map);
    return out;
}

inline size_t locator_size(size_t top)
{
    const auto first_ten = std::min(size_t(10), top);
    const auto back_off = floor_subtract(top, size_t(10));
    return first_ten + static_cast<size_t>(std::log2(back_off)) + size_t(1);
}

// This algorithm is a network best practice, not a consensus rule.
block::indexes block::locator_heights(size_t top)
{
    size_t step = 1;
    block::indexes heights;
    const auto reservation = locator_size(top);
    heights.reserve(reservation);

    // Start at the top of the chain and work backwards to zero.
    for (auto height = top; height > 0; height = floor_subtract(height, step))
    {
        // Push top 10 indexes first, then back off exponentially.
        if (heights.size() >= 10)
            step <<= 1;

        heights.push_back(height);
    }

    // Push the genesis block index.
    heights.push_back(0);

    // Validate the reservation computation.
    BITCOIN_ASSERT(heights.size() == reservation);
    return heights;
}

block::block()
{
}

block::block(const block& other)
  : block(other.header, other.transactions)
{
}

block::block(const chain::header& header,
    const chain::transaction::list& transactions)
  : header(header),
    transactions(transactions)
{
}

block::block(block&& other)
  : block(std::forward<chain::header>(other.header),
        std::forward<chain::transaction::list>(other.transactions))
{
}

block::block(chain::header&& header, chain::transaction::list&& transactions)
  : header(std::forward<chain::header>(header)),
    transactions(std::forward<chain::transaction::list>(transactions))
{
}

block& block::operator=(block&& other)
{
    header = std::move(other.header);
    transactions = std::move(other.transactions);
    return *this;
}

block& block::operator=(const block& other)
{
    header = other.header;
    transactions = other.transactions;
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

// Convenience property.
hash_digest block::hash() const
{
    return header.hash();
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
size_t block::signature_operations(bool bip16_active) const
{
    const auto value = [bip16_active](size_t total, const transaction& tx)
    {
        return ceiling_add(total, tx.signature_operations(bip16_active));
    };

    const auto& txs = transactions;
    return std::accumulate(txs.begin(), txs.end(), size_t(0), value);
}

size_t block::total_inputs() const
{
    const auto inputs = [](size_t total, const transaction& tx)
    {
        return ceiling_add(total, tx.inputs.size());
    };

    const auto& txs = transactions;
    return std::accumulate(txs.begin(), txs.end(), size_t(0), inputs);
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases also returns true.
bool block::is_extra_coinbases() const
{
    if (transactions.empty())
        return false;

    const auto value = [](const transaction& tx)
    {
        return tx.is_coinbase();
    };

    const auto& txs = transactions;
    return std::any_of(txs.begin() + 1, txs.end(), value);
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

hash_digest block::generate_merkle_root() const
{
    auto merkle = to_hashes(transactions);

    if (merkle.empty())
        return null_hash;

    hash_list update;

    // Initial capacity is half of the original list (clear doesn't reset).
    update.reserve((merkle.size() + 1) / 2);

    while (merkle.size() > 1)
    {
        // If number of hashes is odd, duplicate last hash in the list.
        if (merkle.size() % 2 != 0)
            merkle.push_back(merkle.back());

        for (auto it = merkle.begin(); it != merkle.end(); it += 2)
            update.push_back(bitcoin_hash(build_chunk({ it[0], it[1] })));

        std::swap(merkle, update);
        update.clear();
    }

    // There is now only one item in the list.
    return merkle.front();
}

bool block::is_valid_merkle_root() const
{
    return generate_merkle_root() == header.merkle;
}

// static
hash_number block::work(uint32_t bits)
{
    hash_number target;

    if (!target.set_compact(bits) || target == 0)
        return 0;

    // We need to compute 2**256 / (target+1), but we can't represent 2**256
    // as it's too large for uint256. However as 2**256 is at least as large as
    // target+1, it is equal to ((2**256 - target - 1) / (target+1)) + 1, or 
    // ~target / (target+1) + 1.
    return (~target / (target + 1)) + 1;
}

// static
uint64_t block::subsidy(size_t height)
{
    auto subsidy = bitcoin_to_satoshi(initial_block_reward);
    subsidy >>= (height / reward_interval);
    return subsidy;
}

uint64_t block::fees() const
{
    const auto value = [](uint64_t total, const transaction& tx)
    {
        return ceiling_add(total, tx.fees());
    };

    const auto& txs = transactions;
    return std::accumulate(txs.begin(), txs.end(), uint64_t(0), value);
}

uint64_t block::claim() const
{
    return transactions.empty() ? 0 :
        transactions.front().total_output_value();
}

uint64_t block::reward(size_t height) const
{
    return ceiling_add(fees(), subsidy(height));
}

bool block::is_valid_coinbase_claim(size_t height) const
{
    return claim() <= reward(height);
}

bool block::is_valid_coinbase_script(size_t height) const
{
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

code block::check_transactions() const
{
    code ec;

    for (const auto& tx: transactions)
        if ((ec = tx.check(false)))
            return ec;

    return error::success;
}

code block::accept_transactions(const chain_state& state) const
{
    code ec;

    for (const auto& tx: transactions)
        if ((ec = tx.accept(state, false)))
            return ec;

    return error::success;
}

code block::connect_transactions(const chain_state& state) const
{
    code ec;

    for (const auto& tx: transactions)
        if ((ec = tx.connect(state)))
            return ec;

    return error::success;
}

// These checks are self-contained; blockchain (and so version) independent.
code block::check() const
{
    if (serialized_size() > max_block_size)
        return error::size_limits;

    else if (!header.is_valid_proof_of_work())
        return error::proof_of_work;

    else if (!header.is_valid_time_stamp())
        return error::futuristic_timestamp;

    else if (transactions.empty())
        return error::empty_block;

    else if (!transactions.front().is_coinbase())
        return error::first_not_coinbase;

    else if (is_extra_coinbases())
        return error::extra_coinbases;

    else if (!is_distinct_transaction_set())
        return error::duplicate;

    // We cannot know if bip16 is enabled at this point so we disable it.
    // This will not make a difference unless prevouts are populated, in which
    // case they are ignored. This means that p2sh sigops are not counted here.
    // This is a preliminary check, the final count must come from connect().
    else if (signature_operations(false) > max_block_sigops)
        return error::too_many_sigs;

    else if (is_valid_merkle_root())
        return error::merkle_mismatch;

    else
        return check_transactions();
}

// TODO: implement sigops and total input/output value caching.
// These checks assume that prevout caching is completed on all tx.inputs.
// Flags should be based on connecting at the specified blockchain height.
code block::accept(const chain_state& state) const
{
    const auto bip16 = state.is_enabled(rule_fork::bip16_rule);
    const auto bip34 = state.is_enabled(rule_fork::bip34_rule);

    if (!state.is_checkpoint_failure(header))
        return error::checkpoints_failed;

    else if (header.version < state.minimum_version())
        return error::old_version_block;

    else if (header.bits != state.work_required())
        return error::incorrect_proof_of_work;

    else if (header.timestamp <= state.median_time_past())
        return error::timestamp_too_early;

    // This recurses txs but is not applied to mempool (timestamp required).
    else if (!is_final(state.next_height()))
        return error::non_final_transaction;

    else if (bip34 && !is_valid_coinbase_script(state.next_height()))
        return error::coinbase_height_mismatch;

    // This recomputes sigops to include p2sh from prevouts.
    else if (signature_operations(bip16) > max_block_sigops)
        return error::too_many_sigs;

    else if (!is_valid_coinbase_claim(state.next_height()))
        return error::coinbase_too_large;

    else
        return accept_transactions(state);
}

code block::connect(const chain_state& state) const
{
    return connect_transactions(state);
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
