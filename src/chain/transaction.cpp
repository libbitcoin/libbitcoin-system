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
#include <bitcoin/system/chain/transaction.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Precompute fixed elements of signature hashing.
// ----------------------------------------------------------------------------

constexpr auto prefixed = true;

static const auto& null_output() NOEXCEPT
{
    static const auto null = output{}.to_data();
    return null;
}

static const auto& empty_script() NOEXCEPT
{
    static const auto empty = script{}.to_data(prefixed);
    return empty;
}

static const auto& zero_sequence() NOEXCEPT
{
    static const auto sequence = to_little_endian<uint32_t>(0);
    return sequence;
}

// Constructors.
// ----------------------------------------------------------------------------

transaction::transaction() NOEXCEPT
  : transaction(0,
      to_shared<input_cptrs>(),
      to_shared<output_cptrs>(),
      0, false, false)
{
}

transaction::~transaction() NOEXCEPT
{
}

transaction::transaction(transaction&& other) NOEXCEPT
  : transaction(other)
{
}

transaction::transaction(const transaction& other) NOEXCEPT
  : transaction(
      other.version_,
      other.inputs_,
      other.outputs_,
      other.locktime_,
      other.segregated_,
      other.valid_)
{
    // Optimized for faster optional, not for copy.

    if (other.nominal_hash_)
        nominal_hash_ = to_unique(*other.nominal_hash_);
    else
        nominal_hash_.reset();

    if (other.witness_hash_)
        witness_hash_ = to_unique(*other.witness_hash_);
    else
        witness_hash_.reset();

    if (other.sighash_cache_)
        sighash_cache_ = to_unique(*other.sighash_cache_);
    else
        sighash_cache_.reset();
}

transaction::transaction(uint32_t version, chain::inputs&& inputs,
    chain::outputs&& outputs, uint32_t locktime) NOEXCEPT
  : transaction(version, to_shareds(std::move(inputs)),
      to_shareds(std::move(outputs)), locktime)
{
}

transaction::transaction(uint32_t version, const chain::inputs& inputs,
    const chain::outputs& outputs, uint32_t locktime) NOEXCEPT
  : transaction(version, to_shareds(inputs), to_shareds(outputs), locktime,
      segregated(inputs), true)
{
}

transaction::transaction(uint32_t version, const chain::inputs_cptr& inputs,
    const chain::outputs_cptr& outputs, uint32_t locktime) NOEXCEPT
  : transaction(version, inputs, outputs, locktime, segregated(*inputs), true)
{
}

transaction::transaction(const data_slice& data, bool witness) NOEXCEPT
  : transaction(stream::in::copy(data), witness)
{
}

////transaction::transaction(stream::in::fast&& stream, bool witness) NOEXCEPT
////  : transaction(read::bytes::fast(stream), witness)
////{
////}

transaction::transaction(stream::in::fast& stream, bool witness) NOEXCEPT
  : transaction(read::bytes::fast(stream), witness)
{
}

transaction::transaction(std::istream&& stream, bool witness) NOEXCEPT
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(std::istream& stream, bool witness) NOEXCEPT
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(reader&& source, bool witness) NOEXCEPT
  : transaction(from_data(source, witness))
{
}

transaction::transaction(reader& source, bool witness) NOEXCEPT
  : transaction(from_data(source, witness))
{
}

// protected
transaction::transaction(uint32_t version,
    const chain::inputs_cptr& inputs, const chain::outputs_cptr& outputs,
    uint32_t locktime, bool segregated, bool valid) NOEXCEPT
  : version_(version),
    inputs_(inputs ? inputs : to_shared<input_cptrs>()),
    outputs_(outputs ? outputs : to_shared<output_cptrs>()),
    locktime_(locktime),
    segregated_(segregated),
    valid_(valid),
    size_(serialized_size(*inputs, *outputs, segregated))
{
}

// Operators.
// ----------------------------------------------------------------------------

transaction& transaction::operator=(transaction&& other) NOEXCEPT
{
    *this = other;
    return *this;
}

