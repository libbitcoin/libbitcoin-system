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
#include <bitcoin/system/chain/transaction.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
//-----------------------------------------------------------------------------

transaction::transaction()
  : transaction(0, 0, {}, {}, false)
{
}

transaction::transaction(transaction&& other)
  : transaction(
      other.version_,
      other.locktime_,
      std::move(other.inputs_),
      std::move(other.outputs_),
      other.valid_)
{
}

transaction::transaction(const transaction& other)
  : transaction(
      other.version_,
      other.locktime_,
      other.inputs_,
      other.outputs_,
      other.valid_)
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    input::list&& inputs, output::list&& outputs)
  : transaction(version, locktime, std::move(inputs), std::move(outputs),
      true)
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    const input::list& inputs, const output::list& outputs)
  : transaction(version, locktime, inputs, outputs, true)
{
}

transaction::transaction(const data_chunk& data, bool witness)
{
    from_data(data, witness);
}

transaction::transaction(std::istream& stream, bool witness)
{
    from_data(stream, witness);
}

transaction::transaction(reader& source, bool witness)
{
    from_data(source, witness);
}

// protected
transaction::transaction(uint32_t version, uint32_t locktime,
    input::list&& inputs, output::list&& outputs, bool valid)
  : version_(version),
    locktime_(locktime),
    inputs_(std::move(inputs)),
    outputs_(std::move(outputs)),
    valid_(valid)
{
}

// protected
transaction::transaction(uint32_t version, uint32_t locktime,
    const input::list& inputs, const output::list& outputs, bool valid)
  : version_(version),
    locktime_(locktime),
    inputs_(inputs),
    outputs_(outputs),
    valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

transaction& transaction::operator=(transaction&& other)
{
    version_ = other.version_;
    locktime_ = other.locktime_;
    inputs_ = std::move(other.inputs_);
    outputs_ = std::move(other.outputs_);
    valid_ = other.valid_;
    return *this;
}

// This can be expensive, try to avoid.
transaction& transaction::operator=(const transaction& other)
{
    version_ = other.version_;
    locktime_ = other.locktime_;
    inputs_ = other.inputs_;
    outputs_ = other.outputs_;
    valid_ = other.valid_;
    return *this;
}

bool transaction::operator==(const transaction& other) const
{
    return (version_ == other.version_)
        && (locktime_ == other.locktime_)
        && (inputs_ == other.inputs_)
        && (outputs_ == other.outputs_);
}

