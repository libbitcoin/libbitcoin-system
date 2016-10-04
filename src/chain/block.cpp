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
#include <cfenv>
#include <cmath>
#include <memory>
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
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/math/script_number.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::config;

const size_t block::validation::orphan_height = 0;

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

block block::factory_from_data(reader& source)
{
    block instance;
    instance.from_data(source);
    return instance;
}

chain::block block::genesis_mainnet()
{
    data_chunk raw_block;
    decode_base16(raw_block, encoded_mainnet_genesis_block);
    const auto genesis = chain::block::factory_from_data(raw_block);

    BITCOIN_ASSERT(genesis.is_valid());
    BITCOIN_ASSERT(genesis.transactions().size() == 1);
    BITCOIN_ASSERT(genesis.generate_merkle_root() == genesis.header().merkle());

    return genesis;
}

chain::block block::genesis_testnet()
{
    data_chunk raw_block;
    decode_base16(raw_block, encoded_testnet_genesis_block);
    const auto genesis = chain::block::factory_from_data(raw_block);

    BITCOIN_ASSERT(genesis.is_valid());
    BITCOIN_ASSERT(genesis.transactions().size() == 1);
    BITCOIN_ASSERT(genesis.generate_merkle_root() == genesis.header().merkle());

    return genesis;
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

size_t block::locator_size(size_t top)
{
    // Thread side effect :<
    std::fesetround(FE_UPWARD);

    const auto first_ten_or_top = std::min(size_t(10), top);
    const auto remaining = top - first_ten_or_top;
    const auto back_off = remaining == 0 ? 0.0 : std::log2(remaining);
    const auto rounded_up_log = static_cast<size_t>(std::nearbyint(back_off));
    return first_ten_or_top + rounded_up_log + size_t(1);
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
    BITCOIN_ASSERT(heights.size() <= reservation);
    return heights;
}

hash_number block::difficulty(uint32_t bits)
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

uint64_t block::subsidy(size_t height)
{
    auto subsidy = bitcoin_to_satoshi(initial_block_reward);
    subsidy >>= (height / reward_interval);
    return subsidy;
}

// Hash ordering matters, don't use std::transform here.
inline hash_list to_hashes(const transaction::list& transactions)
{
    hash_list out;
    out.reserve(transactions.size());
    auto hasher = [&out](const transaction& tx) { out.push_back(tx.hash()); };
    std::for_each(transactions.begin(), transactions.end(), hasher);
    return out;
}

block::block()
  : header_(), transactions_()
{
}

block::block(const chain::header& header,
    const transaction::list& transactions)
  : header_(header), transactions_(transactions)
{
}

block::block(chain::header&& header, transaction::list&& transactions)
  : header_(std::move(header)),
    transactions_(std::move(transactions))
{
}

block::block(const block& other)
  : block(other.header_, other.transactions_)
{
}

block::block(block&& other)
  : block(std::move(other.header_), std::move(other.transactions_))
{
}

chain::header& block::header()
{
    return header_;
}

const chain::header& block::header() const
{
    return header_;
}

void block::set_header(const chain::header& value)
{
    header_ = value;
}

void block::set_header(chain::header&& value)
{
    header_ = std::move(value);
}

transaction::list& block::transactions()
{
    return transactions_;
}

const transaction::list& block::transactions() const
{
    return transactions_;
}

void block::set_transactions(const transaction::list& value)
{
    transactions_ = value;
}

void block::set_transactions(transaction::list&& value)
{
    transactions_ = std::move(value);
}

block& block::operator=(block&& other)
{
    header_ = std::move(other.header_);
    transactions_ = std::move(other.transactions_);
    return *this;
}

bool block::operator==(const block& other) const
{
    return (header_ == other.header_)
        && (transactions_ == other.transactions_);
}

bool block::operator!=(const block& other) const
{
    return !(*this == other);
}

bool block::is_valid() const
{
    return !transactions_.empty() || header_.is_valid();
}

void block::reset()
{
    header_.reset();
    transactions_.clear();
    transactions_.shrink_to_fit();
}

bool block::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool block::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool block::from_data(reader& source)
{
    reset();

    if (!header_.from_data(source, true))
        return false;

    transactions_.resize(safe_unsigned<size_t>(header_.transaction_count()));

    // Order is required.
    for (auto& tx: transactions_)
    {
        if (!tx.from_data(source))
        {
            reset();
            return false;
        }
    }

    return true;
}

data_chunk block::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void block::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void block::to_data(writer& sink) const
{
    header_.to_data(sink, false);
    sink.write_variable_uint_little_endian(transactions_.size());
    const auto to = [&sink](const transaction& tx) { tx.to_data(sink); };
    std::for_each(transactions_.begin(), transactions_.end(), to);
}

// TODO: provide optimization option to balance total sigops across buckets.
// Disperse the inputs of the block evenly to the specified number of buckets.
transaction::sets_const_ptr block::to_input_sets(size_t fanout,
    bool with_coinbase_transaction) const
{
    const auto total = total_inputs(with_coinbase_transaction);
    const auto buckets = transaction::reserve_buckets(total, fanout);

    // Guard against division by zero.
    if (!buckets->empty())
    {
        size_t count = 0;
        const auto& txs = transactions_;
        const auto start = with_coinbase_transaction ? 0 : 1;

        // Populate each bucket with full (or full-1) input references.
        for (auto tx = txs.begin() + start; tx != txs.end(); ++tx)
            for (size_t index = 0; index < tx->inputs().size(); ++index)
                (*buckets)[count++ % fanout].push_back({ *tx, index });
    }

    return std::const_pointer_cast<const transaction::sets>(buckets);
}

// Convenience property.
hash_digest block::hash() const
{
    return header_.hash();
}

hash_number block::difficulty() const
{
    return difficulty(header_.bits());
}

// overflow returns max_uint64
uint64_t block::serialized_size() const
{
    auto block_size = header_.serialized_size(true);
    const auto value = [](uint64_t total, const transaction& tx)
    {
        const auto size = tx.serialized_size();
        return safe_add(total, size);
    };

    const auto& txs = transactions_;
    return std::accumulate(txs.begin(), txs.end(), block_size, value);
    return block_size;
}

// Unpopulated chain state returns max_size_t.
size_t block::signature_operations() const
{
    const auto state = validation.state;
    return state ? signature_operations(
        state->is_enabled(rule_fork::bip16_rule)) : max_size_t;
}

size_t block::signature_operations(bool bip16_active) const
{
    const auto value = [bip16_active](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.signature_operations(bip16_active));
    };

    const auto& txs = transactions_;
    return std::accumulate(txs.begin(), txs.end(), size_t(0), value);
}