transaction& transaction::operator=(const transaction& other) NOEXCEPT
{
    version_ = other.version_;
    inputs_ = other.inputs_;
    outputs_ = other.outputs_;
    locktime_ = other.locktime_;
    segregated_ = other.segregated_;
    valid_ = other.valid_;
    size_ = other.size_;

    // Optimized for faster optional, not for copy.

    if (other.nominal_hash_)
        nominal_hash_ = to_unique(*other.nominal_hash_);
    else
        nominal_hash_.reset();

    if (other.witness_hash_)
        witness_hash_ = to_unique(*other.witness_hash_);
    else
        witness_hash_.reset();

    if (other.sighash_cache_)
        sighash_cache_ = to_unique(*other.sighash_cache_);
    else
        sighash_cache_.reset();

    return *this;
}

bool transaction::operator==(const transaction& other) const NOEXCEPT
{
    // Compares input/output elements, not pointers, cache not compared.
    return (version_ == other.version_)
        && (locktime_ == other.locktime_)
        && ((inputs_ == other.inputs_) || 
            deep_equal(*inputs_, *other.inputs_))
        && ((outputs_ == other.outputs_) ||
            deep_equal(*outputs_, *other.outputs_));
}

bool transaction::operator!=(const transaction& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

template<class Put, class Source>
std::shared_ptr<const std::vector<std::shared_ptr<const Put>>>
read_puts(Source& source) NOEXCEPT
{
    auto puts = to_shared<std::vector<std::shared_ptr<const Put>>>();
    const auto capacity = source.read_size(max_block_size);

    puts->reserve(capacity);
    for (auto put = zero; put < capacity; ++put)
        puts->push_back(to_shared<Put>(source));

    // This is a pointer copy (non-const to const).
    return puts;
}

// static/private
transaction transaction::from_data(reader& source, bool witness) NOEXCEPT
{
    const auto version = source.read_4_bytes_little_endian();

    // Inputs must be non-const so that they may assign the witness.
    auto inputs = read_puts<input>(source);
    chain::outputs_cptr outputs;

    // Expensive repeated recomputation, so cache segregated state.
    const auto segregated = inputs->size() == witness_marker &&
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
                // Safe to cast as this method exclusively owns the input.
                const_cast<chain::input*>(input.get())->set_witness(source);
            }
            else
            {
                witness::skip(source, true);
            }
        }
    }
    else
    {
        // Default witness is populated on input construct.
        outputs = read_puts<const output>(source);
    }

    const auto locktime = source.read_4_bytes_little_endian();
    return { version, inputs, outputs, locktime, segregated, source };
}

// Serialization.
// ----------------------------------------------------------------------------

