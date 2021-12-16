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
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/optional.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/context.hpp>
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
// ----------------------------------------------------------------------------

transaction::transaction()
  : transaction(
      false, 0, 0,
      to_shared<input_ptrs>(),
      to_shared<output_ptrs>(),
      false)
{
}

transaction::transaction(transaction&& other)
  : transaction(other)
{
}

transaction::transaction(const transaction& other)
  : transaction(
      other.segregated_,
      other.version_,
      other.locktime_,
      other.inputs_,
      other.outputs_,
      other.valid_)
{
}

// segregated_ is the first property because of this constructor.
transaction::transaction(uint32_t version, uint32_t locktime,
    chain::inputs&& inputs, chain::outputs&& outputs)
  : transaction(segregated(inputs), version, locktime,
      to_shareds(std::move(inputs)), to_shareds(std::move(outputs)), true)
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    const chain::inputs& inputs, const chain::outputs& outputs)
  : transaction(segregated(inputs), version, locktime,
      to_shareds(inputs), to_shareds(outputs), true)
{
}

transaction::transaction(uint32_t version, uint32_t locktime,
    const inputs_ptr& inputs, const outputs_ptr& outputs)
  : transaction(segregated(inputs), version, locktime, inputs, outputs, true)
{
}

transaction::transaction(const data_slice& data, bool witness)
  : transaction(stream::in::copy(data), witness)
{
}

transaction::transaction(std::istream& stream, bool witness)
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(reader& source, bool witness)
  : transaction(from_data(source, witness))
{
}

// protected
transaction::transaction(bool segregated, uint32_t version, uint32_t locktime,
    const inputs_ptr& inputs, const outputs_ptr& outputs, bool valid)
  : version_(version),
    locktime_(locktime),
    inputs_(inputs ? inputs : to_shared<input_ptrs>()),
    outputs_(outputs ? outputs : to_shared<output_ptrs>()),
    segregated_(segregated),
    valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

transaction& transaction::operator=(transaction&& other)
{
    *this = other;
    return *this;
}

transaction& transaction::operator=(const transaction& other)
{
    version_ = other.version_;
    locktime_ = other.locktime_;
    inputs_ = other.inputs_;
    outputs_ = other.outputs_;
    segregated_ = other.segregated_;
    valid_ = other.valid_;
    return *this;
}

bool transaction::operator==(const transaction& other) const
{
    // Compares input/output elements, not pointers.
    return (version_ == other.version_)
        && (locktime_ == other.locktime_)
        && equal_points(*inputs_, *other.inputs_)
        && equal_points(*outputs_, *other.outputs_);
}

bool transaction::operator!=(const transaction& other) const
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

template<class Put, class Source>
std::shared_ptr<std::vector<std::shared_ptr<const Put>>> read_puts(Source& source)
{
    auto puts = to_shared<std::vector<std::shared_ptr<const Put>>>();
    puts->reserve(source.read_size(max_block_size));

    for (auto put = zero; put < puts->capacity(); ++put)
        puts->emplace_back(new Put{ source });

    return puts;
}

// static/private
transaction transaction::from_data(reader& source, bool witness)
{
    const auto version = source.read_4_bytes_little_endian();

    // Inputs must be non-const so that they may assign the witness.
    auto inputs = read_puts<chain::input>(source);
    outputs_ptr outputs;

    // Expensive repeated recomputation, so cache segregated state.
    const auto segregated =
        inputs->size() == witness_marker &&
        source.peek_byte() == witness_enabled;

    // Detect witness as no inputs (marker) and expected flag (bip144).
    if (segregated)
    {
        // Skip over the peeked witness flag.
        source.skip_byte();

        // Inputs and outputs are constructed on a vector of const pointers.
        inputs = read_puts<input>(source);
        outputs = read_puts<output>(source);

        // Read or skip witnesses as specified.
        for (auto& input: *inputs)
        {
            if (witness)
            {
                // Safe to cast as this method exclusively owns the input and
                // input::witness_ a mutable public property of the instance.
                const auto setter = const_cast<chain::input*>(input.get());
                setter->witness_ = to_shared(new chain::witness{ source, true });
            }
            else
            {
                source.skip_bytes(input->witness().serialized_size(true));
            }
        }
    }
    else
    {
        // Default witness is populated on input construct.
        outputs = read_puts<const chain::output>(source);
    }

    // TODO: move locktime_ after outputs in member/construction order.
    // TODO: will make it consistent with serialization order, as all others.
    const auto locktime = source.read_4_bytes_little_endian();

    // to_const is overhead, creating an extra shared pointer per input.
    return { segregated, version, locktime, inputs, outputs, source };
}

