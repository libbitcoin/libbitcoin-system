/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Constructors.
// ----------------------------------------------------------------------------

block::block() NOEXCEPT
  : block(to_shared<chain::header>(), to_shared<chain::transaction_cptrs>(),
      false)
{
}

block::block(chain::header&& header, chain::transactions&& txs) NOEXCEPT
  : block(to_shared(std::move(header)), to_shareds(std::move(txs)), true)
{
}

block::block(const chain::header& header,
    const chain::transactions& txs) NOEXCEPT
  : block(to_shared<chain::header>(header), to_shareds(txs), true)
{
}

block::block(const chain::header::cptr& header,
    const chain::transactions_cptr& txs) NOEXCEPT
  : block(header ? header : to_shared<chain::header>(),
      txs ? txs : to_shared<transaction_cptrs>(), true)
{
}

block::block(const data_slice& data, bool witness) NOEXCEPT
  : block(stream::in::copy(data), witness)
{
}

////block::block(stream::in::fast&& stream, bool witness) NOEXCEPT
////  : block(read::bytes::fast(stream), witness)
////{
////}

block::block(stream::in::fast& stream, bool witness) NOEXCEPT
  : block(read::bytes::fast(stream), witness)
{
}

block::block(std::istream&& stream, bool witness) NOEXCEPT
  : block(read::bytes::istream(stream), witness)
{
}

block::block(std::istream& stream, bool witness) NOEXCEPT
  : block(read::bytes::istream(stream), witness)
{
}

block::block(reader&& source, bool witness) NOEXCEPT
  : block(from_data(source, witness))
{
}

block::block(reader& source, bool witness) NOEXCEPT
  : block(from_data(source, witness))
{
}

// protected
block::block(const chain::header::cptr& header,
    const chain::transactions_cptr& txs, bool valid) NOEXCEPT
  : header_(header),
    txs_(txs),
    valid_(valid),
    size_(serialized_size(*txs))
{
}

// Operators.
// ----------------------------------------------------------------------------

bool block::operator==(const block& other) const NOEXCEPT
{
    return (header_ == other.header_ || *header_ == *other.header_)
        && deep_equal(*txs_, *other.txs_);
}