// Transactions with empty witnesses always use old serialization (bip144).
// If no inputs are witness programs then witness hash is tx hash (bip141).
data_chunk transaction::to_data(bool witness) const NOEXCEPT
{
    witness &= segregated_;

    data_chunk data(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void transaction::to_data(std::ostream& stream, bool witness) const NOEXCEPT
{
    witness &= segregated_;

    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void transaction::to_data(writer& sink, bool witness) const NOEXCEPT
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

// static/private
transaction::sizes transaction::serialized_size(
    const chain::input_cptrs& inputs,
    const chain::output_cptrs& outputs, bool segregated) NOEXCEPT
{
    sizes size{ zero, zero };

    // Keep the condition outside of the loop.
    if (segregated)
    {
        std::for_each(inputs.begin(), inputs.end(), [&](const auto& in) NOEXCEPT
        {
            size.nominal = ceilinged_add(size.nominal, in->nominal_size());
            size.witnessed = ceilinged_add(size.witnessed, in->witnessed_size());
        });
    }
    else
    {
        // Witness must be zeroed because witnesses have nonzero size when they
        // are zero-valued, so they can be archived easily. Also it would be
        // wasteful to to count mutiple zero sizes, so exclude them here.
        std::for_each(inputs.begin(), inputs.end(), [&](const auto& in) NOEXCEPT
        {
            size.nominal = ceilinged_add(size.nominal, in->nominal_size());
        });
    }

    const auto outs = [](size_t total, const auto& output) NOEXCEPT
    {
        return ceilinged_add(total, output->serialized_size());
    };

    constexpr auto base_const_size = ceilinged_add(
        sizeof(version_),
        sizeof(locktime_));

    constexpr auto witness_const_size = ceilinged_add(
        sizeof(witness_marker),
        sizeof(witness_enabled));

    const auto base_size = ceilinged_add(ceilinged_add(ceilinged_add(
        base_const_size,
        variable_size(inputs.size())),
        variable_size(outputs.size())),
        std::accumulate(outputs.begin(), outputs.end(), zero, outs));

    const auto nominal_size = ceilinged_add(
        base_size,
        size.nominal);

    const auto witnessed_size = ceilinged_add(ceilinged_add(
        base_size,
        witness_const_size),
        size.witnessed);

    return { nominal_size, witnessed_size };
}

size_t transaction::serialized_size(bool witness) const NOEXCEPT
{
    witness &= segregated_;

    return witness ? size_.witnessed : size_.nominal;
}

// Properties.
// ----------------------------------------------------------------------------

bool transaction::is_valid() const NOEXCEPT
{
    return valid_;
}

size_t transaction::inputs() const NOEXCEPT
{
    return inputs_->size();
}

size_t transaction::outputs() const NOEXCEPT
{
    return outputs_->size();
}

uint32_t transaction::version() const NOEXCEPT
{
    return version_;
}

uint32_t transaction::locktime() const NOEXCEPT
{
    return locktime_;
}

const inputs_cptr& transaction::inputs_ptr() const NOEXCEPT
{
    return inputs_;
}

const outputs_cptr& transaction::outputs_ptr() const NOEXCEPT
{
    return outputs_;
}

uint64_t transaction::fee() const NOEXCEPT
{
    // Underflow returns zero (and is_overspent() will be true).
    // This is value of prevouts spent by inputs minus that claimed by outputs.
    return floored_subtract(value(), claim());
}

void transaction::set_nominal_hash(hash_digest&& hash) const NOEXCEPT
{
    nominal_hash_ = to_unique(std::move(hash));
}

void transaction::set_witness_hash(hash_digest&& hash) const NOEXCEPT
{
    witness_hash_ = to_unique(std::move(hash));
}

const hash_digest& transaction::get_hash(bool witness) const NOEXCEPT
{
    if (witness)
    {
        if (!witness_hash_) set_witness_hash(hash(witness));
        return *witness_hash_;
    }
    else
    {
        if (!nominal_hash_) set_nominal_hash(hash(witness));
        return *nominal_hash_;
    }
}

hash_digest transaction::hash(bool witness) const NOEXCEPT
{
    if (segregated_)
    {
        if (witness)
        {
            // Witness coinbase tx hash is assumed to be null_hash (bip141).
            if (witness_hash_) return *witness_hash_;
            if (is_coinbase()) return null_hash;
        }
        else
        {
            if (nominal_hash_) return *nominal_hash_;
        }
    }
    else
    {
        if (nominal_hash_) return *nominal_hash_;
    }

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    to_data(sink, witness);
    sink.flush();
    return digest;
}

// Methods.
// ----------------------------------------------------------------------------

bool transaction::is_dusty(uint64_t minimum_output_value) const NOEXCEPT
{
    const auto dusty = [=](const auto& output) NOEXCEPT
    {
        return output->is_dust(minimum_output_value);
    };

    return std::any_of(outputs_->begin(), outputs_->end(), dusty);
}

size_t transaction::signature_operations(bool bip16, bool bip141) const NOEXCEPT
{
    // Includes BIP16 p2sh additional sigops, max_size_t if prevout invalid.
    const auto in = [=](size_t total, const auto& input) NOEXCEPT
    {
        return ceilinged_add(total, input->signature_operations(bip16, bip141));
    };

    const auto out = [=](size_t total, const auto& output) NOEXCEPT
    {
        return ceilinged_add(total, output->signature_operations(bip141));
    };

    // Overflow returns max_size_t.
    return ceilinged_add(
        std::accumulate(inputs_->begin(), inputs_->end(), zero, in),
        std::accumulate(outputs_->begin(), outputs_->end(), zero, out));
}

chain::points transaction::points() const NOEXCEPT
{
    chain::points out(inputs_->size());

    const auto point = [](const auto& input) NOEXCEPT
    {
        return input->point();
    };

    std::transform(inputs_->begin(), inputs_->end(), out.begin(), point);
    return out;
}

hash_digest transaction::outputs_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->outputs;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()
        
    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    const auto& outs = *outputs_;
    for (const auto& output: outs)
        output->to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::points_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->points;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    const auto& ins = *inputs_;
    for (const auto& input: ins)
        input->point().to_data(sink);

    sink.flush();
    return digest;
}

hash_digest transaction::sequences_hash() const NOEXCEPT
{
    if (sighash_cache_)
        return sighash_cache_->sequences;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    const auto& ins = *inputs_;
    for (const auto& input: ins)
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return digest;
}

// Signing (unversioned).
// ----------------------------------------------------------------------------

// private
transaction::input_iterator transaction::input_at(
    uint32_t index) const NOEXCEPT
{
    // Guarded by check_signature and create_endorsement.
    BC_ASSERT_MSG(index < inputs_->size(), "invalid input index");

    return std::next(inputs_->begin(), index);
}

// private
uint32_t transaction::input_index(const input_iterator& input) const NOEXCEPT
{
    // Guarded by unversioned_signature_hash and output_hash.
    BC_ASSERT_MSG(inputs_->begin() != inputs_->end(), "invalid input iterator");

    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_->begin(), input));
}