bool transaction::operator!=(const transaction& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// Read a length-prefixed collection of inputs or outputs from the source.
template<class Source, class Put>
bool read(Source& source, std::vector<Put>& puts)
{
    const auto count = source.read_size();

    // Guard against potential for arbitrary memory allocation.
    if (count > max_block_size)
    {
        source.invalidate();
    }
    else
    {
        puts.reserve(count);
        for (size_t put = 0; put < count; ++put)
            puts.emplace_back(source);
    }

    return source;
}

bool transaction::from_data(const data_chunk& data, bool witness)
{
    stream::in::copy istream(data);
    return from_data(istream, witness);
}

bool transaction::from_data(std::istream& stream, bool witness)
{
    read::bytes::istream source(stream);
    return from_data(source, witness);
}

bool transaction::from_data(reader& source, bool witness)
{
    reset();

    version_ = source.read_4_bytes_little_endian();
    read(source, inputs_);

    // Detect witness as no inputs (marker) and expected flag (bip144).
    if (inputs_.size() == witness_marker &&
        source.peek_byte() == witness_enabled)
    {
        // Skip over the peeked witness flag.
        source.skip_byte();
        read(source, inputs_);
        read(source, outputs_);
        for (auto& input: inputs_)
            input.witness_ = witness::witness(source, true);
    }
    else
    {
        read(source, outputs_);
    }

    locktime_ = source.read_4_bytes_little_endian();

    if (!witness)
        strip_witness();

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// protected
void transaction::reset()
{
    version_ = 0;
    locktime_ = 0;
    inputs_.clear();
    inputs_.shrink_to_fit();
    outputs_.clear();
    outputs_.shrink_to_fit();
    valid_ = false;
}

bool transaction::is_valid() const
{
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

// Write a length-prefixed collection of inputs or outputs to the sink.
template<class Sink, class Put>
void write(Sink& sink, const std::vector<Put>& puts)
{
    sink.write_variable(puts.size());

    for (const auto& put: puts)
        put.to_data(sink);
}

// Transactions with empty witnesses always use old serialization (bip144).
// If no inputs are witness programs then witness hash is tx hash (bip141).
data_chunk transaction::to_data(bool witness) const
{
    witness &= is_segregated();

    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void transaction::to_data(std::ostream& stream, bool witness) const
{
    witness &= is_segregated();

    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void transaction::to_data(writer& sink, bool witness) const
{
    DEBUG_ONLY(const auto size = serialized_size(witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    witness &= is_segregated();

    sink.write_4_bytes_little_endian(version_);

    if (witness)
    {
        sink.write_byte(witness_marker);
        sink.write_byte(witness_enabled);
    }

    write(sink, inputs_);
    write(sink, outputs_);

    if (witness)
        for (auto& input: inputs_)
            input.witness().to_data(sink, true);

    sink.write_4_bytes_little_endian(locktime_);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

// Size.
//-----------------------------------------------------------------------------

// static
size_t transaction::maximum_size(bool is_coinbase)
{
    // TODO: find smallest spendable coinbase tx with maximal input script.
    // This is not consensus critical but if too small is a disk fill vector.
    static const auto min_coinbase_tx = 1024;

    static const auto max_coinbase_tx = max_block_size - (sizeof(uint32_t) +
        header::satoshi_fixed_size());

    // A pool (non-coinbase) tx must fit into a block with at least a coinbase.
    return is_coinbase ? max_coinbase_tx : max_coinbase_tx - min_coinbase_tx;
}

size_t transaction::serialized_size(bool witness) const
{
    witness &= is_segregated();

    const auto ins = [=](size_t size, const input& input)
    {
        return size + input.serialized_size(witness);
    };

    const auto outs = [](size_t size, const output& output)
    {
        return size + output.serialized_size();
    };

    return sizeof(version_)
        + (witness ? sizeof(witness_marker) : zero)
        + (witness ? sizeof(witness_enabled) : zero)
        + variable_size(inputs_.size())
        + std::accumulate(inputs_.begin(), inputs_.end(), zero, ins)
        + variable_size(outputs_.size())
        + std::accumulate(outputs_.begin(), outputs_.end(), zero, outs)
        + sizeof(locktime_);
}

// Accessors.
//-----------------------------------------------------------------------------

uint32_t transaction::version() const
{
    return version_;
}

uint32_t transaction::locktime() const
{
    return locktime_;
}

const input::list& transaction::inputs() const
{
    return inputs_;
}

const output::list& transaction::outputs() const
{
    return outputs_;
}

// Cache.
//-----------------------------------------------------------------------------

hash_digest transaction::hash(bool witness) const
{
    // Witness hashing must be disabled for non-segregated txs.
    witness &= is_segregated();

    // Witness coinbase tx hash is assumed to be null_hash (bip141).
    return witness && is_coinbase() ? null_hash :
        bitcoin_hash(to_data(witness));
}

hash_digest transaction::outputs_hash() const
{
    const auto size = std::accumulate(outputs().begin(), outputs().end(), zero,
        [&](size_t total, const output& output)
        {
            return total + output.serialized_size();
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);
    for (const auto& output: outputs())
        output.to_data(writer);

    ////BITCOIN_ASSERT(writer && writer.get_position() == size);
    return bitcoin_hash(data);
}

hash_digest transaction::points_hash() const
{
    const auto size = std::accumulate(inputs().begin(), inputs().end(), zero,
        [&](size_t total, const input& input)
        {
            return total + input.point().serialized_size();
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);

    for (const auto& input : inputs())
        input.point().to_data(writer);

    ////BITCOIN_ASSERT(riter && writer.get_position() == size);
    return bitcoin_hash(data);
}

hash_digest transaction::sequences_hash() const
{
    const auto size = std::accumulate(inputs().begin(), inputs().end(), zero,
        [&](size_t total, const input& /* input */)
        {
            return total + sizeof(uint32_t);
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);

    for (const auto& input: inputs())
        writer.write_4_bytes_little_endian(input.sequence());

    ////BITCOIN_ASSERT(writer && writer.get_position() == size);
    return bitcoin_hash(data);
}

// Utilities.
//-----------------------------------------------------------------------------

// Clear witness from all inputs (does not change default transaction hash).
void transaction::strip_witness()
{
    const auto strip = [](input& input)
    {
        input.strip_witness();
    };

    std::for_each(inputs_.begin(), inputs_.end(), strip);
}

// Validation helpers.
//-----------------------------------------------------------------------------

bool transaction::is_coinbase() const
{
    return inputs_.size() == 1 && inputs_.front().point().is_null();
}

// True if coinbase and has invalid input[0] script size.
bool transaction::is_oversized_coinbase() const
{
    if (!is_coinbase())
        return false;

    const auto script_size = inputs_.front().script().serialized_size(false);
    return script_size < min_coinbase_size || script_size > max_coinbase_size;
}

// True if not coinbase but has null previous_output(s).
bool transaction::is_null_non_coinbase() const
{
    if (is_coinbase())
        return false;

    const auto invalid = [](const input& input)
    {
        return input.point().is_null();
    };

    return std::any_of(inputs_.begin(), inputs_.end(), invalid);
}


static bool all_final(const input::list& inputs)
{
    const auto finalized = [](const input& input)
    {
        return input.is_final();
    };

    return std::all_of(inputs.begin(), inputs.end(), finalized);
}

bool transaction::is_final(size_t block_height, uint32_t block_time) const
{
    const auto max_locktime = [=]()
    {
        return locktime_ < locktime_threshold ?
            safe_cast<uint32_t>(block_height) : block_time;
    };

    return is_zero(locktime_) || locktime_ < max_locktime() ||
        all_final(inputs_);
}

// This is not a consensus rule, just detection of an irrational use.
bool transaction::is_locktime_conflict() const
{
    return !is_zero(locktime_) && all_final(inputs_);
}

// Overflow returns max_uint64.
uint64_t transaction::total_output_value() const
{
    const auto sum = [](uint64_t total, const output& output)
    {
        return ceilinged_add(total, output.value());
    };

    return std::accumulate(outputs_.begin(), outputs_.end(), uint64_t(0), sum);
}

size_t transaction::weight() const
{
    // Block weight is 3 * Base size * + 1 * Total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

point::list transaction::previous_outputs() const
{
    static default_allocator<point> no_fill_allocator{};
    point::list out(no_fill_allocator);
    out.resize(inputs_.size());

    const auto prevout = [](const input& input)
    {
        return input.point();
    };

    std::transform(inputs_.begin(), inputs_.end(), out.begin(), prevout);
    return out;
}

bool transaction::is_internal_double_spend() const
{
    return !is_distinct(previous_outputs());
}

bool transaction::is_dusty(uint64_t minimum_output_value) const
{
    const auto dust = [minimum_output_value](const output& output)
    {
        return output.is_dust(minimum_output_value);
    };

    return std::any_of(outputs_.begin(), outputs_.end(), dust);
}

bool transaction::is_segregated() const
{
    const auto segregated = [](const input& input)
    {
        return input.is_segregated();
    };

    // If no block tx has witness data the commitment is optional (bip141).
    return std::any_of(inputs_.begin(), inputs_.end(), segregated);
}

////// Coinbase transactions return success, to simplify iteration.
////code transaction::connect_input(const chain_state& state,
////    size_t input_index) const
////{
////    if (input_index >= inputs_.size())
////        return error::inputs_overflow;
////
////    if (is_coinbase())
////        return error::transaction_success;
////
////    const auto& prevout = inputs_[input_index].point().prevout;
////
////    // Verify that the previous output cache has been populated.
////    if (!prevout)
////        return error::missing_previous_output;
////
////    const auto forks = state.forks();
////    const auto index32 = static_cast<uint32_t>(input_index);
////
////    // Verify the transaction input script against the previous output.
////    return script::verify(*this, index32, forks);
////}

// Validation.
//-----------------------------------------------------------------------------

// These checks are self-contained; blockchain (and so version) independent.
code transaction::check(uint64_t max_money, bool transaction_pool) const
{
    if (inputs_.empty() || outputs_.empty())
        return error::empty_transaction;

    else if (is_null_non_coinbase())
        return error::previous_output_null;

    else if (total_output_value() > max_money)
        return error::spend_overflow;

    else if (!transaction_pool && is_oversized_coinbase())
        return error::invalid_coinbase_script_size;

    else if (transaction_pool && is_coinbase())
        return error::coinbase_transaction;

    // This is redundant when executing within a block object.
    // However, must be checked if accepting block transactions before blocks.
    else if (transaction_pool && is_internal_double_spend())
        return error::transaction_internal_double_spend;

    // TODO: reduce by header, txcount and smallest coinbase size for height.
    else if (transaction_pool && serialized_size(false) >=
        maximum_size(false))
        return error::transaction_size_limit;

    // We cannot know if bip16/bip141 is enabled here so we do not check it.
    // This will not make a difference unless prevouts are populated, in which
    // case they are ignored. This means that p2sh sigops are not counted here.
    // This is a preliminary check, the final count must come from accept().
    // Reenable once sigop caching is implemented, otherwise is deoptimization.
    ////else if (transaction_pool &&
    ////    signature_operations(false, false) > max_block_sigops)
    ////    return error::transaction_legacy_sigop_limit;

    else
        return error::transaction_success;
}

// These checks assume that prevout caching is completed on all tx.inputs.
code transaction::accept(const chain_state& state, bool transaction_pool) const
{
    const auto bip16 = state.is_enabled(forks::bip16_rule);
    const auto bip68 = state.is_enabled(forks::bip68_rule);
    const auto bip141 = state.is_enabled(forks::bip141_rule);

    // Segwit sigops are discounted by increasing limit and legacy weight.
    const auto max_sigops = bip141 ? max_fast_sigops : max_block_sigops;

    if (transaction_pool && state.is_under_checkpoint())
        return error::premature_validation;

    // This is deceptive, as number of inputs deserialized is zero, but this is
    // only a serialization distinction. The transaction becomes constructed
    // with inputs for consensus purposes. Old p2p nodes will reject the tx.
    // A segregated tx should appear empty if bip141 is not enabled.
    if (!bip141 && is_segregated())
        return error::empty_transaction;

    if (transaction_pool && !is_final(state.height(), state.median_time_past()))
        return error::transaction_non_final;

    // TODO: currently tautological, as maximum_transaction_version is max_uint32.
    if (transaction_pool && version() > state.maximum_transaction_version())
        return error::transaction_version;

    // requires prevouts
    ////else if (is_missing_previous_outputs())
    ////    return error::missing_previous_output;

    // requires prevouts
    ////else if (is_confirmed_double_spend())
    ////    return error::double_spend;

    // requires prevouts
    // TODO: rename to is_spent_coinbase_mature/error::imature_coinbase (?)
    // This relates height to maturity of spent coinbase.
    // Since reorganization may decrease height, this is not cache safe.
    // A decrease in validated height invalidate validation cache.
    ////else if (!is_mature(state.height()))
    ////    return error::coinbase_maturity;

    // requires prevouts
    ////else if (is_overspent())
    ////    return error::spend_exceeds_value;

    // requires prevouts
    ////else if (bip68 && is_locked(state.height(), state.median_time_past()))
    ////    return error::sequence_locked;

    // requires prevouts
    // This recomputes sigops to include (if bip16 is true) p2sh from prevouts.
    ////else if (transaction_pool && signature_operations(bip16, bip141) > max_sigops)
    ////    return error::transaction_embedded_sigop_limit;

    // This causes second serialized_size(true, false) computation (uncached).
    // TODO: reduce by header, txcount and smallest coinbase size for height.
    else if (transaction_pool && bip141 && weight() > max_block_weight)
        return error::transaction_weight_limit;

    else
        return error::transaction_success;
}

////code transaction::connect(const chain_state& state) const
////{
////    code ec;
////
////    for (size_t input = 0; input < inputs_.size(); ++input)
////        if ((ec = connect_input(state, input)))
////            return ec;
////
////    return error::transaction_success;
////}

} // namespace chain
} // namespace system
} // namespace libbitcoin
