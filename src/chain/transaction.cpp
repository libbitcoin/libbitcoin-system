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
#include <bitcoin/system/machine/machine.hpp>
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

// Cache not copied or moved.
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

transaction::transaction(std::istream&& stream, bool witness)
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(std::istream& stream, bool witness)
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(reader&& source, bool witness)
  : transaction(from_data(source, witness))
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
    // Cache not assigned.
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
    // Compares input/output elements, not pointers, cache not compared.
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
    for (const auto& output: *outputs_)
        output->to_data(sink);

    if (witness)
        for (auto& input: *inputs_)
            input->witness().to_data(sink, true);

    sink.write_4_bytes_little_endian(locktime_);
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
    if (witness && segregated_ && is_coinbase())
        return null_hash;

    hash_digest sha256;
    hash::sha256::copy sink(sha256);
    to_data(sink, witness);
    sink.flush();
    return sha256_hash(sha256);
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

hash_digest transaction::output_hash(uint32_t index) const
{
    //*************************************************************************
    // CONSENSUS: if index exceeds outputs in signature hash, return null_hash.
    //*************************************************************************
    if (index >= outputs_->size())
        return null_hash;

    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    (*outputs_)[index]->to_data(sink);

    sink.flush();
    return sha256_hash(sha256);
}

hash_digest transaction::outputs_hash() const
{
    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    for (const auto& output: *outputs())
        output->to_data(sink);

    sink.flush();
    return sha256_hash(sha256);
}

hash_digest transaction::points_hash() const
{
    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    for (const auto& input: *inputs())
        input->point().to_data(sink);

    sink.flush();
    return sha256_hash(sha256);
}

hash_digest transaction::sequences_hash() const
{
    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    for (const auto& input: *inputs())
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return sha256_hash(sha256);
}

// Signing (unversioned).
// ----------------------------------------------------------------------------

// Precompute fixed elements of signature hashing.
static constexpr auto prefixed = true;
static const auto null_output = output{}.to_data();
static const auto empty_script = script{}.to_data(prefixed);
static const auto zero_sequence = to_little_endian<uint32_t>(0);

//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline coverage mask_sighash(uint8_t flags)
{
    switch (flags & coverage::mask)
    {
        case coverage::hash_single:
            return coverage::hash_single;
        case coverage::hash_none:
            return coverage::hash_none;
        default:
            return coverage::hash_all;
    }
}

void transaction::signature_hash_single(writer& sink, uint32_t index,
    const script& subscript, uint8_t flags) const
{
    const auto write_inputs = [this, &subscript, index, flags](writer& sink)
    {
        const auto& inputs = *inputs_;
        const auto& self = inputs[index];
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
        input_ptrs::const_iterator input;

        sink.write_variable(anyone ? one : inputs.size());

        for (input = inputs.begin(); !anyone && *input != self; ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_bytes(zero_sequence);
        }

        self->point().to_data(sink);
        subscript.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self->sequence());

        for (++input; !anyone && input != inputs.end(); ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_bytes(zero_sequence);
        }
    };

    const auto write_outputs = [this, index](writer& sink)
    {
        sink.write_variable(add1(index));

        for (size_t output = 0; output < index; ++output)
            sink.write_bytes(null_output);

        // Index guarded in unversioned_signature_hash.
        (*outputs_)[index]->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(flags);
}

void transaction::signature_hash_none(writer& sink, uint32_t index,
    const script& subscript, uint8_t flags) const
{
    const auto write_inputs = [this, &subscript, index, flags](writer& sink)
    {
        const auto& inputs = *inputs_;
        const auto& self = inputs[index];
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
        input_ptrs::const_iterator input;

        sink.write_variable(anyone ? one : inputs.size());

        for (input = inputs.begin(); !anyone && *input != self; ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_bytes(zero_sequence);
        }

        self->point().to_data(sink);
        subscript.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self->sequence());

        for (++input; !anyone && input != inputs.end(); ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_bytes(zero_sequence);
        }
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    sink.write_variable(zero);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(flags);
}

void transaction::signature_hash_all(writer& sink, uint32_t index,
    const script& subscript, uint8_t flags) const
{
    const auto write_inputs = [this, &subscript, index, flags](writer& sink)
    {
        const auto& inputs = *inputs_;
        const auto& self = inputs[index];
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
        input_ptrs::const_iterator input;

        sink.write_variable(anyone ? one : inputs.size());

        for (input = inputs.begin(); !anyone && *input != self; ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_4_bytes_little_endian((*input)->sequence());
        }

        self->point().to_data(sink);
        subscript.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self->sequence());

        for (++input; !anyone && input != inputs.end(); ++input)
        {
            (*input)->point().to_data(sink);
            sink.write_bytes(empty_script);
            sink.write_4_bytes_little_endian((*input)->sequence());
        }
    };

    const auto write_outputs = [this](writer& sink)
    {
        sink.write_variable(outputs_->size());
        for (const auto& output: *outputs_)
            output->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(flags);
}