// Serialization.
// ----------------------------------------------------------------------------

// Transactions with empty witnesses always use old serialization (bip144).
// If no inputs are witness programs then witness hash is tx hash (bip141).
data_chunk transaction::to_data(bool witness) const
{
    witness &= segregated_;

    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void transaction::to_data(std::ostream& stream, bool witness) const
{
    witness &= segregated_;

    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void transaction::to_data(writer& sink, bool witness) const
{
    DEBUG_ONLY(const auto bytes = serialized_size(witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    witness &= segregated_;

    sink.write_4_bytes_little_endian(version_);

    if (witness)
    {
        sink.write_byte(witness_marker);
        sink.write_byte(witness_enabled);
    }

    sink.write_variable(inputs_->size());
    for (const auto& input: *inputs_)
        input->to_data(sink);

    sink.write_variable(outputs_->size());
    for (const auto& input: *outputs_)
        input->to_data(sink);

    if (witness)
        for (auto& input: *inputs_)
            input->witness().to_data(sink, true);

    sink.write_4_bytes_little_endian(locktime_);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t transaction::serialized_size(bool witness) const
{
    witness &= segregated_;

    const auto ins = [=](size_t total, const input::ptr& input)
    {
        // Inputs account for witness bytes. 
        return total + input->serialized_size(witness);
    };

    const auto outs = [](size_t total, const output::ptr& output)
    {
        return total + output->serialized_size();
    };

    return sizeof(version_)
        + (witness ? sizeof(witness_marker) + sizeof(witness_enabled) : zero)
        + variable_size(inputs_->size())
        + std::accumulate(inputs_->begin(), inputs_->end(), zero, ins)
        + variable_size(outputs_->size())
        + std::accumulate(outputs_->begin(), outputs_->end(), zero, outs)
        + sizeof(locktime_);
}

// Properties.
// ----------------------------------------------------------------------------

bool transaction::is_valid() const
{
    return valid_;
}

uint32_t transaction::version() const
{
    return version_;
}

uint32_t transaction::locktime() const
{
    return locktime_;
}

const inputs_ptr transaction::inputs() const
{
    return inputs_;
}

const outputs_ptr transaction::outputs() const
{
    return outputs_;
}

uint64_t transaction::fee() const
{
    // Underflow returns zero (and is_overspent() will be true).
    // This is value of prevouts spent by inputs minus that claimed by outputs.
    return floored_subtract(value(), claim());
}

hash_digest transaction::hash(bool witness) const
{
    // Witness coinbase tx hash is assumed to be null_hash (bip141).
    return witness && segregated_ && is_coinbase() ? null_hash :
        bitcoin_hash(to_data(witness));
}

// Methods.
// ----------------------------------------------------------------------------

bool transaction::is_dusty(uint64_t minimum_output_value) const
{
    const auto dusty = [=](const output::ptr& output)
    {
        return output->is_dust(minimum_output_value);
    };

    return std::any_of(outputs_->begin(), outputs_->end(), dusty);
}

size_t transaction::signature_operations(bool bip16, bool bip141) const
{
    // Includes BIP16 p2sh additional sigops, max_size_t if prevout invalid.
    const auto in = [=](size_t total, const input::ptr& input)
    {
        return ceilinged_add(total, input->signature_operations(bip16, bip141));
    };

    const auto out = [=](size_t total, const output::ptr& output)
    {
        return ceilinged_add(total, output->signature_operations(bip141));
    };

    // Overflow returns max_size_t.
    return std::accumulate(inputs_->begin(), inputs_->end(), zero, in) +
        std::accumulate(outputs_->begin(), outputs_->end(), zero, out);
}

chain::points transaction::points() const
{
    static default_allocator<point> no_fill_allocator{};
    chain::points out(no_fill_allocator);
    out.resize(inputs_->size());

    const auto point = [](const input::ptr& input)
    {
        return input->point();
    };

    std::transform(inputs_->begin(), inputs_->end(), out.begin(), point);
    return out;
}

hash_digest transaction::outputs_hash() const
{
    const auto size = std::accumulate(outputs()->begin(), outputs()->end(), zero,
        [&](size_t total, const output::ptr& output)
        {
            return total + output->serialized_size();
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);
    for (const auto& output: *outputs())
        output->to_data(writer);

    BITCOIN_ASSERT(writer && writer.get_position() == size);
    return bitcoin_hash(data);
}

hash_digest transaction::points_hash() const
{
    const auto size = std::accumulate(inputs()->begin(), inputs()->end(), zero,
        [&](size_t total, const input::ptr& input)
        {
            return total + input->point().serialized_size();
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);

    for (const auto& input: *inputs())
        input->point().to_data(writer);

    BITCOIN_ASSERT(writer && writer.get_position() == size);
    return bitcoin_hash(data);
}

hash_digest transaction::sequences_hash() const
{
    const auto size = std::accumulate(inputs()->begin(), inputs()->end(), zero,
        [&](size_t total, const input::ptr& input)
        {
            return total + sizeof(input->sequence());
        });

    data_chunk data(no_fill_byte_allocator);
    data.resize(size);
    write::bytes::copy writer(data);

    for (const auto& input: *inputs())
        writer.write_4_bytes_little_endian(input->sequence());

    BITCOIN_ASSERT(writer && writer.get_position() == size);
    return bitcoin_hash(data);
}

// Guard (context free).
// ----------------------------------------------------------------------------

bool transaction::is_coinbase() const
{
    return inputs_->size() == one && inputs_->front()->point().is_null();
}

bool transaction::is_internal_double_spend() const
{
    return !is_distinct(points());
}

// TODO: a pool (non-coinbase) tx must fit into a block (with a coinbase).
bool transaction::is_oversized() const
{
    return serialized_size(false) > max_block_size;
}

// Guard (contextual).
// ----------------------------------------------------------------------------

// static/private
bool transaction::segregated(const chain::inputs& inputs)
{
    const auto witnessed = [](const input& input)
    {
        return !input.witness().stack().empty();
    };

    return std::any_of(inputs.begin(), inputs.end(), witnessed);
}

// static/private
bool transaction::segregated(const inputs_ptr& inputs)
{
    if (!inputs)
        return false;

    const auto witnessed = [](const input::ptr& input)
    {
        return !input->witness().stack().empty();
    };

    return std::any_of(inputs->begin(), inputs->end(), witnessed);
}

bool transaction::is_segregated() const
{
    return segregated_;
}

size_t transaction::weight() const
{
    // Block weight is 3 * base size * + 1 * total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

bool transaction::is_overweight() const
{
    return weight() > max_block_weight;
}

//*****************************************************************************
// CONSENSUS: Legacy sigops are counted in coinbase scripts despite the fact
// that coinbase input scripts are never executed. There is no need to exclude
// p2sh coinbase sigops since there is never a script to count.
//*****************************************************************************
bool transaction::is_signature_operations_limit(bool bip16,
    bool bip141) const
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;
    return signature_operations(bip16, bip141) > limit;
}

// Check (context free).
// ----------------------------------------------------------------------------

bool transaction::is_empty() const
{
    return inputs_->empty() || outputs_->empty();
}

bool transaction::is_null_non_coinbase() const
{
    BITCOIN_ASSERT(!is_coinbase());

    const auto invalid = [](const input::ptr& input)
    {
        return input->point().is_null();
    };

    // True if not coinbase but has null previous_output(s).
    return std::any_of(inputs_->begin(), inputs_->end(), invalid);
}

bool transaction::is_invalid_coinbase_size() const
{
    BITCOIN_ASSERT(is_coinbase());

    // True if coinbase and has invalid input[0] script size.
    const auto script_size = inputs_->front()->script().serialized_size(false);
    return script_size < min_coinbase_size || script_size > max_coinbase_size;
}

// Accept (contextual).
// ----------------------------------------------------------------------------

bool transaction::is_non_final(size_t height, uint32_t timestamp,
    uint32_t median_time_past, bool bip113) const
{
    // BIP113: comparing the locktime against the median of the past 11 block
    // timestamps, rather than the timestamp of the block including the tx.
    const auto time = bip113 ? median_time_past : timestamp;

    const auto finalized = [](const input::ptr& input)
    {
        return input->is_final();
    };

    const auto height_time = locktime_ < locktime_threshold ? height : time;

    return !(is_zero(locktime_) || locktime_ < height_time ||
        std::all_of(inputs_->begin(), inputs_->end(), finalized));
}

bool transaction::is_missing_prevouts() const
{
    BITCOIN_ASSERT(!is_coinbase());

    // Invalidity indicates not found.
    const auto missing = [](const input::ptr& input)
    {
        return !input->prevout->is_valid();
    };

    return std::any_of(inputs_->begin(), inputs_->end(), missing);
}

uint64_t transaction::claim() const
{
    // Overflow returns max_uint64.
    const auto sum = [](uint64_t total, const output::ptr& output)
    {
        return ceilinged_add(total, output->value());
    };

    // The amount claimed by outputs.
    return std::accumulate(outputs_->begin(), outputs_->end(), uint64_t(0), sum);
}

uint64_t transaction::value() const
{
    // Overflow and coinbase (default) return max_uint64.
    const auto sum = [](uint64_t total, const input::ptr& input)
    {
        return ceilinged_add(total, input->prevout->value());
    };

    // The amount of prevouts (referenced by inputs).
    return std::accumulate(inputs_->begin(), inputs_->end(), uint64_t(0), sum);
}

bool transaction::is_overspent() const
{
    BITCOIN_ASSERT(!is_coinbase());

    return claim() > value();
}

//*****************************************************************************
// CONSENSUS: Genesis block is treated as forever immature (satoshi bug).
//*****************************************************************************
bool transaction::is_immature(size_t height) const
{
    BITCOIN_ASSERT(!is_coinbase());

    // Overflow returns max_size_t.
    // Zero is either genesis or not found, either is immature.
    // Spends internal to a block are handled by block validation.
    const auto immature = [=](const input::ptr& input)
    {
        return input->prevout->coinbase && (is_zero(input->prevout->height) ||
            height < ceilinged_add(input->prevout->height, coinbase_maturity));
    };

    return std::any_of(inputs_->begin(), inputs_->end(), immature);
}

bool transaction::is_locked(size_t height, uint32_t median_time_past) const
{
    // BIP68: not applied to the sequence of the input of a coinbase.
    BITCOIN_ASSERT(!is_coinbase());

    // BIP68: applied to txs with a version greater than or equal to two.
    if (version_ < relative_locktime_min_version)
        return false;

    // BIP68: references to median time past are as defined by bip113.
    const auto locked = [=](const input::ptr& input)
    {
        return input->is_locked(height, median_time_past);
    };

    // BIP68: when the relative lock time is block based, it is interpreted as
    // a minimum block height constraint over the age of the input.
    return std::any_of(inputs_->begin(), inputs_->end(), locked);
}

// Spends internal to a block are handled by block validation.
bool transaction::is_unconfirmed_spend(size_t height) const
{
    BITCOIN_ASSERT(!is_coinbase());

    // Zero is either genesis or not found.
    // Test maturity first to obtain proper error code.
    // Spends internal to a block are handled by block validation.
    const auto unconfirmed = [=](const input::ptr& input)
    {
        return is_zero(input->prevout->height) &&
            !(height > input->prevout->height);
    };

    return std::any_of(inputs_->begin(), inputs_->end(), unconfirmed);
}

bool transaction::is_confirmed_double_spend(size_t height) const
{
    BITCOIN_ASSERT(!is_coinbase());

    // Spends internal to a block are handled by block validation.
    const auto spent = [=](const input::ptr& input)
    {
        return input->prevout->spent && height > input->prevout->height;
    };

    return std::any_of(inputs_->begin(), inputs_->end(), spent);
}

// Delegated.
// ----------------------------------------------------------------------------

// Coinbase transactions return success, to simplify iteration.
code transaction::connect_input(const context& state, size_t index) const
{
    // This is guarded as a protected method.
    BITCOIN_ASSERT(!is_zero(index) && index < inputs_->size());

    const auto prevout = (*inputs_)[index]->prevout;
    BITCOIN_ASSERT(prevout->is_valid());

    // Verify the transaction input script against the previous output.
    return script::verify(*this, static_cast<uint32_t>(index), state.forks,
        prevout->script(), prevout->value());
}

// Guards (for tx pool without compact blocks).
// ----------------------------------------------------------------------------

code transaction::guard() const
{
    // Pools do not have coinbases.
    if (is_coinbase())
        return error::coinbase_transaction;

    // Redundant with block is_internal_double_spend check.
    if (is_internal_double_spend())
        return error::transaction_internal_double_spend;

    // Redundant with block max_block_size check.
    if (is_oversized())
        return error::transaction_size_limit;

    return error::transaction_success;
}

code transaction::guard(const context& state) const
{
    const auto bip16 = state.is_enabled(forks::bip16_rule);
    const auto bip141 = state.is_enabled(forks::bip141_rule);

    if (!bip141 && is_segregated())
        return error::unexpected_witness_transaction;

    // Redundant with block max_block_weight accept.
    if (bip141 && is_overweight())
        return error::transaction_weight_limit;

    // prevouts required

    if (is_missing_prevouts())
        return error::missing_previous_output;

    // Independently computed by block accept to obtain total.
    if (is_signature_operations_limit(bip16, bip141))
        return error::transaction_sigop_limit;

    return error::transaction_success;
}

// Validation.
// ----------------------------------------------------------------------------

code transaction::check() const
{
    if (is_empty())
        return error::empty_transaction;

    if (is_coinbase())
    {
        if (is_invalid_coinbase_size())
            return error::invalid_coinbase_script_size;
    }
    else
    {
        if (is_null_non_coinbase())
            return error::previous_output_null;
    }

    return error::transaction_success;
}

code transaction::accept(const context& state) const
{
    const auto bip68 = state.is_enabled(forks::bip68_rule);
    const auto bip113 = state.is_enabled(forks::bip113_rule);

    // Store note: timestamp and mtp should be merged to single field.
    if (is_non_final(state.height, state.timestamp,
        state.median_time_past, bip113))
        return error::transaction_non_final;

    // Coinbases do not have prevouts.
    if (!is_coinbase())
    {
        // prevouts required

        if (is_missing_prevouts())
            return error::missing_previous_output;

        if (is_overspent())
            return error::spend_exceeds_value;

        if (is_immature(state.height))
            return error::coinbase_maturity;

        if (bip68 && is_locked(state.height, state.median_time_past))
            return error::relative_time_locked;

        // prevout confirmation state required

        if (is_unconfirmed_spend(state.height))
            return error::unconfirmed_spend;

        if (is_confirmed_double_spend(state.height))
            return error::confirmed_double_spend;
    }

    return error::transaction_success;
}

code transaction::connect(const context& state) const
{
    code ec;

    // Skip coinbase.
    for (auto input = one; input < inputs_->size(); ++input)
        if ((ec = connect_input(state, input)))
            return ec;

    return error::transaction_success;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