//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline coverage mask_sighash(uint8_t sighash_flags) NOEXCEPT
{
    switch (sighash_flags & coverage::mask)
    {
        case coverage::hash_single:
            return coverage::hash_single;
        case coverage::hash_none:
            return coverage::hash_none;
        default:
            return coverage::hash_all;
    }
}

void transaction::signature_hash_single(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &sub, sighash_flags](
        writer& sink) NOEXCEPT
    {
        const auto& self = **input;
        const auto anyone = to_bool(sighash_flags & coverage::anyone_can_pay);
        input_cptrs::const_iterator in;

        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        self.point().to_data(sink);
        sub.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self.sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }
    };

    const auto write_outputs = [this, &input](writer& sink) NOEXCEPT
    {
        const auto index = input_index(input);

        sink.write_variable(add1(index));

        for (size_t output = 0; output < index; ++output)
            sink.write_bytes(null_output());

        // Guarded by unversioned_signature_hash.
        outputs_->at(index)->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_none(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t sighash_flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &sub, sighash_flags](
        writer& sink) NOEXCEPT
    {
        const auto& self = **input;
        const auto anyone = to_bool(sighash_flags & coverage::anyone_can_pay);
        input_cptrs::const_iterator in;

        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }

        self.point().to_data(sink);
        sub.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self.sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_bytes(zero_sequence());
        }
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    sink.write_variable(zero);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);
}

void transaction::signature_hash_all(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t flags) const NOEXCEPT
{
    const auto write_inputs = [this, &input, &sub, flags](
        writer& sink) NOEXCEPT
    {
        const auto& self = **input;
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
        input_cptrs::const_iterator in;

        sink.write_variable(anyone ? one : inputs_->size());

        for (in = inputs_->begin(); !anyone && in != input; ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian((*in)->sequence());
        }

        self.point().to_data(sink);
        sub.to_data(sink, prefixed);
        sink.write_4_bytes_little_endian(self.sequence());

        for (++in; !anyone && in != inputs_->end(); ++in)
        {
            (*in)->point().to_data(sink);
            sink.write_bytes(empty_script());
            sink.write_4_bytes_little_endian((*in)->sequence());
        }
    };

    const auto write_outputs = [this](writer& sink) NOEXCEPT
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
hash_digest transaction::unversioned_signature_hash(
    const input_iterator& input, const script& sub,
    uint8_t sighash_flags) const NOEXCEPT
{
    // Set options.
    const auto flag = mask_sighash(sighash_flags);

    // Create hash writer.
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    switch (flag)
    {
        case coverage::hash_single:
        {
            //*****************************************************************
            // CONSENSUS: return one_hash if index exceeds outputs in sighash.
            // Related Bug: bitcointalk.org/index.php?topic=260595
            // Exploit: joncave.co.uk/2014/08/bitcoin-sighash-single/
            //*****************************************************************
            if (input_index(input) >= outputs_->size())
                return one_hash;

            signature_hash_single(sink, input, sub, sighash_flags);
            break;
        }
        case coverage::hash_none:
        {
            signature_hash_none(sink, input, sub, sighash_flags);
            break;
        }
        default:
        case coverage::hash_all:
        {
            signature_hash_all(sink, input, sub, sighash_flags);
        }
    }

    sink.flush();
    return digest;
}

// Signing (version 0).
// ----------------------------------------------------------------------------

// private
// TODO: taproot requires both single and double hash of each.
void transaction::initialize_sighash_cache() const NOEXCEPT
{
    // This overconstructs the cache (anyone or !all), however it is simple.
    if (!segregated_)
        return;

    sighash_cache_ = to_unique<sighash_cache>
    (
        outputs_hash(),
        points_hash(),
        sequences_hash()
    );
}

// private
hash_digest transaction::output_hash(const input_iterator& input) const NOEXCEPT
{
    const auto index = input_index(input);

    //*************************************************************************
    // CONSENSUS: if index exceeds outputs in signature hash, return null_hash.
    //*************************************************************************
    if (index >= outputs_->size())
        return null_hash;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };
    outputs_->at(index)->to_data(sink);
    sink.flush();
    return digest;
}