size_t block::total_inputs(bool with_coinbase_transaction) const
{
    const auto inputs = [](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.inputs().size());
    };

    const auto& txs = transactions_;
    const size_t offset = with_coinbase_transaction ? 0 : 1;
    return std::accumulate(txs.begin() + offset, txs.end(), size_t(0), inputs);
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases also returns true.
bool block::is_extra_coinbases() const
{
    if (transactions_.empty())
        return false;

    const auto value = [](const transaction& tx)
    {
        return tx.is_coinbase();
    };

    const auto& txs = transactions_;
    return std::any_of(txs.begin() + 1, txs.end(), value);
}

bool block::is_final(size_t height) const
{
    const auto timestamp = header_.timestamp();
    const auto value = [height, timestamp](const transaction& tx)
    {
        return tx.is_final(height, timestamp);
    };

    const auto& txs = transactions_;
    return std::all_of(txs.begin(), txs.end(), value);
}

// Distinctness is defined by transaction hash.
bool block::is_distinct_transaction_set() const
{
    const auto hasher = [](const transaction& tx) { return tx.hash(); };
    const auto& txs = transactions_;
    hash_list hashes(txs.size());
    std::transform(txs.begin(), txs.end(), hashes.begin(), hasher);
    std::sort(hashes.begin(), hashes.end());
    const auto distinct_end = std::unique(hashes.begin(), hashes.end());
    return distinct_end == hashes.end();
}

