/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/chain/block.hpp>

#include <algorithm>
#include <cstddef>
#include <cfenv>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/input_point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/concurrency/asio.hpp>
#include <bitcoin/system/config/checkpoint.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/collection.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/iostream/iostream.hpp>
#include <bitcoin/system/machine/opcode.hpp>
#include <bitcoin/system/machine/rule_fork.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/message/messages.hpp>
#include <bitcoin/system/settings.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace bc::system::config;
using namespace bc::system::machine;
using namespace boost::adaptors;

// Constructors.
//-----------------------------------------------------------------------------

block::block()
  : metadata{}
{
}

block::block(const block& other)
  : metadata(other.metadata),
    header_(other.header_),
    transactions_(other.transactions_),
    total_inputs_(other.total_inputs_cache()),
    non_coinbase_inputs_(other.non_coinbase_inputs_cache())
{
}

block::block(block&& other)
  : metadata(other.metadata),
    header_(std::move(other.header_)),
    transactions_(std::move(other.transactions_)),
    total_inputs_(other.total_inputs_cache()),
    non_coinbase_inputs_(other.non_coinbase_inputs_cache())
{
}

block::block(const chain::header& header,
    const transaction::list& transactions)
  : metadata{},
    header_(header),
    transactions_(transactions)
{
}

block::block(chain::header&& header, transaction::list&& transactions)
  : metadata{},
    header_(std::move(header)),
    transactions_(std::move(transactions))
{
}

block::optional_size block::total_inputs_cache() const
{
    shared_lock lock(mutex_);
    return total_inputs_;
}

block::optional_size block::non_coinbase_inputs_cache() const
{
    shared_lock lock(mutex_);
    return non_coinbase_inputs_;
}

// Operators.
//-----------------------------------------------------------------------------

block& block::operator=(block&& other)
{
    total_inputs_ = other.total_inputs_cache();
    non_coinbase_inputs_ = other.non_coinbase_inputs_cache();
    header_ = std::move(other.header_);
    transactions_ = std::move(other.transactions_);
    metadata = std::move(other.metadata);
    return *this;
}

bool block::operator==(const block& other) const
{
    return (header_ == other.header_) && (transactions_ == other.transactions_);
}