// private
hash_digest transaction::version_0_signature_hash(const input_iterator& input,
    const script& sub, uint64_t value, uint8_t sighash_flags,
    bool bip143) const NOEXCEPT
{
    // bip143/v0: the way of serialization is changed.
    if (!bip143)
        return unversioned_signature_hash(input, sub, sighash_flags);

    // Set options.
    const auto anyone = to_bool(sighash_flags & coverage::anyone_can_pay);
    const auto flag = mask_sighash(sighash_flags);
    const auto all = (flag == coverage::hash_all);
    const auto single = (flag == coverage::hash_single);
    const auto& self = **input;

    // Create hash writer.
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest digest;
    BC_POP_WARNING()

    stream::out::fast stream{ digest };
    hash::sha256x2::fast sink{ stream };

    // Create signature hash.
    sink.write_little_endian(version_);

    // Conditioning points, sequences, and outputs writes on cache_ instead of
    // conditionally passing them from methods avoids copying the cached hash.

    // points
    sink.write_bytes(!anyone ? points_hash() : null_hash);

    // sequences
    sink.write_bytes(!anyone && all ? sequences_hash() : null_hash);

    self.point().to_data(sink);
    sub.to_data(sink, prefixed);
    sink.write_little_endian(value);
    sink.write_little_endian(self.sequence());

    // outputs
    if (single)
        sink.write_bytes(output_hash(input));
    else
        sink.write_bytes(all ? outputs_hash() : null_hash);

    sink.write_little_endian(locktime_);
    sink.write_4_bytes_little_endian(sighash_flags);

    sink.flush();
    return digest;
}

// Signing (unversioned and version 0).
// ----------------------------------------------------------------------------

// ****************************************************************************
// CONSENSUS: sighash flags are carried in a single byte but are encoded as 4
// bytes in the signature hash preimage serialization.
// ****************************************************************************

hash_digest transaction::signature_hash(const input_iterator& input,
    const script& sub, uint64_t value, uint8_t sighash_flags,
    script_version version, bool bip143) const NOEXCEPT
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BC_ASSERT(!is_coinbase());

    switch (version)
    {
        case script_version::unversioned:
            return unversioned_signature_hash(input, sub, sighash_flags);
        case script_version::zero:
            return version_0_signature_hash(input, sub, value, sighash_flags,
                bip143);
        case script_version::reserved:
        default:
            return {};
    }
}

// This is not used internal to the library.
bool transaction::check_signature(const ec_signature& signature,
    const data_slice& public_key, const script& sub, uint32_t index,
    uint64_t value, uint8_t sighash_flags, script_version version,
    bool bip143) const NOEXCEPT
{
    if ((index >= inputs_->size()) || signature.empty() || public_key.empty())
        return false;

    const auto sighash = signature_hash(input_at(index), sub, value,
        sighash_flags, version, bip143);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

// This is not used internal to the library.
bool transaction::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& sub, uint32_t index, uint64_t value, uint8_t sighash_flags,
    script_version version, bool bip143) const NOEXCEPT
{
    if (index >= inputs_->size())
        return false;

    out.reserve(max_endorsement_size);
    const auto sighash = signature_hash(input_at(index), sub, value,
        sighash_flags, version, bip143);

    // Create the EC signature and encode as DER.
    ec_signature signature;
    if (!sign(signature, secret, sighash) || !encode_signature(out, signature))
        return false;

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(sighash_flags);
    out.shrink_to_fit();
    return true;
}

