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
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/settings.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

static default_allocator<hash_digest> no_fill_hash_allocator{};

// Constructors.
// ----------------------------------------------------------------------------

block::block()
  : block({}, {}, false)
{
}

block::block(const block& other)
  : block(other.header_, other.txs_, other.valid_)
{
}

block::block(block&& other)
  : block(std::move(other.header_), std::move(other.txs_),
      other.valid_)
{
}

block::block(const chain::header& header, const transaction::list& txs)
  : block(header, txs, true)
{
}

block::block(chain::header&& header, transaction::list&& txs)
  : block(std::move(header), std::move(txs), true)
{
}

block::block(const data_chunk& data, bool witness)
{
    from_data(data, witness);
}

block::block(std::istream& stream, bool witness)
{
    from_data(stream, witness);
}

block::block(reader& source, bool witness)
{
    from_data(source, witness);
}

// protected
block::block(chain::header&& header, transaction::list&& txs, bool valid)
  : header_(std::move(header)),
    txs_(std::move(txs)),
    valid_(valid)
{
}

// protected
block::block(const chain::header& header, const transaction::list& txs,
    bool valid)
  : header_(header),
    txs_(txs),
    valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

block& block::operator=(block&& other)
{
    header_ = std::move(other.header_);
    txs_ = std::move(other.txs_);
    valid_ = other.valid_;
    return *this;
}

block& block::operator=(const block& other)
{
    header_ = other.header_;
    txs_ = other.txs_;
    valid_ = other.valid_;
    return *this;
}

bool block::operator==(const block& other) const
{
    return (header_ == other.header_)
        && (txs_ == other.txs_);
}

bool block::operator!=(const block& other) const
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

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

bool block::from_data(reader& source, bool witness)
{
    reset();

    header_.from_data(source);
    const auto count = source.read_size();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
    {
        source.invalidate();
    }
    else
    {
        txs_.reserve(count);
        for (size_t tx = 0; tx < count; ++tx)
            txs_.emplace_back(source, witness);
    }

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// private
void block::reset()
{
    header_.reset();
    txs_.clear();
    txs_.shrink_to_fit();
    valid_ = false;
}

bool block::is_valid() const
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

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
    sink.write_variable(txs_.size());

    for (const auto& transaction: txs_)
        transaction.to_data(sink, witness);

    BITCOIN_ASSERT(sink && sink.get_position() - start == size);
}

// Properties.
// ----------------------------------------------------------------------------

size_t block::serialized_size(bool witness) const
{
    // Overflow returns max_size_t.
    const auto sum = [witness](size_t total, const transaction& tx)
    {
        return ceilinged_add(total, tx.serialized_size(witness));
    };

    return header_.serialized_size()
        + variable_size(txs_.size())
        + std::accumulate(txs_.begin(), txs_.end(), zero, sum);

}

const chain::header& block::header() const
{
    return header_;
}

const transaction::list& block::transactions() const
{
    return txs_;
}

hash_list block::transaction_hashes(bool witness) const
{
    hash_list out(no_fill_hash_allocator);
    out.resize(txs_.size());

    const auto hash = [witness](const transaction& tx)
    {
        return tx.hash(witness);
    };

    std::transform(txs_.begin(), txs_.end(), out.begin(), hash);
    return out;
}

// computed
hash_digest block::hash() const
{
    return header_.hash();
}

// Connect.
// ----------------------------------------------------------------------------

////// Subset of is_internal_double_spend if sha256 collisions cannot happen.
////bool block::is_distinct_transaction_set() const
////{
////    // A set is used to collapse duplicates.
////    std::set<hash_digest> hashes;
////
////    const auto hasher = [&hashes](const transaction& tx)
////    {
////        hashes.insert(tx.hash());
////    };
////
////    std::for_each(txs_.begin(), txs_.end(), hasher);
////    return hashes.size() == txs.size();
////}

bool block::is_empty() const
{
    return txs_.empty();
}

bool block::is_oversized() const
{
    return serialized_size(false) > max_block_size;
}

