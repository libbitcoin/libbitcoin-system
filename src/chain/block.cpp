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
#include <set>
#include <type_traits>
#include <utility>
#include <unordered_map>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/enums/rule_fork.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/mutex.hpp>
#include <bitcoin/system/optional.hpp>
#include <bitcoin/system/settings.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

static default_allocator<hash_digest> no_fill_hash_allocator{};

// Constructors.
//-----------------------------------------------------------------------------

block::block()
  : block({}, {}, false)
{
}

block::block(const block& other)
  : block(other.header_, other.transactions_, other.valid_)
{
}

block::block(block&& other)
  : block(std::move(other.header_), std::move(other.transactions_),
      other.valid_)
{
}

block::block(const chain::header& header, const transaction::list& transactions)
  : block(header, transactions, true)
{
}

block::block(chain::header&& header, transaction::list&& transactions)
  : block(std::move(header), std::move(transactions), true)
{
}

// protected
block::block(const chain::header& header, const transaction::list& transactions,
    bool valid)
  : header_(header),
    transactions_(transactions),
    valid_(valid)
{
}

// protected
block::block(chain::header&& header, transaction::list&& transactions,
    bool valid)
  : header_(std::move(header)),
    transactions_(std::move(transactions)),
    valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

block& block::operator=(block&& other)
{
    header_ = std::move(other.header_);
    transactions_ = std::move(other.transactions_);
    valid_ = other.valid_;
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
    stream::in::copy stream(data);
    return from_data(stream, witness);
}

bool block::from_data(std::istream& stream, bool witness)
{
    read::bytes::istream source(stream);
    return from_data(source, witness);
}