// Guard (context free).
// ----------------------------------------------------------------------------

bool transaction::is_coinbase() const NOEXCEPT
{
    return is_one(inputs_->size()) && inputs_->front()->point().is_null();
}

bool transaction::is_internal_double_spend() const NOEXCEPT
{
    // TODO: optimize (see block.is_internal_double_spend).
    return !is_distinct(points());
}

// TODO: a pool (non-coinbase) tx must fit into a block (with a coinbase).
bool transaction::is_oversized() const NOEXCEPT
{
    return serialized_size(false) > max_block_size;
}

// Guard (contextual).
// ----------------------------------------------------------------------------

// static/private
bool transaction::segregated(const chain::inputs& inputs) NOEXCEPT
{
    const auto witnessed = [](const auto& input) NOEXCEPT
    {
        return !input.witness().stack().empty();
    };

    return std::any_of(inputs.begin(), inputs.end(), witnessed);
}

// static/private
bool transaction::segregated(const chain::input_cptrs& inputs) NOEXCEPT
{
    const auto witnessed = [](const auto& input) NOEXCEPT
    {
        return !input->witness().stack().empty();
    };

    return std::any_of(inputs.begin(), inputs.end(), witnessed);
}

bool transaction::is_segregated() const NOEXCEPT
{
    return segregated_;
}

size_t transaction::weight() const NOEXCEPT
{
    // Block weight is 3 * base size * + 1 * total size (bip141).
    return ceilinged_add(
        ceilinged_multiply(base_size_contribution, serialized_size(false)),
        ceilinged_multiply(total_size_contribution, serialized_size(true)));
}

bool transaction::is_overweight() const NOEXCEPT
{
    return weight() > max_block_weight;
}

//*****************************************************************************
// CONSENSUS: Legacy sigops are counted in coinbase scripts despite the fact
// that coinbase input scripts are never executed. There is no need to exclude
// p2sh coinbase sigops since there is never a script to count.
//*****************************************************************************
bool transaction::is_signature_operations_limit(bool bip16,
    bool bip141) const NOEXCEPT
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;
    return signature_operations(bip16, bip141) > limit;
}

// Check (context free).
// ----------------------------------------------------------------------------

bool transaction::is_empty() const NOEXCEPT
{
    return inputs_->empty() || outputs_->empty();
}

bool transaction::is_null_non_coinbase() const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    const auto invalid = [](const auto& input) NOEXCEPT
    {
        return input->point().is_null();
    };

    // True if not coinbase but has null previous_output(s).
    return std::any_of(inputs_->begin(), inputs_->end(), invalid);
}

bool transaction::is_invalid_coinbase_size() const NOEXCEPT
{
    BC_ASSERT(is_coinbase());

    // True if coinbase and has invalid input[0] script size.
    const auto script_size = inputs_->front()->script().serialized_size(false);
    return script_size < min_coinbase_size || script_size > max_coinbase_size;
}

// Accept (contextual).
// ----------------------------------------------------------------------------

bool transaction::is_non_final(size_t height, uint32_t timestamp,
    uint32_t median_time_past, bool bip113) const NOEXCEPT
{
    // BIP113: comparing the locktime against the median of the past 11 block
    // timestamps, rather than the timestamp of the block including the tx.
    const auto time = bip113 ? median_time_past : timestamp;

    const auto finalized = [](const auto& input) NOEXCEPT
    {
        return input->is_final();
    };

    const auto height_time = locktime_ < locktime_threshold ? height : time;

    return !(is_zero(locktime_) || locktime_ < height_time ||
        std::all_of(inputs_->begin(), inputs_->end(), finalized));
}

bool transaction::is_missing_prevouts() const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    // Null or invalid prevout indicates not found.
    const auto missing = [](const auto& input) NOEXCEPT
    {
        return !input->prevout;
    };

    return std::any_of(inputs_->begin(), inputs_->end(), missing);
}

uint64_t transaction::claim() const NOEXCEPT
{
    // Overflow returns max_uint64.
    const auto sum = [](uint64_t total, const auto& output) NOEXCEPT
    {
        return ceilinged_add(total, output->value());
    };

    // The amount claimed by outputs.
    return std::accumulate(outputs_->begin(), outputs_->end(), 0_u64, sum);
}

