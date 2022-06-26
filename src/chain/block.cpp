/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <unordered_set>
#include <utility>
#include <unordered_map>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/forks.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/settings.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
// ----------------------------------------------------------------------------

block::block() noexcept
  : block(to_shared<chain::header>(), to_shared<chain::transaction_ptrs>(),
      false)
{
}

block::block(chain::header&& header, chain::transactions&& txs) noexcept
  : block(to_shared(std::move(header)), to_shareds(std::move(txs)), true)
{
}

block::block(const chain::header& header,
    const chain::transactions& txs) noexcept
  : block(to_shared<chain::header>(header), to_shareds(txs), true)
{
}

block::block(const chain::header::cptr& header,
    const chain::transactions_cptr& txs) noexcept
  : block(header ? header : to_shared<chain::header>(),
      txs ? txs : to_shared<transaction_ptrs>(), true)
{
}

block::block(const data_slice& data, bool witness) noexcept
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : block(stream::in::copy(data), witness)
    BC_POP_WARNING()
{
}


block::block(std::istream&& stream, bool witness) noexcept
  : block(read::bytes::istream(stream), witness)
{
}

block::block(std::istream& stream, bool witness) noexcept
  : block(read::bytes::istream(stream), witness)
{
}

block::block(reader&& source, bool witness) noexcept
  : block(from_data(source, witness))
{
}

block::block(reader& source, bool witness) noexcept
  : block(from_data(source, witness))
{
}

// protected
block::block(const chain::header::cptr& header,
    const chain::transactions_cptr& txs, bool valid) noexcept
  : header_(header), txs_(txs), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool block::operator==(const block& other) const noexcept
{
    return (header_ == other.header_ || *header_ == *other.header_)
        && deep_equal(*txs_, *other.txs_);
}