// Full block deserialization is always canonical encoding.
bool block::from_data(reader& source, bool witness)
{
    reset();

    header_.from_data(source);

    const auto count = source.read_size();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
        source.invalidate();
    else
        transactions_.resize(count);

    for (auto& tx: transactions_)
        tx.from_data(source, witness);

    if (!witness)
        strip_witness();

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// private
void block::reset()
{
    header_.reset();
    transactions_.clear();
    transactions_.shrink_to_fit();
    valid_ = false;
}

bool block::is_valid() const
{
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk block::to_data(bool witness) const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void block::to_data(std::ostream& stream, bool witness) const
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void block::to_data(writer& sink, bool witness) const
{
    DEBUG_ONLY(const auto size = serialized_size(witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    header_.to_data(sink);
    sink.write_variable(transactions_.size());

    for (const auto& transaction: transactions_)
        transaction.to_data(sink, witness);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

hash_list block::to_hashes(bool witness) const
{
    hash_list out(no_fill_hash_allocator);
    out.resize(transactions_.size());

    const auto hash = [witness](const transaction& tx)
    {
        return tx.hash(witness);
    };

    const auto& txs = transactions_;
    std::transform(txs.begin(), txs.end(), out.begin(), hash);
    return out;
}

// Properties.
//-----------------------------------------------------------------------------

// Full block serialization is always canonical encoding.
size_t block::serialized_size(bool witness) const
{
    const auto sum = [witness](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.serialized_size(witness));
    };

    const auto& txs = transactions_;
    return header_.serialized_size()
        + variable_size(transactions_.size())
        + std::accumulate(txs.begin(), txs.end(), zero, sum);

}

const chain::header& block::header() const
{
    return header_;
}

const transaction::list& block::transactions() const
{
    return transactions_;
}

hash_digest block::hash() const
{
    return header_.hash();
}

void block::strip_witness()
{
    const auto strip = [](transaction& transaction)
    {
        transaction.strip_witness();
    };

    std::for_each(transactions_.begin(), transactions_.end(), strip);
}

// Validation helpers.
//-----------------------------------------------------------------------------

// static
uint64_t block::subsidy(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42)
{
    const auto halvings = height / subsidy_interval;

    //*************************************************************************
    // CONSENSUS: bip42 compensates for c++ undefined behavior of a right
    // shift of a number of bits greater or equal to the shifted integer width.
    // However, being undefined, the result of this operation may vary by
    // compiler. The call below explicitly implements the presumed pre-bip42
    // behavior (shift overflow modulo) by default, and the specified bip42
    // behavior (shift overflow to zero) when bip42 is enabled.
    //*************************************************************************
    return shift_right(initial_block_subsidy_satoshi, halvings, bip42);
}

size_t block::total_non_coinbase_inputs() const
{
    const auto inputs = [](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.inputs().size());
    };

    const auto& txs = transactions_;
    return std::accumulate(std::next(txs.begin()), txs.end(), zero, inputs);
}

size_t block::total_inputs() const
{
    const auto inputs = [](size_t total, const transaction& tx)
    {
        return safe_add(total, tx.inputs().size());
    };

    const auto& txs = transactions_;
    return std::accumulate(txs.begin(), txs.end(), zero, inputs);
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

// TODO: add bip50 to chain_state with timestamp range activation.
// "Special short-term limits to avoid 10,000 BDB lock limit.
// Count of unique txids <= 4500 to prevent 10000 BDB lock exhaustion.
// header.timestamp > 1363039171 && header.timestamp < 1368576000."
bool block::is_hash_limit_exceeded() const
{
    if (transactions_.empty())
        return false;

    // A set is used to collapse duplicates.
    std::set<hash_digest> hashes;

    // Just the coinbase tx hash, skip its null input hashes.
    const auto& txs = transactions_;
    hashes.insert(txs.front().hash());

    const auto tx_inserter = [&hashes](const transaction& tx)
    {
        const auto input_inserter = [&hashes](const input& input)
        {
            hashes.insert(input.previous_output().hash());
        };

        hashes.insert(tx.hash());
        const auto& inputs = tx.inputs();
        std::for_each(inputs.begin(), inputs.end(), input_inserter);
    };

    std::for_each(std::next(txs.begin()), txs.end(), tx_inserter);
    return hashes.size() <= hash_limit;
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
    // A set is used to collapse duplicates.
    std::set<hash_digest> hashes;

    const auto hasher = [&hashes](const transaction& tx)
    {
        hashes.insert(tx.hash());
    };

    const auto& txs = transactions_;
    std::for_each(txs.begin(), txs.end(), hasher);
    return hashes.size() == txs.size();
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

bool block::is_internal_double_spend() const
{
    if (transactions_.empty())
        return false;

    // A set is used to collapse duplicates.
    std::set<point> outs;

    const auto inserter = [&outs](const transaction& tx)
    {
        auto out = tx.previous_outputs();
        std::move(out.begin(), out.end(), std::inserter(outs, outs.end()));
    };

    // Move the copied prevouts of all non-coinbase transactions into one set.
    const auto& txs = transactions_;
    std::for_each(std::next(txs.begin()), txs.end(), inserter);
    return outs.size() != total_non_coinbase_inputs();
}

bool block::is_valid_merkle_root() const
{
    return generate_merkle_root() == header_.merkle_root();
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
    const auto segregated = [](const transaction& tx)
    {
        return tx.is_segregated();
    };

    // If no block tx has witness data the commitment is optional (bip141).
    return std::any_of(transactions_.begin(), transactions_.end(), segregated);
}

code block::check_transactions(uint64_t max_money) const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.check(max_money, false)))
            return ec;

    return error::block_success;
}

code block::accept_transactions(const chain_state& state) const
{
    code ec;

    for (const auto& tx: transactions_)
        if ((ec = tx.accept(state, false)))
            return ec;

    return error::block_success;
}

////code block::connect_transactions(const chain_state& state) const
////{
////    code ec;
////
////    for (const auto& tx: transactions_)
////        if ((ec = tx.connect(state)))
////            return ec;
////
////    return error::block_success;
////}

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

    else if (transactions_.empty())
        return error::empty_block;

    // Relates to total of tx.size (pool cache tx.size(false)).
    else if (serialized_size(false) > max_block_size)
        return error::block_size_limit;

    // Coinbase transactions are not pooled (cache is_coinbase).
    else if (!transactions_.front().is_coinbase())
        return error::first_not_coinbase;

    // Coinbase transactions are not pooled (cache is_coinbase).
    else if (is_extra_coinbases())
        return error::extra_coinbases;

    // Determinable from tx pool graph (cannot cache, must navigate).
    // Satoshi implementation side effect, as tx order is otherwise irrelevant.
    // Txs cannot form cycles under the presumption of hash non-collision.
    else if (is_forward_reference())
        return error::forward_reference;

    // Subset of is_internal_double_spend if sha256 collisions cannot happen.
    else if (!is_distinct_transaction_set())
        return error::internal_duplicate;

    // Determinable from tx pool graph (cannot cache, must navigate).
    // This also precludes the block merkle calculation DoS exploit.
    // bitcointalk.org/?topic=102395
    else if (is_internal_double_spend())
        return error::block_internal_double_spend;

    // Relates height to tx.hash (pool cache tx.hash(false)).
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

// These checks assume that prevout caching is completed on all tx.inputs.
code block::accept(const chain_state& state,
    const system::settings& settings, bool transactions, bool header) const
{
    code ec;
    const auto bip16 = state.is_enabled(bip16_rule);
    const auto bip30 = state.is_enabled(bip30_rule);
    const auto bip34 = state.is_enabled(bip34_rule);
    const auto bip42 = state.is_enabled(bip42_rule);
    const auto bip50 = state.is_enabled(bip50_rule);
    const auto bip113 = state.is_enabled(bip113_rule);
    const auto bip141 = state.is_enabled(bip141_rule);

    const auto max_sigops = bip141 ? max_fast_sigops : max_block_sigops;
    const auto block_time = bip113 ? state.median_time_past() :
        header_.timestamp();

    if (header && ((ec = header_.accept(state))))
        return ec;

    else if (state.is_under_checkpoint())
        return error::success;

    // Relates block limit to total of tx.weight (pool cache tx.size(t/f)).
    else if (bip141 && weight() > max_block_weight)
        return error::block_weight_limit;

    // Relates block height to coinbase, always under checkpoint.
    else if (bip34 && !is_valid_coinbase_script(state.height()))
        return error::coinbase_height_mismatch;

    // Relates block time to tx and prevout hashes, always under checkpoint.
    else if (bip50 && is_hash_limit_exceeded())
        return error::temporary_hash_limit;

    // This merely rolls up checks already performed by tx accept.
    // Relates block time to each tx.locktime (pool cache tx.locktime).
    else if (!is_final(state.height(), block_time))
        return error::block_non_final;

    // This causes a second merkle root computation (uncached).
    else if (bip141 && !is_valid_witness_commitment())
        return error::invalid_witness_commitment;

    // requires prevouts
    // This is a spentness check, to be applied at confirmation not validation.
    ////else if (bip30 && is_unspent_coinbase_collision())
    ////    return error::unspent_coinbase_collision;

    // requires prevouts
    // Relates block height to total of tx.fee (pool cache tx.fee).
    ////else if (!is_valid_coinbase_claim(state.height(),
    ////    settings.subsidy_interval_blocks, settings.initial_subsidy(), bip42))
    ////    return error::coinbase_value_limit;

    // requires prevouts
    // Relates block limit to total of tx.sigops (pool cache tx.sigops).
    ////else if (transactions && signature_operations(bip16, bip141) > max_sigops)
    ////    return error::block_embedded_sigop_limit;

    else if (transactions)
        return accept_transactions(state);

    else
        return ec;
}

////code block::connect(const chain_state& state) const
////{
////    if (state.is_under_checkpoint())
////        return error::block_success;
////
////    return connect_transactions(state);
////}

} // namespace chain
} // namespace system
} // namespace libbitcoin