uint64_t transaction::value() const NOEXCEPT
{
    // Overflow, not populated, and coinbase (default) return max_uint64.
    const auto sum = [](uint64_t total, const auto& input) NOEXCEPT
    {
        const auto value = input->prevout ? input->prevout->value() : max_uint64;
        return ceilinged_add(total, value);
    };

    // The amount of prevouts (referenced by inputs).
    return std::accumulate(inputs_->begin(), inputs_->end(), 0_u64, sum);
}

bool transaction::is_overspent() const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    return claim() > value();
}

constexpr bool is_non_coinbase_mature(size_t tx_height, size_t height) NOEXCEPT
{
    return tx_height <= height;
}

//*****************************************************************************
// CONSENSUS: Coinbase output matures at 100 blocks depth.
// CONSENSUS: Genesis coinbase is forever immature (exception).
//*****************************************************************************
bool transaction::is_coinbase_mature(size_t coinbase_height,
    size_t height) NOEXCEPT
{
    return !is_zero(coinbase_height) &&
        ceilinged_add(coinbase_height, coinbase_maturity) <= height;
}

bool transaction::is_immature(size_t height) const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    // Spends internal to a block are handled by block validation.
    const auto mature = [=](const auto& input) NOEXCEPT
    {
        return input->metadata.coinbase ?
            is_coinbase_mature(input->metadata.height, height) :
            is_non_coinbase_mature(input->metadata.height, height);
    };

    return !std::all_of(inputs_->begin(), inputs_->end(), mature);
}

bool transaction::is_locked(size_t height,
    uint32_t median_time_past) const NOEXCEPT
{
    // BIP68: not applied to the sequence of the input of a coinbase.
    BC_ASSERT(!is_coinbase());

    // BIP68: applied to txs with a version greater than or equal to two.
    if (version_ < relative_locktime_min_version)
        return false;

    // BIP68: references to median time past are as defined by bip113.
    const auto locked = [=](const auto& input) NOEXCEPT
    {
        return input->is_locked(height, median_time_past);
    };

    // BIP68: when the relative lock time is block based, it is interpreted as
    // a minimum block height constraint over the age of the input.
    return std::any_of(inputs_->begin(), inputs_->end(), locked);
}

// Spends internal to a block are handled by block validation.
bool transaction::is_unconfirmed_spend(size_t height) const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    // Zero is either genesis or not found.
    // Test maturity first to obtain proper error code.
    // Spends internal to a block are handled by block validation.
    const auto unconfirmed = [=](const auto& input) NOEXCEPT
    {
        const auto prevout_height = input->metadata.height;
        return is_zero(prevout_height) && !(height > prevout_height);
    };

    return std::any_of(inputs_->begin(), inputs_->end(), unconfirmed);
}

bool transaction::is_confirmed_double_spend(size_t height) const NOEXCEPT
{
    BC_ASSERT(!is_coinbase());

    // Spends internal to a block are handled by block validation.
    const auto spent = [=](const auto& input) NOEXCEPT
    {
        return input->metadata.spent && height > input->metadata.height;
    };

    return std::any_of(inputs_->begin(), inputs_->end(), spent);
}

// Guards (for tx pool without compact blocks).
// ----------------------------------------------------------------------------

// Pools do not have coinbases.
// Redundant with block is_internal_double_spend check.
// Redundant with block max_block_size check.
code transaction::guard_check() const NOEXCEPT
{
    if (is_coinbase())
        return error::coinbase_transaction;
    if (is_internal_double_spend())
        return error::transaction_internal_double_spend;
    if (is_oversized())
        return error::transaction_size_limit;

    return error::transaction_success;
}

// Redundant with block max_block_weight accept.
code transaction::guard_check(const context& ctx) const NOEXCEPT
{
    const auto bip141 = ctx.is_enabled(flags::bip141_rule);

     if (!bip141 && is_segregated())
        return error::unexpected_witness_transaction;
    if (bip141 && is_overweight())
        return error::transaction_weight_limit;

    return error::transaction_success;
}