// private
hash_digest transaction::unversioned_signature_hash(uint32_t index,
    const script& subscript, uint8_t flags) const
{
    // Set options.
    const auto flag = mask_sighash(flags);

    //*************************************************************************
    // CONSENSUS: if index exceeds outputs in signature hash, return one_hash.
    //*************************************************************************
    if ((flag == coverage::hash_single) && index >= outputs()->size())
        return one_hash;

    // Create hash writer.
    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    switch (flag)
    {
        case coverage::hash_single:
            signature_hash_single(sink, index, subscript, flags);
            break;
        case coverage::hash_none:
            signature_hash_none(sink, index, subscript, flags);
            break;
        default:
        case coverage::hash_all:
            signature_hash_all(sink, index, subscript, flags);
    }

    sink.flush();
    return sha256_hash(sha256);
}

// Signing (version 0).
// ----------------------------------------------------------------------------

// private
// TODO: taproot requires both single and double hash of each.
void transaction::initialize_hash_cache() const
{
    // This overconstructs the cache (anyone or !all), however it is simple and
    // the same criteria applied by satoshi.
    if (segregated_)
        cache_.reset(new hash_cache
            {
                outputs_hash(),
                points_hash(),
                sequences_hash()
            });
}

// private
hash_digest transaction::version_0_signature_hash(uint32_t index,
    const script& subscript, uint64_t value, uint8_t flags, bool bip143) const
{
    // bip143/v0: the way of serialization is changed.
    if (!bip143)
        return unversioned_signature_hash(index, subscript, flags);

    // Set options.
    const auto anyone = to_bool(flags & coverage::anyone_can_pay);
    const auto flag = mask_sighash(flags);
    const auto all = (flag == coverage::hash_all);
    const auto single = (flag == coverage::hash_single);
    const auto& input = (*inputs_)[index];

    // Create hash writer.
    hash_digest sha256;
    hash::sha256::copy sink(sha256);

    // Create signature hash.
    sink.write_little_endian(version_);

    // Conditioning points, sequences, and outputs writes on cache_ instead of
    // conditionally passing them from methods avoids copying the cached hash.

    // points
    if (cache_)
        sink.write_bytes(!anyone ? cache_->points : null_hash);
    else
        sink.write_bytes(!anyone ? points_hash() : null_hash);

    // sequences
    if (cache_)
        sink.write_bytes(!anyone && all ? cache_->sequences : null_hash);
    else
        sink.write_bytes(!anyone && all ? sequences_hash() : null_hash);

    input->point().to_data(sink);
    subscript.to_data(sink, prefixed);
    sink.write_little_endian(value);
    sink.write_little_endian(input->sequence());

    // outputs
    if (single)
        sink.write_bytes(output_hash(index));
    else if (cache_)
        sink.write_bytes(all ? cache_->outputs : null_hash);
    else
        sink.write_bytes(all ? outputs_hash() : null_hash);

    sink.write_little_endian(locktime_);
    sink.write_4_bytes_little_endian(flags);

    sink.flush();
    return sha256_hash(sha256);
}

// Signing (unversioned and version 0).
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************

hash_digest transaction::signature_hash(uint32_t index,
    const script& subscript, uint64_t value, uint8_t flags,
    script_version version, bool bip143) const
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BITCOIN_ASSERT(!is_coinbase());

    // Null return implies program misparameterization, not consensus possible.
    if (index >= inputs()->size())
        return {};

    switch (version)
    {
        case script_version::unversioned:
            return unversioned_signature_hash(index, subscript, flags);
        case script_version::zero:
            return version_0_signature_hash(index, subscript, value, flags,
                bip143);
        case script_version::reserved:
        default:
            return {};
    }
}

bool transaction::check_signature(const ec_signature& signature,
    const data_slice& public_key, const script& subscript, uint32_t index,
    uint64_t value, uint8_t flags, script_version version, bool bip143) const
{
    if (signature.empty() || public_key.empty())
        return false;

    const auto sighash = signature_hash(index, subscript, value, flags,
        version, bip143);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

bool transaction::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& prevout_script, uint32_t index, uint64_t value, uint8_t flags,
    script_version version, bool bip143) const
{
    out.reserve(max_endorsement_size);

    const auto sighash = signature_hash(index, prevout_script, value, flags,
        version, bip143);

    // Create the EC signature and encode as DER.
    ec_signature signature;
    if (!sign(signature, secret, sighash) || !encode_signature(out, signature))
        return false;

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(flags);
    out.shrink_to_fit();
    return true;
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
        // Could be delegated to input loop.
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
    if (is_non_final(state.height, state.timestamp, state.median_time_past, bip113))
        return error::transaction_non_final;

    // Coinbases do not have prevouts.
    if (!is_coinbase())
    {
        // prevouts required

        if (is_missing_prevouts())
            return error::missing_previous_output;

        if (is_overspent())
            return error::spend_exceeds_value;

        // Could be delegated to input loop.
        if (is_immature(state.height))
            return error::coinbase_maturity;

        // Could be delegated to input loop.
        if (bip68 && is_locked(state.height, state.median_time_past))
            return error::relative_time_locked;

        // prevout confirmation state required

        // Could be delegated to input loop.
        if (is_unconfirmed_spend(state.height))
            return error::unconfirmed_spend;

        // Could be delegated to input loop.
        if (is_confirmed_double_spend(state.height))
            return error::confirmed_double_spend;
    }

    return error::transaction_success;
}