bool block::operator!=(const block& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
block block::from_data(reader& source, bool witness) NOEXCEPT
{
    const auto read_transactions = [witness](reader& source) NOEXCEPT
    {
        auto txs = to_shared<transaction_cptrs>();
        const auto capacity = source.read_size(max_block_size);
        txs->reserve(capacity);

        for (size_t tx = 0; tx < capacity; ++tx)
        {
            BC_PUSH_WARNING(NO_NEW_OR_DELETE)
            txs->emplace_back(new transaction{ source, witness });
            BC_POP_WARNING()
        }

        // This is a pointer copy (non-const to const).
        return txs;
    };

    return
    {
        to_shared<chain::header>(source),
        read_transactions(source),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk block::to_data(bool witness) const NOEXCEPT
{
    data_chunk data(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void block::to_data(std::ostream& stream, bool witness) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void block::to_data(writer& sink, bool witness) const NOEXCEPT
{
    header_->to_data(sink);
    sink.write_variable(txs_->size());

    for (const auto& tx: *txs_)
        tx->to_data(sink, witness);
}

// Properties.
// ----------------------------------------------------------------------------

bool block::is_valid() const NOEXCEPT
{
    return valid_;
}

size_t block::transactions() const NOEXCEPT
{
    return txs_->size();
}

const chain::header& block::header() const NOEXCEPT
{
    return *header_;
}

const chain::header::cptr block::header_ptr() const NOEXCEPT
{
    return header_;
}

// Roll up inputs for concurrent prevout processing.
const inputs_cptr block::inputs_ptr() const NOEXCEPT
{
    const auto inputs = std::make_shared<input_cptrs>();
    const auto append_ins = [&inputs](const auto& tx) NOEXCEPT
    {
        const auto& tx_ins = *tx->inputs_ptr();
        inputs->insert(inputs->end(), tx_ins.begin(), tx_ins.end());
    };

    std::for_each(txs_->begin(), txs_->end(), append_ins);
    return inputs;
}

// vector<transaction> is not exposed (because we don't have it).
// This would require a from_shared(txs_) conversion (expensive).
const transactions_cptr& block::transactions_ptr() const NOEXCEPT
{
    return txs_;
}

hashes block::transaction_hashes(bool witness) const NOEXCEPT
{
    const auto count = txs_->size();
    const auto size = is_odd(count) && count > one ? add1(count) : count;
    hashes out(size);

    // Extra allocation for odd count optimizes for merkle root.
    // Vector capacity is never reduced when resizing to smaller size.
    out.resize(count);

    const auto hash = [witness](const auto& tx) NOEXCEPT
    {
        return tx->hash(witness);
    };

    std::transform(txs_->begin(), txs_->end(), out.begin(), hash);
    return out;
}

// computed
hash_digest block::hash() const NOEXCEPT
{
    return header_->hash();
}

// static/private
block::sizes block::serialized_size(
    const chain::transaction_cptrs& txs) NOEXCEPT
{
    sizes size{};
    std::for_each(txs.begin(), txs.end(), [&](const auto& tx) NOEXCEPT
    {
        size.nominal = ceilinged_add(size.nominal, tx->serialized_size(false));
        size.witnessed = ceilinged_add(size.witnessed, tx->serialized_size(true));
    });

    const auto common_size = ceilinged_add(
        header::serialized_size(),
        variable_size(txs.size()));

    const auto nominal_size = ceilinged_add(
        common_size,
        size.nominal);

    const auto witnessed_size = ceilinged_add(
        common_size,
        size.witnessed);

    return { nominal_size, witnessed_size };
}

size_t block::serialized_size(bool witness) const NOEXCEPT
{
    return witness ? size_.witnessed : size_.nominal;
}

// Connect.
// ----------------------------------------------------------------------------

bool block::is_empty() const NOEXCEPT
{
    return txs_->empty();
}

bool block::is_oversized() const NOEXCEPT
{
    return serialized_size(false) > max_block_size;
}

bool block::is_first_non_coinbase() const NOEXCEPT
{
    return !txs_->empty() && !txs_->front()->is_coinbase();
}

// True if there is another coinbase other than the first tx.
// No txs or coinbases returns false.
bool block::is_extra_coinbases() const NOEXCEPT
{
    if (txs_->empty())
        return false;

    const auto value = [](const auto& tx) NOEXCEPT
    {
        return tx->is_coinbase();
    };

    return std::any_of(std::next(txs_->begin()), txs_->end(), value);
}

//*****************************************************************************
// CONSENSUS: This is only necessary because satoshi stores and queries as it
// validates, imposing an otherwise unnecessary partial transaction ordering.
//*****************************************************************************
bool block::is_forward_reference() const NOEXCEPT
{
    if (txs_->empty())
        return false;

    const auto sum_txs = sub1(txs_->size());
    unordered_set_of_constant_referenced_hashes hashes{ sum_txs };
    const auto spent = [&hashes](const input::cptr& input) NOEXCEPT
    {
        return hashes.find(std::ref(input->point().hash())) != hashes.end();
    };

    const auto spend = [&spent, &hashes](const auto& tx) NOEXCEPT
    {
        const auto& ins = *tx->inputs_ptr();
        const auto forward = std::any_of(ins.begin(), ins.end(), spent);
        hashes.emplace(tx->get_hash(false));
        return forward;
    };

    return std::any_of(txs_->rbegin(), std::prev(txs_->rend()), spend);
}

// This also precludes the block merkle calculation DoS exploit by preventing
// duplicate txs, as a duplicate non-empty tx implies a duplicate point.
// bitcointalk.org/?topic=102395
bool block::is_internal_double_spend() const NOEXCEPT
{
    if (txs_->empty())
        return false;

    // Overflow returns max_size_t.
    const auto sum_ins = [](size_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx->inputs());
    };

    const auto tx1 = std::next(txs_->begin());
    const auto spends_count = std::accumulate(tx1, txs_->end(), zero, sum_ins);
    unordered_set_of_constant_referenced_points points{ spends_count };
    const auto spent = [&points](const input::cptr& in) NOEXCEPT
    {
        return !points.emplace(in->point()).second;
    };

    const auto double_spent = [&spent](const auto& tx) NOEXCEPT
    {
        const auto& ins = *tx->inputs_ptr();
        return std::any_of(ins.begin(), ins.end(), spent);
    };

    return std::any_of(tx1, txs_->end(), double_spent);
}

// private
hash_digest block::generate_merkle_root(bool witness) const NOEXCEPT
{
    return sha256::merkle_root(transaction_hashes(witness));
}

bool block::is_invalid_merkle_root() const NOEXCEPT
{
    return generate_merkle_root(false) != header_->merkle_root();
}

// Accept (contextual).
// ----------------------------------------------------------------------------

size_t block::weight() const NOEXCEPT
{
    // Block weight is 3 * nominal size * + 1 * witness size (bip141).
    return ceilinged_add(
        ceilinged_multiply(base_size_contribution, serialized_size(false)),
        ceilinged_multiply(total_size_contribution, serialized_size(true)));
}

bool block::is_overweight() const NOEXCEPT
{
    return weight() > max_block_weight;
}

bool block::is_invalid_coinbase_script(size_t height) const NOEXCEPT
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
bool block::is_hash_limit_exceeded() const NOEXCEPT
{
    if (txs_->empty())
        return false;

    // A set is used to collapse duplicates.
    unordered_set_of_constant_referenced_hashes hashes{};

    // Just the coinbase tx hash, skip its null input hashes.
    hashes.emplace(txs_->front()->get_hash(false));

    for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
    {
        // Insert the transaction hash.
        hashes.emplace((*tx)->get_hash(false));
        const auto& inputs = *(*tx)->inputs_ptr();

        // Insert all input point hashes.
        for (const auto& input: inputs)
            hashes.emplace(input->point().hash());
    }

    return hashes.size() > hash_limit;
}

bool block::is_malleable() const NOEXCEPT
{
    return is_malleable64() || is_malleable32();
}

bool block::is_malleable32() const NOEXCEPT
{
    const auto unmalleated = txs_->size();
    for (auto mally = one; mally <= unmalleated; mally *= two)
        if (is_malleable32(unmalleated, mally))
            return true;

    return false;
}

bool block::is_malleated32() const NOEXCEPT
{
    return !is_zero(malleated32_size());
}

// protected
// The size of an actual malleation of this block, or zero.
size_t block::malleated32_size() const NOEXCEPT
{
    const auto malleated = txs_->size();
    for (auto mally = one; mally <= to_half(malleated); mally *= two)
        if (is_malleable32(malleated - mally, mally) && is_malleated32(mally))
            return mally;

    return zero;
}

// protected
// True if the last width set of tx hashes repeats.
bool block::is_malleated32(size_t width) const NOEXCEPT
{
    const auto malleated = txs_->size();
    if (is_zero(width) || width > to_half(malleated))
        return false;

    auto mally = txs_->rbegin();
    auto legit = std::next(mally, width);
    while (!is_zero(width--))
        if ((*mally++)->hash(false) != (*legit++)->hash(false))
            return false;

    return true;
}

bool block::is_malleable64() const NOEXCEPT
{
    return is_malleable64(*txs_);
}

// static
// If all non-witness tx serializations are 64 bytes the id is malleable.
// This form of malleability does not imply current block instance is invalid.
bool block::is_malleable64(const transaction_cptrs& txs) NOEXCEPT
{
    const auto two_leaves = [](const auto& tx) NOEXCEPT
    {
        return tx->serialized_size(false) == two * hash_size;
    };

    return !txs.empty() && std::all_of(txs.begin(), txs.end(), two_leaves);
}

bool block::is_segregated() const NOEXCEPT
{
    const auto segregated = [](const auto& tx) NOEXCEPT
    {
        return tx->is_segregated();
    };

    return std::any_of(txs_->begin(), txs_->end(), segregated);
}

bool block::is_invalid_witness_commitment() const NOEXCEPT
{
    if (txs_->empty())
        return false;

    const auto& coinbase = *txs_->front();
    if (coinbase.inputs_ptr()->empty())
        return false;

    // If there is a valid commitment, return false (valid).
    // Last output of commitment pattern holds committed value (bip141).
    hash_digest reserved{}, committed{};
    if (coinbase.inputs_ptr()->front()->reserved_hash(reserved))
        for (const auto& output: views_reverse(*coinbase.outputs_ptr()))
            if (output->committed_hash(committed))
                if (committed == sha256::double_hash(
                    generate_merkle_root(true), reserved))
                    return false;
    
    // If no valid commitment, return true (invalid) if segregated.
    // If no block tx has witness data the commitment is optional (bip141).
    return is_segregated();
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
    uint64_t initial_block_subsidy_satoshi, bool bip42) NOEXCEPT
{
    // Guard: quotient domain cannot increase with positive integer divisor.
    const auto halves = possible_narrow_cast<size_t>(height / subsidy_interval);
    return shift_right(initial_block_subsidy_satoshi, halves, bip42);
}

// Prevouts required.

uint64_t block::fees() const NOEXCEPT
{
    // Overflow returns max_uint64.
    const auto value = [](uint64_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx->fee());
    };

    return std::accumulate(txs_->begin(), txs_->end(), uint64_t{0}, value);
}

uint64_t block::claim() const NOEXCEPT
{
    return txs_->empty() ? zero : txs_->front()->value();
}

uint64_t block::reward(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT
{
    // Overflow returns max_uint64.
    return ceilinged_add(fees(), block_subsidy(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42));
}

bool block::is_overspent(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT
{
    return claim() > reward(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42);
}

size_t block::signature_operations(bool bip16, bool bip141) const NOEXCEPT
{
    // Overflow returns max_size_t.
    const auto value = [=](size_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx->signature_operations(bip16, bip141));
    };

    return std::accumulate(txs_->begin(), txs_->end(), zero, value);
}

bool block::is_signature_operations_limited(bool bip16,
    bool bip141) const NOEXCEPT
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;
    return signature_operations(bip16, bip141) > limit;
}

//*****************************************************************************
// CONSENSUS:
// This check is excluded under two bip30 exception blocks and bip30_deactivate
// until bip30_reactivate. These conditions are rolled up into the bip30 flag.
//*****************************************************************************
bool block::is_unspent_coinbase_collision() const NOEXCEPT
{
    if (txs_->empty() || txs_->front()->inputs_ptr()->empty())
        return false;

    // May only commit duplicate coinbase that is already confirmed spent.
    // Metadata population defaults coinbase to spent (not a collision).
    return !txs_->front()->inputs_ptr()->front()->metadata.spent;
}

// Search is not ordered, forward references are caught by block.check.
void block::populate() const NOEXCEPT
{
    std::unordered_map<point, output::cptr> points{};
    uint32_t index{};

    // Populate outputs hash table.
    for (auto tx = txs_->begin(); tx != txs_->end(); ++tx, index = 0)
        for (const auto& out: *(*tx)->outputs_ptr())
            points.emplace(std::pair{ point{ (*tx)->hash(false), index++ },
                out });

    // Populate input prevouts from hash table.
    for (auto tx = txs_->begin(); tx != txs_->end(); ++tx)
    {
        for (const auto& in: *(*tx)->inputs_ptr())
        {
            const auto point = points.find(in->point());
            if (point != points.end())
                in->prevout = point->second;
        }
    }
}

// Delegated.
// ----------------------------------------------------------------------------

// DO invoke on coinbase.
code block::check_transactions() const NOEXCEPT
{
    code ec;
    
    for (const auto& tx: *txs_)
        if ((ec = tx->check()))
            return ec;

    return error::block_success;
}

// DO invoke on coinbase.
code block::check_transactions(const context& ctx) const NOEXCEPT
{
    code ec;
    
    for (const auto& tx: *txs_)
        if ((ec = tx->check(ctx)))
            return ec;

    return error::block_success;
}

// Do NOT invoke on coinbase.
code block::accept_transactions(const context& ctx) const NOEXCEPT
{
    code ec;

    if (!is_empty())
        for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
            if ((ec = (*tx)->accept(ctx)))
                return ec;

    return error::block_success;
}

// Do NOT invoke on coinbase.
code block::connect_transactions(const context& ctx) const NOEXCEPT
{
    code ec;

    if (!is_empty())
        for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
            if ((ec = (*tx)->connect(ctx)))
                return ec;

    return error::block_success;
}

// Do NOT invoke on coinbase.
code block::confirm_transactions(const context& ctx) const NOEXCEPT
{
    code ec;

    if (!is_empty())
        for (auto tx = std::next(txs_->begin()); tx != txs_->end(); ++tx)
            if ((ec = (*tx)->confirm(ctx)))
                return ec;

    return error::block_success;
}

// Validation.
// ----------------------------------------------------------------------------
// The block header is checked/accepted independently.

// context free.
// TODO: use of get_hash() in is_forward_reference makes this thread unsafe.
code block::check(bool bypass) const NOEXCEPT
{
    if (bypass)
    {
        // type32_malleated is subset of is_internal_double_spend, assuming
        // otherwise valid txs, as that will catch any duplicated transaction.
        if (is_invalid_merkle_root())
            return error::merkle_mismatch;
        if (is_malleated32())
            return error::type32_malleated;

        return error::block_success;
    }

    // empty_block is redundant with first_not_coinbase.
    //if (is_empty())
    //    return error::empty_block;
    if (is_oversized())
        return error::block_size_limit;
    if (is_first_non_coinbase())
        return error::first_not_coinbase;
    if (is_extra_coinbases())
        return error::extra_coinbases;
    if (is_forward_reference())
        return error::forward_reference;
    if (is_internal_double_spend())
        return error::block_internal_double_spend;
    if (is_invalid_merkle_root())
        return error::merkle_mismatch;

    return check_transactions();
}

// forks
// height
// timestamp
// median_time_past

// context required.
// TODO: use of get_hash() in is_hash_limit_exceeded makes this thread unsafe.
code block::check(const context& ctx, bool bypass) const NOEXCEPT
{
    if (bypass)
    {
        const auto bip141 = ctx.is_enabled(bip141_rule);

        if (bip141 && is_invalid_witness_commitment())
            return error::invalid_witness_commitment;
        
        return error::block_success;
    }

    const auto bip34 = ctx.is_enabled(bip34_rule);
    const auto bip50 = ctx.is_enabled(bip50_rule);
    const auto bip141 = ctx.is_enabled(bip141_rule);

    if (bip141 && is_overweight())
        return error::block_weight_limit;
    if (bip34 && is_invalid_coinbase_script(ctx.height))
        return error::coinbase_height_mismatch;
    if (bip50 && is_hash_limit_exceeded())
        return error::temporary_hash_limit;
    if (bip141 && is_invalid_witness_commitment())
        return error::invalid_witness_commitment;

    return check_transactions(ctx);
}

// forks
// height

// This assumes that prevout caching is completed on all inputs.
code block::accept(const context& ctx, size_t subsidy_interval,
    uint64_t initial_subsidy) const NOEXCEPT
{
    const auto bip16 = ctx.is_enabled(bip16_rule);
    const auto bip42 = ctx.is_enabled(bip42_rule);
    const auto bip141 = ctx.is_enabled(bip141_rule);

    // prevouts required.
    if (is_overspent(ctx.height, subsidy_interval, initial_subsidy, bip42))
        return error::coinbase_value_limit;
    if (is_signature_operations_limited(bip16, bip141))
        return error::block_sigop_limit;

    return accept_transactions(ctx);
}

// forks

// Node performs these checks through database query.
// This assumes that prevout and metadata caching are completed on all inputs.
code block::confirm(const context& ctx) const NOEXCEPT
{
    const auto bip30 = ctx.is_enabled(bip30_rule);

    if (bip30 && is_unspent_coinbase_collision())
        return error::unspent_coinbase_collision;

    return confirm_transactions(ctx);
}

// forks

code block::connect(const context& ctx) const NOEXCEPT
{
    return connect_transactions(ctx);
}

BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

block tag_invoke(json::value_to_tag<block>,
    const json::value& value) NOEXCEPT
{
    return
    {
        json::value_to<header>(value.at("header")),
        json::value_to<chain::transactions>(value.at("transactions"))
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const block& block) NOEXCEPT
{
    value =
    {
        { "header", block.header() },
        { "transactions", *block.transactions_ptr() },
    };
}

BC_POP_WARNING()

block::cptr tag_invoke(json::value_to_tag<block::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<block>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const block::cptr& block) NOEXCEPT
{
    tag_invoke(tag, value, *block);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