hash_digest block::generate_merkle_root() const
{
    auto merkle = to_hashes(transactions_);

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
    return (generate_merkle_root() == header_.merkle());
}

// Overflow returns max_uint64.
uint64_t block::fees() const
{
    ////static_assert(max_money() < max_uint64, "overflow sentinel invalid");
    const auto value = [](uint64_t total, const transaction& tx)
    {
        return ceiling_add(total, tx.fees());
    };

    const auto& txs = transactions_;
    return std::accumulate(txs.begin(), txs.end(), uint64_t(0), value);
}

uint64_t block::claim() const
{
    return transactions_.empty() ? 0 :
        transactions_.front().total_output_value();
}

// Overflow returns max_uint64.
uint64_t block::reward(size_t height) const
{
    ////static_assert(max_money() < max_uint64, "overflow sentinel invalid");
    return ceiling_add(fees(), subsidy(height));
}

bool block::is_valid_coinbase_claim(size_t height) const
{
    return claim() <= reward(height);
}

bool block::is_valid_coinbase_script(size_t height) const
{
    if (transactions_.empty() || transactions_.front().inputs().empty())
        return false;

    // Get the serialized coinbase input script as a byte vector.
    const auto& actual_tx = transactions_.front();
    const auto& actual_script = actual_tx.inputs().front().script();
    const auto actual = actual_script.to_data(false);

    // Create the expected script as a byte vector.
    script expected_script;
    script_number number(height);
    expected_script.operations().push_back({ opcode::special, number.data() });
    const auto expected = expected_script.to_data(false);

    // Require that the coinbase script match the expected coinbase script.
    return std::equal(expected.begin(), expected.end(), actual.begin());
}

code block::check_transactions() const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.check(false)))
            return ec;

    return error::success;
}

code block::accept_transactions(const chain_state& state) const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.accept(state, false)))
            return ec;

    return error::success;
}

code block::connect_transactions(const chain_state& state) const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.connect(state)))
            return ec;

    return error::success;
}

// These checks are self-contained; blockchain (and so version) independent.
code block::check() const
{
    code ec;

    if ((ec = header_.check()))
        return ec;

    else if (serialized_size() > max_block_size)
        return error::size_limits;

    else if (transactions_.empty())
        return error::empty_block;

    else if (!transactions_.front().is_coinbase())
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

    else if (!is_valid_merkle_root())
        return error::merkle_mismatch;

    else
        return check_transactions();
}

code block::accept() const
{
    const auto state = validation.state;
    return state ? accept(*state) : error::operation_failed;
}

// Deprecated (?)
// TODO: implement sigops and total input/output value caching.
// These checks assume that prevout caching is completed on all tx.inputs.
// Flags should be based on connecting at the specified blockchain height.
code block::accept(const chain_state& state) const
{
    code ec;
    const auto bip16 = state.is_enabled(rule_fork::bip16_rule);
    const auto bip34 = state.is_enabled(rule_fork::bip34_rule);

    if ((ec = header_.accept(state)))
        return ec;

    // This recurses txs but is not applied to mempool (timestamp required).
    else if (!is_final(state.height()))
        return error::non_final_transaction;

    else if (bip34 && !is_valid_coinbase_script(state.height()))
        return error::coinbase_height_mismatch;

    // This recomputes sigops to include p2sh from prevouts.
    else if (signature_operations(bip16) > max_block_sigops)
        return error::too_many_sigs;

    else if (!is_valid_coinbase_claim(state.height()))
        return error::coinbase_too_large;

    else
        return accept_transactions(state);
}

code block::connect() const
{
    const auto state = validation.state;
    return state ? connect(*state) : error::operation_failed;
}

// Deprecated (?)
code block::connect(const chain_state& state) const
{
    return connect_transactions(state);
}

} // namespace chain
} // namespace libbitcoin