bool block::operator!=(const block& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
block block::from_data(reader& source, bool witness) noexcept
{
    const auto read_transactions = [witness](reader& source) noexcept
    {
        auto txs = std::make_shared<transaction_ptrs>();
        txs->reserve(source.read_size(max_block_size));

        for (size_t tx = 0; tx < txs->capacity(); ++tx)
        {
            BC_PUSH_WARNING(NO_NEW_DELETE)
            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            txs->emplace_back(new transaction{ source, witness });
            BC_POP_WARNING()
            BC_POP_WARNING()
        }

        return txs;
    };

    return
    {
        BC_PUSH_WARNING(NO_NEW_DELETE)
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        to_shared(new chain::header{ source }),
        BC_POP_WARNING()
        BC_POP_WARNING()

        read_transactions(source),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk block::to_data(bool witness) const noexcept
{
    data_chunk data(serialized_size(witness), no_fill_byte_allocator);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream, witness);
    return data;
}

void block::to_data(std::ostream& stream, bool witness) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void block::to_data(writer& sink, bool witness) const noexcept
{
    header_->to_data(sink);
    sink.write_variable(txs_->size());

    for (const auto& tx: *txs_)
        tx->to_data(sink, witness);
}

// Properties.
// ----------------------------------------------------------------------------

bool block::is_valid() const noexcept
{
    return valid_;
}

const chain::header&block::header() const noexcept
{
    return *header_;
}

const chain::header::cptr block::header_ptr() const noexcept
{
    return header_;
}

// vector<transaction> is not exposed (because we don't have it).
// This would reqiure a from_shared(txs_) conversion (expensive).
const transactions_cptr& block::transactions_ptr() const noexcept
{
    return txs_;
}

hash_list block::transaction_hashes(bool witness) const noexcept
{
    static no_fill_allocator<hash_digest> no_fill_hash_allocator{};
    const auto count = txs_->size();

    // Addition is guarded by block size limit.
    const auto space = count + (is_odd(count) && !is_one(count) ? one : zero);

    // Excess reservation accounts for generate_merkle_root addition.
    hash_list out(space, no_fill_hash_allocator);

    // Vector capacity is never reduced when resizing to smaller size.
    out.resize(count);

    const auto hash = [witness](const transaction::cptr& tx) noexcept
    {
        return tx->hash(witness);
    };

    std::transform(txs_->begin(), txs_->end(), out.begin(), hash);
    return out;
}

// computed
hash_digest block::hash() const noexcept
{
    return header_->hash();
}

size_t block::serialized_size(bool witness) const noexcept
{
    // Overflow returns max_size_t.
    const auto sum = [witness](size_t total, const transaction::cptr& tx) noexcept
    {
        return ceilinged_add(total, tx->serialized_size(witness));
    };

    return header::serialized_size()
        + variable_size(txs_->size())
        + std::accumulate(txs_->begin(), txs_->end(), zero, sum);

}

// Connect.
// ----------------------------------------------------------------------------

////// Subset of is_internal_double_spend if sha256 collisions cannot happen.
////bool block::is_distinct_transaction_set() const
////{
////    // A set is used to collapse duplicates.
////    std::set<hash_digest> hashes;
////
////    for (const auto& tx: *txs_)
////        hashes.insert(tx->hash(false));
////
////    return hashes.size() == txs_->size();
////}

bool block::is_empty() const noexcept
{
    return txs_->empty();
}

bool block::is_oversized() const noexcept
{
    return serialized_size(false) > max_block_size;
}

bool block::is_first_non_coinbase() const noexcept
{
    return !txs_->front()->is_coinbase();
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases returns false.
bool block::is_extra_coinbases() const noexcept
{
    if (txs_->empty())
        return false;

    const auto value = [](const transaction::cptr& tx) noexcept
    {
        return tx->is_coinbase();
    };

    return std::any_of(std::next(txs_->begin()), txs_->end(), value);
}

//*****************************************************************************
// CONSENSUS: This is only necessary because satoshi stores and queries as it
// validates, imposing an otherwise unnecessary partial transaction ordering.
//*****************************************************************************
bool block::is_forward_reference() const noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    std::unordered_map<hash_digest, bool> hashes(txs_->size());
    BC_POP_WARNING()

    const auto is_forward = [&hashes](const input::cptr& input) noexcept
    {
        return !is_zero(hashes.count(input->point().hash()));
    };

    for (const auto& tx: views_reverse(*txs_))
    {
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        hashes.emplace(tx->hash(false), false);
        BC_POP_WARNING()

        const auto& inputs = *tx->inputs_ptr();
        if (std::any_of(inputs.begin(), inputs.end(), is_forward))
            return true;
    }

    return false;
}

// private
size_t block::non_coinbase_inputs() const noexcept
{
    // Overflow returns max_size_t.
    const auto inputs = [](size_t total, const transaction::cptr& tx) noexcept
    {
        return ceilinged_add(total, tx->inputs_ptr()->size());
    };

    return std::accumulate(std::next(txs_->begin()), txs_->end(), zero, inputs);
}

bool block::is_internal_double_spend() const noexcept
{
    if (txs_->empty())
        return false;

    // A set is used to collapse duplicate points.
    std::unordered_set<point> outs{};

    // Move the points of all non-coinbase transactions into one set.
    for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
    {
        auto out =(*tx)->points();
        std::move(out.begin(), out.end(), std::inserter(outs, outs.end()));
    }

    return outs.size() != non_coinbase_inputs();
}

// private
hash_digest block::generate_merkle_root(bool witness) const noexcept
{
    if (txs_->empty())
        return null_hash;

    auto merkle = transaction_hashes(witness);

    // Reduce to a single hash (each iteration divides list size in half).
    while (!is_one(merkle.size()))
    {
        // If number of hashes is odd, duplicate the last hash in the list.
        if (is_odd(merkle.size()))
            merkle.push_back(merkle.back());

        // Hash all pairs of tx hashes in place, resizes vector to half.
        hash_reduce(merkle);
    }

    // There is now only one item in the list.
    return merkle.front();
}

bool block::is_invalid_merkle_root() const noexcept
{
    return generate_merkle_root(false) != header_->merkle_root();
}

// Accept (contextual).
// ----------------------------------------------------------------------------

size_t block::weight() const noexcept
{
    // Block weight is 3 * Base size * + 1 * Total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

bool block::is_overweight() const noexcept
{
    return weight() > max_block_weight;
}

bool block::is_invalid_coinbase_script(size_t height) const noexcept
{
    if (txs_->empty() || txs_->front()->inputs_ptr()->empty())
        return false;

    const auto& script = txs_->front()->inputs_ptr()->front()->script();
    return !script::is_coinbase_pattern(script.ops(), height);
}

// TODO: add bip50 to chain_state with timestamp range activation.
// "Special short-term limits to avoid 10,000 BDB lock limit.
// Count of unique txids <= 4500 to prevent 10000 BDB lock exhaustion.
// header.timestamp > 1363039171 && header.timestamp < 1368576000."
bool block::is_hash_limit_exceeded() const noexcept
{
    if (txs_->empty())
        return false;

    // A set is used to collapse duplicates.
    std::unordered_set<hash_digest> hashes;

    // Just the coinbase tx hash, skip its null input hashes.
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    hashes.insert(txs_->front()->hash(false));
    BC_POP_WARNING()

    for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
    {
        // Insert the transaction hash.
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        hashes.insert((*tx)->hash(false));
        BC_POP_WARNING()

        const auto& inputs = *(*tx)->inputs_ptr();

        // Insert all input point hashes.
        for (const auto& input: inputs)
        {
            BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
            hashes.insert(input->point().hash());
            BC_POP_WARNING()
        }
    }

    return hashes.size() > hash_limit;
}

bool block::is_segregated() const noexcept
{
    const auto segregated = [](const transaction::cptr& tx) noexcept
    {
        return tx->is_segregated();
    };

    return std::any_of(txs_->begin(), txs_->end(), segregated);
}

bool block::is_invalid_witness_commitment() const noexcept
{
    if (txs_->empty() || txs_->front()->inputs_ptr()->empty())
        return false;

    hash_digest reserved{}, committed{};
    const auto& coinbase = txs_->front();

    // Last output of commitment pattern holds committed value (bip141).
    if (coinbase->inputs_ptr()->front()->reserved_hash(reserved))
    {
        const auto& outputs = *coinbase->outputs_ptr();

        for (const auto& output: views_reverse(outputs))
        {
            if (output->committed_hash(committed))
                return committed == bitcoin_hash(generate_merkle_root(true),
                    reserved);
        }
    }
    
    // If no block tx has witness data the commitment is optional (bip141).
    return !is_segregated();
}

//*****************************************************************************
// CONSENSUS:
// bip42 compensates for C++ undefined behavior of a right shift of a number of
// bits greater or equal to the shifted integer width. Yet being undefined, the
// result of this operation may vary by compiler. The shift_right call below
// explicitly implements presumed pre-bip42 behavior (shift overflow modulo) by
// default, and specified bip42 behavior (shift overflow to zero) with bip42.
//*****************************************************************************
static uint64_t block_subsidy(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) noexcept
{
    // Guard: quotient domain cannot increase with positive integer divisor.
    const auto halves = possible_narrow_cast<size_t>(height / subsidy_interval);
    return shift_right(initial_block_subsidy_satoshi, halves, bip42);
}

// Prevouts required.

uint64_t block::fees() const noexcept
{
    // Overflow returns max_uint64.
    const auto value = [](uint64_t total, const transaction::cptr& tx) noexcept
    {
        return ceilinged_add(total, tx->fee());
    };

    return std::accumulate(txs_->begin(), txs_->end(), uint64_t{0}, value);
}

uint64_t block::claim() const noexcept
{
    return txs_->empty() ? zero : txs_->front()->value();
}

uint64_t block::reward(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const noexcept
{
    // Overflow returns max_uint64.
    return ceilinged_add(fees(), block_subsidy(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42));
}

bool block::is_overspent(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const noexcept
{
    return claim() > reward(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42);
}

bool block::is_signature_operations_limited(bool bip16,
    bool bip141) const noexcept
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;

    // Overflow returns max_size_t.
    const auto value = [=](size_t total, const transaction::cptr& tx) noexcept
    {
        return ceilinged_add(total, tx->signature_operations(bip16, bip141));
    };

    return std::accumulate(txs_->begin(), txs_->end(), zero, value) > limit;
}

//*****************************************************************************
// CONSENSUS:
// This check is excluded under two bip30 exception blocks. This also cannot
// occur in any branch above bip34, due to height in coinbase and the
// presumption of sha256 non-collision. So this check is bypassed for both
// exception blocks and if bip34 is active (including under bip90 activation).
//*****************************************************************************
bool block::is_unspent_coinbase_collision(size_t height) const noexcept
{
    if (txs_->empty() || txs_->front()->inputs_ptr()->empty())
        return false;

    const auto& prevout = txs_->front()->inputs_ptr()->front()->prevout;

    // This requires that prevout.spent was populated for the height of the
    // validating block, otherwise a collision (unspent) must be assumed.
    return !(height > prevout->height && prevout->spent);
}

// Delegated.
// ----------------------------------------------------------------------------

code block::check_transactions() const noexcept
{
    code ec;

    for (const auto& tx: *txs_)
        if ((ec = tx->check()))
            return ec;

    return error::block_success;
}

code block::accept_transactions(const context& state) const noexcept
{
    code ec;

    for (const auto& tx: *txs_)
        if ((ec = tx->accept(state)))
            return ec;

    return error::block_success;
}

code block::connect_transactions(const context& state) const noexcept
{
    code ec;

    for (const auto& tx: *txs_)
        if ((ec = tx->connect(state)))
            return ec;

    return error::block_success;
}

// Validation.
// ----------------------------------------------------------------------------

// The block header is checked independently.
// These checks are self-contained; blockchain (and so version) independent.
code block::check() const noexcept
{
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
    uint64_t initial_subsidy) const noexcept
{
    const auto bip16 = state.is_enabled(bip16_rule);
    const auto bip30 = state.is_enabled(bip30_rule);
    const auto bip34 = state.is_enabled(bip34_rule);
    const auto bip42 = state.is_enabled(bip42_rule);
    const auto bip50 = state.is_enabled(bip50_rule);
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

code block::connect(const context& state) const noexcept
{
    return connect_transactions(state);
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

block tag_invoke(json::value_to_tag<block>,
    const json::value& value) noexcept
{
    return
    {
        json::value_to<header>(value.at("header")),
        json::value_to<chain::transactions>(value.at("transactions"))
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const block& block) noexcept
{
    value =
    {
        { "header", block.header() },
        { "transactions", *block.transactions_ptr() },
    };
}

BC_POP_WARNING()

block::cptr tag_invoke(json::value_to_tag<block::cptr>,
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<block>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const block::cptr& block) noexcept
{
    tag_invoke(tag, value, *block);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