code transaction::connect(const context& state) const
{
    code ec;

    // Cache witness hash components that don't change per input.
    initialize_hash_cache();

    // Validate scripts, skip coinbase.
    for (uint32_t input = one; input < inputs_->size(); ++input)
        if ((ec = connect(state, input)))
            return ec;

    return error::transaction_success;
}

// TODO: Implement original op_codeseparator concatentaion [< 0.3.6].
// TODO: Implement combined script size limit soft fork (20,000) [0.3.6+].
code transaction::connect(const context& state, uint32_t index) const
{
    using namespace system::machine;

    code ec;
    bool witnessed;
    const auto& in = (*inputs_)[index];

    // Evaluate input script.
    program input(in->script_ptr(), *this, index, state.forks);
    if ((ec = interpreter::run(input)))
        return ec;

    // Evaluate output script using stack result from input script.
    program prevout(in->prevout->script_ptr(), input);
    if ((ec = interpreter::run(prevout)))
        return ec;

    // This precludes bare witness programs of -0 (undocumented).
    if (!prevout.stack_result(false))
        return error::stack_false;

    // Triggered by output script push of version and witness program (bip141).
    if ((witnessed = in->prevout->script().is_pay_to_witness(state.forks)))
    {
        // The input script must be empty (bip141).
        if (!in->script().ops().empty())
            return error::dirty_witness;

        // Validate the native script.
        switch (in->prevout->script().version())
        {
            case script_version::zero:
            {
                chunk_ptrs stack;
                script witness_script;
                if (!in->witness().extract_script(witness_script, stack,
                    in->prevout->script()))
                    return error::invalid_witness;

                // A defined version indicates bip141 is active (not bip143).
                program witness(to_shared<script>(witness_script), *this,
                    index, state.forks, std::move(stack), in->prevout->value(),
                    in->prevout->script().version());

                if ((ec = interpreter::run(witness)))
                    return ec;

                // A v0 script must succeed with a clean true stack (bip141).
                return witness.stack_result(true) ? error::script_success :
                    error::stack_false;
            }

            // These versions are reserved for future extensions (bip141).
            case script_version::reserved:
                return error::script_success;

            case script_version::unversioned:
            default:
                return error::unversioned_script;
        }
    }

    // p2sh and p2w are mutually exclusive.
    else if (in->prevout->script().is_pay_to_script_hash(state.forks))
    {
        if (!script::is_relaxed_push(in->script().ops()))
            return error::invalid_script_embed;

        // Embedded script must be at the top of the stack (bip16).
        script embedded_script(input.pop(), false);

        program embedded(to_shared<script>(embedded_script), std::move(input), true);
        if ((ec = interpreter::run(embedded)))
            return ec;

        // This precludes embedded witness programs of -0 (undocumented).
        if (!embedded.stack_result(false))
            return error::stack_false;

        // Triggered by embedded push of version and witness program (bip141).
        if ((witnessed = embedded_script.is_pay_to_witness(state.forks)))
        {
            // The input script must be a push of the embedded_script (bip141).
            if (in->script().ops().size() != one)
                return error::dirty_witness;

            // Validate the non-native script.
            switch (embedded_script.version())
            {
                case script_version::zero:
                {
                    chunk_ptrs stack;
                    script witness_script;
                    if (!in->witness().extract_script(witness_script, stack,
                        embedded_script))
                        return error::invalid_witness;

                    // A defined version indicates bip141 is active (not bip143).
                    program witness(to_shared<script>(witness_script), *this,
                        index, state.forks, std::move(stack),
                        in->prevout->value(), embedded_script.version());

                    if ((ec = interpreter::run(witness)))
                        return ec;

                    // A v0 script must succeed with a clean true stack (bip141).
                    return witness.stack_result(true) ? error::script_success :
                        error::stack_false;
                }

                // These versions are reserved for future extensions (bip141).
                case script_version::reserved:
                    return error::script_success;

                case script_version::unversioned:
                default:
                    return error::unversioned_script;
            }
        }
    }

    // Witness must be empty if no bip141 or invalid witness program (bip141).
    if (!witnessed && !in->witness().stack().empty())
        return error::unexpected_witness;

    return error::script_success;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