bool block::is_first_non_coinbase() const
{
    return !txs_.front().is_coinbase();
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases returns false.
bool block::is_extra_coinbases() const
{
    if (txs_.empty())
        return false;

    const auto value = [](const transaction& tx)
    {
        return tx.is_coinbase();
    };

    return std::any_of(std::next(txs_.begin()), txs_.end(), value);
}

//*****************************************************************************
// CONSENSUS: This is only necessary because satoshi stores and queries as it
// validates, imposing an otherwise unnecessary partial transaction ordering.
//*****************************************************************************
bool block::is_forward_reference() const
{
    std::unordered_map<hash_digest, bool> hashes(txs_.size());

    const auto is_forward = [&hashes](const input& input)
    {
        return !is_zero(hashes.count(input.point().hash()));
    };

    for (const auto& tx: boost::adaptors::reverse(txs_))
    {
        hashes.emplace(tx.hash(false), true);
        if (std::any_of(tx.inputs().begin(), tx.inputs().end(), is_forward))
            return true;
    }

    return false;
}

// private
size_t block::non_coinbase_inputs() const
{
    // Overflow returns max_size_t.
    const auto inputs = [](size_t total, const transaction& tx)
    {
        return ceilinged_add(total, tx.inputs().size());
    };

    return std::accumulate(std::next(txs_.begin()), txs_.end(), zero, inputs);
}

bool block::is_internal_double_spend() const
{
    if (txs_.empty())
        return false;

    // A set is used to collapse duplicate points.
    std::set<point> outs;

    const auto inserter = [&outs](const transaction& tx)
    {
        auto out = tx.points();
        std::move(out.begin(), out.end(), std::inserter(outs, outs.end()));
    };

    // Move the points of all non-coinbase transactions into one set.
    std::for_each(std::next(txs_.begin()), txs_.end(), inserter);
    return outs.size() != non_coinbase_inputs();
}

// private
hash_digest block::generate_merkle_root(bool witness) const
{
    if (txs_.empty())
        return null_hash;

    auto merkle = transaction_hashes(witness);
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

bool block::is_invalid_merkle_root() const
{
    return generate_merkle_root(false) != header_.merkle_root();
}

// Accept (contextual).
// ----------------------------------------------------------------------------

size_t block::weight() const
{
    // Block weight is 3 * Base size * + 1 * Total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

bool block::is_overweight() const
{
    return weight() > max_block_weight;
}

bool block::is_invalid_coinbase_script(size_t height) const
{
    if (txs_.empty() || txs_.front().inputs().empty())
        return false;

    const auto& script = txs_.front().inputs().front().script();
    return !script::is_coinbase_pattern(script.operations(), height);
}

// TODO: add bip50 to chain_state with timestamp range activation.
// "Special short-term limits to avoid 10,000 BDB lock limit.
// Count of unique txids <= 4500 to prevent 10000 BDB lock exhaustion.
// header.timestamp > 1363039171 && header.timestamp < 1368576000."
bool block::is_hash_limit_exceeded() const
{
    if (txs_.empty())
        return false;

    // A set is used to collapse duplicates.
    std::set<hash_digest> hashes;

    // Just the coinbase tx hash, skip its null input hashes.
    hashes.insert(txs_.front().hash(false));

    const auto tx_inserter = [=, &hashes](const transaction& tx)
    {
        const auto input_inserter = [&hashes](const input& input)
        {
            hashes.insert(input.point().hash());
        };

        hashes.insert(tx.hash(false));
        const auto& inputs = tx.inputs();
        std::for_each(inputs.begin(), inputs.end(), input_inserter);
    };

    std::for_each(std::next(txs_.begin()), txs_.end(), tx_inserter);
    return hashes.size() > hash_limit;
}

bool block::is_segregated() const
{
    const auto segregated = [](const transaction& tx)
    {
        return tx.is_segregated();
    };

    return std::any_of(txs_.begin(), txs_.end(), segregated);
}

bool block::is_invalid_witness_commitment() const
{
    if (txs_.empty() || txs_.front().inputs().empty())
        return false;

    hash_digest reserved, committed;
    const auto& coinbase = txs_.front();

    // Last output of commitment pattern holds committed value (bip141).
    if (coinbase.inputs().front().reserved_hash(reserved))
        for (const auto& output: boost::adaptors::reverse(coinbase.outputs()))
            if (output.committed_hash(committed))
                return committed == bitcoin_hash(
                    splice(generate_merkle_root(true), reserved));
    
    // If no block tx has witness data the commitment is optional (bip141).
    return !is_segregated();
}


//*****************************************************************************
// CONSENSUS:
// bip42 compensates for c++ undefined behavior of a right shift of a number of
// bits greater or equal to the shifted integer width. Yet being undefined, the
// result of this operation may vary by compiler. The shift_right call below
// explicitly implements presumed pre-bip42 behavior (shift overflow modulo) by
// default, and specified bip42 behavior (shift overflow to zero) with bip42.
//*****************************************************************************
constexpr uint64_t block_subsidy(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42)
{
    const auto halvings = height / subsidy_interval;
    return shift_right(initial_block_subsidy_satoshi, halvings, bip42);
}

// Prevouts required.

uint64_t block::fees() const
{
    // Overflow returns max_uint64.
    const auto value = [](uint64_t total, const transaction& tx)
    {
        return ceilinged_add(total, tx.fee());
    };

    return std::accumulate(txs_.begin(), txs_.end(), uint64_t(0), value);
}

uint64_t block::claim() const
{
    return txs_.empty() ? zero : txs_.front().value();
}

uint64_t block::reward(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const
{
    // Overflow returns max_uint64.
    return ceilinged_add(fees(), block_subsidy(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42));
}

bool block::is_overspent(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const
{
    return claim() > reward(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42);
}

size_t block::is_signature_operations_limited(bool bip16, bool bip141) const
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;

    // Overflow returns max_size_t.
    const auto value = [bip16, bip141](size_t total, const transaction& tx)
    {
        return ceilinged_add(total, tx.signature_operations(bip16, bip141));
    };

    return std::accumulate(txs_.begin(), txs_.end(), zero, value) > limit;
}

//*****************************************************************************
// CONSENSUS:
// This check is excluded under two bip30 exception blocks. This also cannot
// occur in any branch above bip34, due to height in coinbase and the
// presumption of sha256 non-collision. So this check is bypassed for both
// exception blocks and if bip34 is active (including under bip90 activation).
//*****************************************************************************
bool block::is_unspent_coinbase_collision(size_t height) const
{
    if (txs_.empty() || txs_.front().inputs().empty())
        return false;

    const auto& prevout = txs_.front().inputs().front().prevout;

    // This requires that prevout.spent was populated for the height of the
    // validating block, otherwise a collision (unspent) must be assumed.
    return !(height > prevout.height && prevout.spent);
}

// Delegated.
// ----------------------------------------------------------------------------

code block::check_transactions() const
{
    code ec;

    for (const auto& tx: txs_)
        if ((ec = tx.check()))
            return ec;

    return error::block_success;
}

code block::accept_transactions(const context& state) const
{
    code ec;

    for (const auto& tx: txs_)
        if ((ec = tx.accept(state)))
            return ec;

    return error::block_success;
}

code block::connect_transactions(const context& state) const
{
    code ec;

    for (const auto& tx: txs_)
        if ((ec = tx.connect(state)))
            return ec;

    return error::block_success;
}

// Validation.
// ----------------------------------------------------------------------------

// The block header is checked independently.
// These checks are self-contained; blockchain (and so version) independent.
code block::check() const
{
    code ec;

    // Inputs and outputs are required.
    if (is_empty())
        return error::empty_block;

    // Relates to total of tx.size (pool cache tx.size(false)).
    if (is_oversized())
        return error::block_size_limit;

    // The first transaction must be coinbase.
    if (is_first_non_coinbase())
        return error::first_not_coinbase;

    // Only the first transaction may be coinbase.
    if (is_extra_coinbases())
        return error::extra_coinbases;

    // Determinable from tx pool graph.
    // Satoshi implementation side effect, as tx order is otherwise irrelevant.
    if (is_forward_reference())
        return error::forward_reference;

    // Determinable from tx pool graph.
    // This also precludes the block merkle calculation DoS exploit.
    // bitcointalk.org/?topic=102395
    if (is_internal_double_spend())
        return error::block_internal_double_spend;

    // Relates height to tx.hash (pool cache tx.hash(false)).
    if (is_invalid_merkle_root())
        return error::merkle_mismatch;

    // error::empty_transaction
    // error::previous_output_null
    // error::invalid_coinbase_script_size
    return check_transactions();
}

// The block header is accepted independently using chain_state.
// These checks assume that prevout caching is completed on all tx.inputs.
code block::accept(const context& state, size_t subsidy_interval,
    uint64_t initial_subsidy) const
{
    code ec;
    const auto bip16 = state.is_enabled(bip16_rule);
    const auto bip30 = state.is_enabled(bip30_rule);
    const auto bip34 = state.is_enabled(bip34_rule);
    const auto bip42 = state.is_enabled(bip42_rule);
    const auto bip50 = state.is_enabled(bip50_rule);
    const auto bip113 = state.is_enabled(bip113_rule);
    const auto bip141 = state.is_enabled(bip141_rule);

    // Relates block limit to total of tx.weight (pool cache tx.size(t/f)).
    if (bip141 && is_overweight())
        return error::block_weight_limit;

    // Relates block height to coinbase, always under checkpoint.
    if (bip34 && is_invalid_coinbase_script(state.height))
        return error::coinbase_height_mismatch;

    // Relates block time to tx and prevout hashes, always under checkpoint.
    if (bip50 && is_hash_limit_exceeded())
        return error::temporary_hash_limit;

    // Static check but requires context.
    if (bip141 && is_invalid_witness_commitment())
        return error::invalid_witness_commitment;

    // prevouts required

    // Relates block height to total of tx.fee (pool cache tx.fee).
    if (is_overspent(state.height, subsidy_interval, initial_subsidy, bip42))
        return error::coinbase_value_limit;

    // Relates block limit to total of tx.sigops (pool cache tx.sigops).
    if (is_signature_operations_limited(bip16, bip141))
        return error::block_sigop_limit;

    // prevout confirmation state required

    if (bip30 && !bip34 && is_unspent_coinbase_collision(state.height))
        return error::unspent_coinbase_collision;

    // error::unexpected_witness_transaction
    // error::missing_previous_output
    // error::spend_exceeds_value
    // error::coinbase_maturity
    // error::relative_time_locked
    return accept_transactions(state);
}

code block::connect(const context& state) const
{
    return connect_transactions(state);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