bool block::operator!=(const block& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
block block::factory(const data_chunk& data, bool witness)
{
    block instance;
    instance.from_data(data, witness);
    return instance;
}

// static
block block::factory(std::istream& stream, bool witness)
{
    block instance;
    instance.from_data(stream, witness);
    return instance;
}

// static
block block::factory(reader& source, bool witness)
{
    block instance;
    instance.from_data(source, witness);
    return instance;
}

bool block::from_data(const data_chunk& data, bool witness)
{
    data_source istream(data);
    return from_data(istream, witness);
}

bool block::from_data(std::istream& stream, bool witness)
{
    istream_reader source(stream);
    return from_data(source, witness);
}

// Full block deserialization is always canonical encoding.
bool block::from_data(reader& source, bool witness)
{
    const auto start = asio::steady_clock::now();

    reset();

    if (!header_.from_data(source, true))
        return false;

    const auto count = source.read_size_little_endian();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        transactions_.resize(count);

    // Order is required, explicit loop allows early termination.
    for (auto& tx: transactions_)
        if (!tx.from_data(source, true, witness))
            break;

    // TODO: optimize by having reader skip witness data.
    if (!witness)
        strip_witness();

    if (!source)
        reset();

    metadata.deserialize = asio::steady_clock::now() - start;
    return source;
}

// private
void block::reset()
{
    header_.reset();
    transactions_.clear();
    transactions_.shrink_to_fit();
}

bool block::is_valid() const
{
    return !transactions_.empty() || header_.is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk block::to_data(bool witness) const
{
    data_chunk data;
    const auto size = serialized_size(witness);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, witness);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void block::to_data(std::ostream& stream, bool witness) const
{
    ostream_writer sink(stream);
    to_data(sink, witness);
}

// Full block serialization is always canonical encoding.
void block::to_data(writer& sink, bool witness) const
{
    header_.to_data(sink, true);
    sink.write_variable_little_endian(transactions_.size());
    const auto to = [&sink, witness](const transaction& tx)
    {
        tx.to_data(sink, true, witness);
    };

    std::for_each(transactions_.begin(), transactions_.end(), to);
}

hash_list block::to_hashes(bool witness) const
{
    hash_list out;
    out.reserve(transactions_.size());
    const auto to_hash = [&out, witness](const transaction& tx)
    {
        out.push_back(tx.hash(witness));
    };

    // Hash ordering matters, don't use std::transform here.
    std::for_each(transactions_.begin(), transactions_.end(), to_hash);
    return out;
}

// Properties (size, accessors, cache).
//-----------------------------------------------------------------------------

// Full block serialization is always canonical encoding.
size_t block::serialized_size(bool witness) const
{
    size_t value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (witness && total_size_ != boost::none)
    {
        value = total_size_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    if (!witness && base_size_ != boost::none)
    {
        value = base_size_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    const auto sum = [witness](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.serialized_size(true, witness));
    };

    const auto& txs = transactions_;
    value = header_.serialized_size(true) +
        message::variable_uint_size(transactions_.size()) +
        std::accumulate(txs.begin(), txs.end(), zero, sum);

    if (witness)
        total_size_ = value;
    else
        base_size_ = value;

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
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

const transaction::list& block::transactions() const
{
    return transactions_;
}

void block::set_transactions(const transaction::list& value)
{
    transactions_ = value;
    segregated_ = boost::none;
    total_inputs_ = boost::none;
    non_coinbase_inputs_ = boost::none;
    base_size_ = boost::none;
    total_size_ = boost::none;
}

void block::set_transactions(transaction::list&& value)
{
    transactions_ = std::move(value);
    segregated_ = boost::none;
    total_inputs_ = boost::none;
    non_coinbase_inputs_ = boost::none;
    base_size_ = boost::none;
    total_size_ = boost::none;
}

// Convenience property.
hash_digest block::hash() const
{
    return header_.hash();
}

// Utilities.
//-----------------------------------------------------------------------------

// This predicts the size of locator_heights output.
size_t block::locator_size(size_t top)
{
    size_t size = 0, step = 1;
    for (auto height = top; height > 0; height = floor_subtract(height, step))
        if (++size > 9u)
            step <<= 1;

    return ++size;
}

// This algorithm is a network best practice, not a consensus rule.
block::indexes block::locator_heights(size_t top)
{
    size_t step = 1;
    indexes heights;
    heights.reserve(locator_size(top));

    // Start at top block and collect block indexes in reverse.
    for (auto height = top; height > 0; height = floor_subtract(height, step))
    {
        heights.push_back(height);

        // Push top 10 indexes then back off exponentially.
        if (heights.size() > 9u)
            step <<= 1;
    }

    // Push the genesis block index.
    heights.push_back(0);
    return heights;
}

// Utilities.
//-----------------------------------------------------------------------------

// Clear witness from all inputs (does not change default transaction hash).
void block::strip_witness()
{
    const auto strip = [](transaction& transaction)
    {
        transaction.strip_witness();
    };

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(mutex_);

    segregated_ = false;
    total_size_ = boost::none;
    std::for_each(transactions_.begin(), transactions_.end(), strip);
    ///////////////////////////////////////////////////////////////////////////
}

// Validation helpers.
//-----------------------------------------------------------------------------

// static
uint64_t block::subsidy(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42)
{
    static const auto overflow = to_bits(sizeof(uint64_t));
    auto subsidy = initial_block_subsidy_satoshi;
    const auto halvings = height / subsidy_interval;
    subsidy >>= (bip42 && halvings >= overflow ? 0 : halvings);
    return subsidy;
}

// Returns max_size_t in case of overflow or unpopulated chain state.
size_t block::signature_operations() const
{
    const auto state = header_.metadata.state;
    const auto bip16 = state->is_enabled(bip16_rule);
    const auto bip141 = state->is_enabled(bip141_rule);
    return state ? signature_operations(bip16, bip141) : max_size_t;
}

// Overflow returns max_size_t.
size_t block::signature_operations(bool bip16, bool bip141) const
{
    const auto value = [bip16, bip141](size_t total, const transaction& tx)
    {
        return ceiling_add(total, tx.signature_operations(bip16, bip141));
    };

    //*************************************************************************
    // CONSENSUS: Legacy sigops are counted in coinbase scripts despite the
    // fact that coinbase input scripts are never executed. There is no need
    // to exclude p2sh coinbase sigops since there is never a script to count.
    //*************************************************************************
    const auto& txs = transactions_;
    return std::accumulate(txs.begin(), txs.end(), zero, value);
}

size_t block::total_non_coinbase_inputs() const
{
    size_t value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (non_coinbase_inputs_ != boost::none)
    {
        value = non_coinbase_inputs_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    const auto inputs = [](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.inputs().size());
    };

    const auto& txs = transactions_;
    value = std::accumulate(txs.begin() + 1, txs.end(), zero, inputs);
    non_coinbase_inputs_ = value;

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

size_t block::total_inputs() const
{
    size_t value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (total_inputs_ != boost::none)
    {
        value = total_inputs_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    const auto inputs = [](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.inputs().size());
    };

    const auto& txs = transactions_;
    value = std::accumulate(txs.begin(), txs.end(), zero, inputs);
    total_inputs_ = value;

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

size_t block::weight() const
{
    // Block weight is 3 * Base size * + 1 * Total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases returns false.
bool block::is_extra_coinbases() const
{
    if (transactions_.empty())
        return false;

    const auto value = [](const transaction& tx)
    {
        return tx.is_coinbase();
    };

    const auto& txs = transactions_;
    return std::any_of(std::next(txs.begin()), txs.end(), value);
}

bool block::is_final(size_t height, uint32_t block_time) const
{
    const auto value = [=](const transaction& tx)
    {
        return tx.is_final(height, block_time);
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
    return is_distinct(std::move(hashes));
}

hash_digest block::generate_merkle_root(bool witness) const
{
    if (transactions_.empty())
        return null_hash;

    auto merkle = to_hashes(witness);
    if (is_one(merkle.size()))
        return merkle.front();

    // If number of hashes is odd, duplicate the last hash in the list.
    if (is_odd(merkle.size()))
        merkle.push_back(merkle.back());

    // Initial capacity is half of the original list.
    hash_list buffer;
    buffer.reserve(to_half(merkle.size()));

    // Reduce to a single hash (each iteration divides list size in half).
    while (!is_one(merkle.size()))
    {
        // Hash each pair of concatenated transaction hashes.
        for (auto it = merkle.begin(); it != merkle.end(); std::advance(it, 2))
            buffer.push_back(bitcoin_hash(splice(it[0], it[1])));

        std::swap(merkle, buffer);
        buffer.clear();
    }

    // There is now only one item in the list.
    return merkle.front();
}

//****************************************************************************
// CONSENSUS: This is only necessary because satoshi stores and queries as it
// validates, imposing an otherwise unnecessary partial transaction ordering.
//*****************************************************************************
bool block::is_forward_reference() const
{
    std::unordered_map<hash_digest, bool> hashes(transactions_.size());
    const auto is_forward = [&hashes](const input& input)
    {
        return !is_zero(hashes.count(input.previous_output().hash()));
    };

    for (const auto& tx: boost::adaptors::reverse(transactions_))
    {
        hashes.emplace(tx.hash(), true);

        if (std::any_of(tx.inputs().begin(), tx.inputs().end(), is_forward))
            return true;
    }

    return false;
}

// This is an early check that is redundant with block pool accept checks.
bool block::is_internal_double_spend() const
{
    if (transactions_.empty())
        return false;

    point::list outs;
    outs.reserve(total_non_coinbase_inputs());
    const auto& txs = transactions_;

    // Merge the prevouts of all non-coinbase transactions into one set.
    for (auto tx = std::next(txs.begin()); tx != txs.end(); ++tx)
    {
        auto out = tx->previous_outputs();
        std::move(out.begin(), out.end(), std::inserter(outs, outs.end()));
    }

    return !is_distinct(std::move(outs));
}

bool block::is_valid_merkle_root() const
{
    return generate_merkle_root() == header_.merkle_root();
}

// Overflow returns max_uint64.
uint64_t block::fees() const
{
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
uint64_t block::reward(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const
{
    return ceiling_add(fees(), subsidy(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42));
}

bool block::is_valid_coinbase_claim(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const
{
    return claim() <= reward(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42);
}

bool block::is_valid_coinbase_script(size_t height) const
{
    if (transactions_.empty() || transactions_.front().inputs().empty())
        return false;

    const auto& script = transactions_.front().inputs().front().script();
    return script::is_coinbase_pattern(script.operations(), height);
}

bool block::is_valid_witness_commitment() const
{
    if (transactions_.empty() || transactions_.front().inputs().empty())
        return false;

    hash_digest reserved, committed;
    const auto& coinbase = transactions_.front();

    // Last output of commitment pattern holds committed value (bip141).
    if (coinbase.inputs().front().extract_reserved_hash(reserved))
        for (const auto& output: boost::adaptors::reverse(coinbase.outputs()))
            if (output.extract_committed_hash(committed))
                return committed == bitcoin_hash(
                    splice(generate_merkle_root(true), reserved));

    // If no txs in block are segregated the commitment is optional (bip141).
    return !is_segregated();
}

bool block::is_segregated() const
{
    bool value;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (segregated_ != boost::none)
    {
        value = segregated_.get();
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return value;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    const auto segregated = [](const transaction& tx)
    {
        return tx.is_segregated();
    };

    // If no block tx has witness data the commitment is optional (bip141).
    value = std::any_of(transactions_.begin(), transactions_.end(), segregated);

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return value;
}

code block::check_transactions(uint64_t max_money) const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.check(max_money, false)))
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

// Validation.
//-----------------------------------------------------------------------------

// These checks are self-contained; blockchain (and so version) independent.
code block::check(uint64_t max_money, uint32_t timestamp_limit_seconds,
    uint32_t proof_of_work_limit, bool scrypt, bool header) const
{
    code ec;

    if (header && ((ec = header_.check(timestamp_limit_seconds,
        proof_of_work_limit, scrypt))))
        return ec;

    // TODO: relates to total of tx.size(false) (pool cache).
    else if (serialized_size(false) > max_block_size)
        return error::block_size_limit;

    else if (transactions_.empty())
        return error::empty_block;

    else if (!transactions_.front().is_coinbase())
        return error::first_not_coinbase;

    else if (is_extra_coinbases())
        return error::extra_coinbases;

    // TODO: determinable from tx pool graph.
    else if (is_forward_reference())
        return error::forward_reference;

    // This is subset of is_internal_double_spend if collisions cannot happen.
    ////else if (!is_distinct_transaction_set())
    ////    return error::internal_duplicate;

    // TODO: determinable from tx pool graph.
    else if (is_internal_double_spend())
        return error::block_internal_double_spend;

    // TODO: relates height to tx.hash(false) (pool cache).
    else if (!is_valid_merkle_root())
        return error::merkle_mismatch;

    // We cannot know if bip16 is enabled at this point so we disable it.
    // This will not make a difference unless prevouts are populated, in which
    // case they are ignored. This means that p2sh sigops are not counted here.
    // This is a preliminary check, the final count must come from connect().
    ////else if (signature_operations(false, false) > max_block_sigops)
    ////    return error::block_legacy_sigop_limit;

    else
        return check_transactions(max_money);
}

code block::accept(const system::settings& settings, bool transactions,
    bool header) const
{
    const auto state = header_.metadata.state;
    return state ? accept(*state, settings, transactions, header) :
        error::operation_failed;
}

// These checks assume that prevout caching is completed on all tx.inputs.
code block::accept(const chain_state& state,
    const system::settings& settings, bool transactions, bool header) const
{
    code ec;
    const auto bip16 = state.is_enabled(bip16_rule);
    const auto bip34 = state.is_enabled(bip34_rule);
    const auto bip42 = state.is_enabled(bip42_rule);
    const auto bip113 = state.is_enabled(bip113_rule);
    const auto bip141 = state.is_enabled(bip141_rule);

    const auto max_sigops = bip141 ? max_fast_sigops : max_block_sigops;
    const auto block_time = bip113 ? state.median_time_past() :
        header_.timestamp();

    if (header && ((ec = header_.accept(state))))
        return ec;

    else if (state.is_under_checkpoint())
        return error::success;

    // TODO: relates height to total of tx.size(true) (pool cache).
    else if (bip141 && weight() > max_block_weight)
        return error::block_weight_limit;

    else if (bip34 && !is_valid_coinbase_script(state.height()))
        return error::coinbase_height_mismatch;

    // TODO: relates height to total of tx.fee (pool cach).
    else if (!is_valid_coinbase_claim(state.height(),
        settings.subsidy_interval_blocks, settings.initial_subsidy(), bip42))
        return error::coinbase_value_limit;

    // TODO: relates median time past to tx.locktime (pool cache min tx.time).
    else if (!is_final(state.height(), block_time))
        return error::block_non_final;

    // TODO: relates height to tx.hash(true) (pool cache).
    else if (bip141 && !is_valid_witness_commitment())
        return error::invalid_witness_commitment;

    // TODO: determine if performance benefit is worth excluding sigops here.
    // TODO: relates block limit to total of tx.sigops (pool cache).
    else if (transactions && signature_operations(bip16, bip141) > max_sigops)
        return error::block_embedded_sigop_limit;

    else if (transactions)
        return accept_transactions(state);

    else
        return ec;
}

code block::connect() const
{
    const auto state = header_.metadata.state;
    return state ? connect(*state) : error::operation_failed;
}

code block::connect(const chain_state& state) const
{
    if (state.is_under_checkpoint())
        return error::success;

    return connect_transactions(state);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