// Redundant with block max_block_sigops accept.
code transaction::guard_accept(const context& ctx) const NOEXCEPT
{
    const auto bip16 = ctx.is_enabled(flags::bip16_rule);
    const auto bip141 = ctx.is_enabled(flags::bip141_rule);

    if (is_missing_prevouts())
        return error::missing_previous_output;
    if (is_signature_operations_limit(bip16, bip141))
        return error::transaction_sigop_limit;

    return error::transaction_success;
}

// Validation.
// ----------------------------------------------------------------------------

// DO invoke on coinbase.
code transaction::check() const NOEXCEPT
{
    const auto coinbase = is_coinbase();

    if (is_empty())
        return error::empty_transaction;
    if (coinbase && is_invalid_coinbase_size())
        return error::invalid_coinbase_script_size;
    if (!coinbase && is_null_non_coinbase())
        return error::previous_output_null;

    return error::transaction_success;
}

// forks
// height
// timestamp
// median_time_past

// DO invoke on coinbase.
code transaction::check(const context& ctx) const NOEXCEPT
{
    const auto bip113 = ctx.is_enabled(bip113_rule);

    if (is_non_final(ctx.height, ctx.timestamp, ctx.median_time_past, bip113))
        return error::transaction_non_final;

    return error::transaction_success;
}

// Do not need to invoke on coinbase.
// This assumes that prevout caching is completed on all inputs.
code transaction::accept(const context&) const NOEXCEPT
{
    ////BC_ASSERT(!is_coinbase());

    if (is_coinbase())
        return error::transaction_success;
    if (is_missing_prevouts())
        return error::missing_previous_output;
    if (is_overspent())
        return error::spend_exceeds_value;

    return error::transaction_success;
}

// forks
// height
// median_time_past

// Do not need to invoke on coinbase.
// Node performs these checks through database query.
// This assumes that prevout and metadata caching are completed on all inputs.
code transaction::confirm(const context& ctx) const NOEXCEPT
{
    ////BC_ASSERT(!is_coinbase());
    const auto bip68 = ctx.is_enabled(bip68_rule);

    if (is_coinbase())
        return error::transaction_success;
    if (bip68 && is_locked(ctx.height, ctx.median_time_past))
        return error::relative_time_locked;
    if (is_immature(ctx.height))
        return error::coinbase_maturity;
    if (is_unconfirmed_spend(ctx.height))
        return error::unconfirmed_spend;
    if (is_confirmed_double_spend(ctx.height))
        return error::confirmed_double_spend;

    return error::transaction_success;
}

// Connect (contextual).
// ----------------------------------------------------------------------------

// forks

// Do not need to invoke on coinbase.
code transaction::connect(const context& ctx) const NOEXCEPT
{
    ////BC_ASSERT(!is_coinbase());

    if (is_coinbase())
        return error::transaction_success;

    code ec{};
    using namespace machine;
    initialize_sighash_cache();

    // Validate scripts.
    for (auto input = inputs_->begin(); input != inputs_->end(); ++input)
    {
        // Evaluate rolling scripts with linear search but constant erase.
        // Evaluate non-rolling scripts with constant search but linear erase.
        if ((ec = (*input)->is_roller() ?
            interpreter<linked_stack>::connect(ctx, *this, input) :
            interpreter<contiguous_stack>::connect(ctx, *this, input)))
            return ec;
    }

    // TODO: accumulate sigops from each connect result and add coinbase.
    // TODO: return in override with out parameter. more impactful with segwit.
    return error::transaction_success;
}

BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

transaction tag_invoke(json::value_to_tag<transaction>,
    const json::value& value) NOEXCEPT
{
    return
    {
        value.at("version").to_number<uint32_t>(),
        json::value_to<chain::inputs>(value.at("inputs")),
        json::value_to<chain::outputs>(value.at("outputs")),
        value.at("locktime").to_number<uint32_t>()
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const transaction& tx) NOEXCEPT
{
    value =
    {
        { "version", tx.version() },
        { "inputs", *tx.inputs_ptr() },
        { "outputs", *tx.outputs_ptr() },
        { "locktime", tx.locktime() }
    };
}

BC_POP_WARNING()

transaction::cptr tag_invoke(json::value_to_tag<transaction::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<transaction>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const transaction::cptr& tx) NOEXCEPT
{
    tag_invoke(tag, value, *tx);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
