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
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/machine/machine.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Precompute fixed elements of signature hashing.
// ----------------------------------------------------------------------------

constexpr auto prefixed = true;

static const auto& null_output() noexcept
{
    static const auto null = output{}.to_data();
    return null;
}

static const auto& empty_script() noexcept
{
    static const auto empty = script{}.to_data(prefixed);
    return empty;
}

static const auto& zero_sequence() noexcept
{
    static const auto sequence = to_little_endian<uint32_t>(0);
    return sequence;
}

// Constructors.
// ----------------------------------------------------------------------------

transaction::transaction() noexcept
  : transaction(0,
      to_shared<input_cptrs>(),
      to_shared<output_cptrs>(),
      0, false, false)
{
}

transaction::~transaction() noexcept
{
}

transaction::transaction(transaction&& other) noexcept
  : transaction(other)
{
}

// Cache not copied or moved.
transaction::transaction(const transaction& other) noexcept
  : transaction(
      other.version_,
      other.inputs_,
      other.outputs_,
      other.locktime_,
      other.segregated_,
      other.valid_)
{
}

transaction::transaction(uint32_t version, chain::inputs&& inputs,
    chain::outputs&& outputs, uint32_t locktime) noexcept
  : transaction(version, to_shareds(std::move(inputs)),
      to_shareds(std::move(outputs)), locktime, false, true)
{
    // Defer execution for constructor move.
    segregated_ = segregated(*inputs_);
}

transaction::transaction(uint32_t version, const chain::inputs& inputs,
    const chain::outputs& outputs, uint32_t locktime) noexcept
  : transaction(version, to_shareds(inputs), to_shareds(outputs), locktime,
      segregated(inputs), true)
{
}

transaction::transaction(uint32_t version, const chain::inputs_cptr& inputs,
    const chain::outputs_cptr& outputs, uint32_t locktime) noexcept
  : transaction(version, inputs, outputs, locktime, segregated(*inputs), true)
{
}

transaction::transaction(const data_slice& data, bool witness) noexcept
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : transaction(stream::in::copy(data), witness)
    BC_POP_WARNING()
{
}

transaction::transaction(std::istream&& stream, bool witness) noexcept
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(std::istream& stream, bool witness) noexcept
  : transaction(read::bytes::istream(stream), witness)
{
}

transaction::transaction(reader&& source, bool witness) noexcept
  : transaction(from_data(source, witness))
{
}

transaction::transaction(reader& source, bool witness) noexcept
  : transaction(from_data(source, witness))
{
}

// protected
transaction::transaction(uint32_t version,
    const chain::inputs_cptr& inputs, const chain::outputs_cptr& outputs,
    uint32_t locktime, bool segregated, bool valid) noexcept
  : version_(version),
    inputs_(inputs ? inputs : to_shared<input_cptrs>()),
    outputs_(outputs ? outputs : to_shared<output_cptrs>()),
    locktime_(locktime),
    segregated_(segregated),
    valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

transaction& transaction::operator=(transaction&& other) noexcept
{
    *this = other;
    return *this;
}

transaction& transaction::operator=(const transaction& other) noexcept
{
    // Cache not assigned.
    version_ = other.version_;
    inputs_ = other.inputs_;
    outputs_ = other.outputs_;
    locktime_ = other.locktime_;
    segregated_ = other.segregated_;
    valid_ = other.valid_;
    return *this;
}

bool transaction::operator==(const transaction& other) const noexcept
{
    // Compares input/output elements, not pointers, cache not compared.
    return (version_ == other.version_)
        && (locktime_ == other.locktime_)
        && (inputs_ == other.inputs_) ||
            deep_equal(*inputs_, *other.inputs_)
        && (outputs_ == other.outputs_) ||
            deep_equal(*outputs_, *other.outputs_);
}

bool transaction::operator!=(const transaction& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

template<class Put, class Source>
std::shared_ptr<const std::vector<std::shared_ptr<const Put>>>
read_puts(Source& source) noexcept
{
    auto puts = to_shared<std::vector<std::shared_ptr<const Put>>>();

    // Subsequent emplace is non-allocating, but still noexcept(false).
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    puts->reserve(source.read_size(max_block_size));
    BC_POP_WARNING()

    for (auto put = zero; put < puts->capacity(); ++put)
    {
        BC_PUSH_WARNING(NO_NEW_DELETE)
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        puts->emplace_back(new Put{ source });
        BC_POP_WARNING()
        BC_POP_WARNING()
    }

    // This is a pointer copy from non-const to const, which is unavoidable if
    // we want to avoid a vector move into a pointer to a const vector.
    return puts;
}

// static/private
transaction transaction::from_data(reader& source, bool witness) noexcept
{
    const auto version = source.read_4_bytes_little_endian();

    // Inputs must be non-const so that they may assign the witness.
    auto inputs = read_puts<chain::input>(source);
    chain::outputs_cptr outputs;

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

                // Use of pointer forward here avoids move construction.
                BC_PUSH_WARNING(NO_NEW_DELETE)
                BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
                setter->witness_ = to_shared(new chain::witness{ source, true });
                BC_POP_WARNING()
                BC_POP_WARNING()
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

    const auto locktime = source.read_4_bytes_little_endian();
    return { version, inputs, outputs, locktime, segregated, source };
}

// Serialization.
// ----------------------------------------------------------------------------

// Transactions with empty witnesses always use old serialization (bip144).
// If no inputs are witness programs then witness hash is tx hash (bip141).
data_chunk transaction::to_data(bool witness) const noexcept
{
    witness &= segregated_;

    data_chunk data(serialized_size(witness), no_fill_byte_allocator);

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream, witness);
    return data;
}

void transaction::to_data(std::ostream& stream, bool witness) const noexcept
{
    witness &= segregated_;

    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void transaction::to_data(writer& sink, bool witness) const noexcept
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

size_t transaction::serialized_size(bool witness) const noexcept
{
    witness &= segregated_;

    const auto ins = [=](size_t total, const auto& input) noexcept
    {
        // Inputs account for witness bytes. 
        return total + input->serialized_size(witness);
    };

    const auto outs = [](size_t total, const auto& output) noexcept
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

bool transaction::is_valid() const noexcept
{
    return valid_;
}

uint32_t transaction::version() const noexcept
{
    return version_;
}

uint32_t transaction::locktime() const noexcept
{
    return locktime_;
}

const inputs_cptr& transaction::inputs_ptr() const noexcept
{
    return inputs_;
}

const outputs_cptr& transaction::outputs_ptr() const noexcept
{
    return outputs_;
}

uint64_t transaction::fee() const noexcept
{
    // Underflow returns zero (and is_overspent() will be true).
    // This is value of prevouts spent by inputs minus that claimed by outputs.
    return floored_subtract(value(), claim());
}

hash_digest transaction::hash(bool witness) const noexcept
{
    // Witness coinbase tx hash is assumed to be null_hash (bip141).
    if (witness && segregated_ && is_coinbase())
        return null_hash;

    // This is an out parameter.
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);
    to_data(sink, witness);
    sink.flush();
    return sha256_hash(sha256);
}

// Methods.
// ----------------------------------------------------------------------------

bool transaction::is_dusty(uint64_t minimum_output_value) const noexcept
{
    const auto dusty = [=](const auto& output) noexcept
    {
        return output->is_dust(minimum_output_value);
    };

    return std::any_of(outputs_->begin(), outputs_->end(), dusty);
}

size_t transaction::signature_operations(bool bip16, bool bip141) const noexcept
{
    // Includes BIP16 p2sh additional sigops, max_size_t if prevout invalid.
    const auto in = [=](size_t total, const auto& input) noexcept
    {
        return ceilinged_add(total, input->signature_operations(bip16, bip141));
    };

    const auto out = [=](size_t total, const auto& output) noexcept
    {
        return ceilinged_add(total, output->signature_operations(bip141));
    };

    // Overflow returns max_size_t.
    return std::accumulate(inputs_->begin(), inputs_->end(), zero, in) +
        std::accumulate(outputs_->begin(), outputs_->end(), zero, out);
}

chain::points transaction::points() const noexcept
{
    static no_fill_allocator<point> no_fill_point_allocator{};
    chain::points out(inputs_->size(), no_fill_point_allocator);

    const auto point = [](const auto& input) noexcept
    {
        return input->point();
    };

    std::transform(inputs_->begin(), inputs_->end(), out.begin(), point);
    return out;
}

hash_digest transaction::outputs_hash() const noexcept
{
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);

    const auto& outs = *outputs_;
    for (const auto& output: outs)
        output->to_data(sink);

    sink.flush();
    return sha256_hash(sha256);
}

hash_digest transaction::points_hash() const noexcept
{
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);

    const auto& ins = *inputs_;
    for (const auto& input: ins)
        input->point().to_data(sink);

    sink.flush();
    return sha256_hash(sha256);
}

hash_digest transaction::sequences_hash() const noexcept
{
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);

    const auto& ins = *inputs_;
    for (const auto& input: ins)
        sink.write_4_bytes_little_endian(input->sequence());

    sink.flush();
    return sha256_hash(sha256);
}

// Signing (unversioned).
// ----------------------------------------------------------------------------

// private
transaction::input_iterator transaction::input_at(
    uint32_t index) const noexcept
{
    // Guarded by check_signature and create_endorsement.
    BC_ASSERT_MSG(index < inputs_->size(), "invalid input index");

    return std::next(inputs_->begin(), index);
}

// private
uint32_t transaction::input_index(const input_iterator& input) const noexcept
{
    // Guarded by unversioned_signature_hash and output_hash.
    BC_ASSERT_MSG(inputs_->begin() != inputs_->end(), "invalid input iterator");

    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_->begin(), input));
}

// C++14: switch in constexpr.
//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag),
// there are 4 possible 7 bit values that can set "single" and 4 others that
// can set none, and yet all other values set "all".
//*****************************************************************************
inline coverage mask_sighash(uint8_t flags) noexcept
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

/// REQUIRES INDEX.
void transaction::signature_hash_single(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t flags) const noexcept
{
    const auto write_inputs = [this, &input, &sub, flags](
        writer& sink) noexcept
    {
        const auto& self = **input;
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
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

    const auto write_outputs = [this, &input](
        writer& sink) noexcept
    {
        // Guarded by unversioned_signature_hash.
        const auto index = input_index(input);

        sink.write_variable(add1(index));

        for (size_t output = 0; output < index; ++output)
            sink.write_bytes(null_output());

        outputs_->at(index)->to_data(sink);
    };

    sink.write_4_bytes_little_endian(version_);
    write_inputs(sink);
    write_outputs(sink);
    sink.write_4_bytes_little_endian(locktime_);
    sink.write_4_bytes_little_endian(flags);
}

void transaction::signature_hash_none(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t flags) const noexcept
{
    const auto write_inputs = [this, &input, &sub, flags](
        writer& sink) noexcept
    {
        const auto& self = **input;
        const auto anyone = to_bool(flags & coverage::anyone_can_pay);
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
    sink.write_4_bytes_little_endian(flags);
}

void transaction::signature_hash_all(writer& sink,
    const input_iterator& input, const script& sub,
    uint8_t flags) const noexcept
{
    const auto write_inputs = [this, &input, &sub, flags](
        writer& sink) noexcept
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

    const auto write_outputs = [this](writer& sink) noexcept
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
    uint8_t flags) const noexcept
{
    // Set options.
    const auto flag = mask_sighash(flags);

    // Create hash writer.
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);

    switch (flag)
    {
        case coverage::hash_single:
        {
            //*****************************************************************
            // CONSENSUS: return one_hash if index exceeds outputs in sighash.
            // Bug: https://bitcointalk.org/index.php?topic=260595
            // Expoit: http://joncave.co.uk/2014/08/bitcoin-sighash-single/
            //*****************************************************************
            if (input_index(input) >= outputs_->size())
                return one_hash;

            signature_hash_single(sink, input, sub, flags);
            break;
        }
        case coverage::hash_none:
            signature_hash_none(sink, input, sub, flags);
            break;
        default:
        case coverage::hash_all:
            signature_hash_all(sink, input, sub, flags);
    }

    sink.flush();
    return sha256_hash(sha256);
}

// Signing (version 0).
// ----------------------------------------------------------------------------

// private
// TODO: taproot requires both single and double hash of each.
void transaction::initialize_hash_cache() const noexcept
{
    // This overconstructs the cache (anyone or !all), however it is simple and
    // the same criteria applied by satoshi.
    if (segregated_)
    {
        BC_PUSH_WARNING(NO_NEW_DELETE)
        BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
        cache_.reset(new hash_cache
        {
            outputs_hash(),
            points_hash(),
            sequences_hash()
        });
        BC_POP_WARNING()
        BC_POP_WARNING()
    }
}

// private
hash_digest transaction::output_hash(const input_iterator& input) const noexcept
{
    const auto index = input_index(input);

    //*************************************************************************
    // CONSENSUS: if index exceeds outputs in signature hash, return null_hash.
    //*************************************************************************
    if (index >= outputs_->size())
        return null_hash;

    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

    hash::sha256::copy sink(sha256);
    outputs_->at(index)->to_data(sink);
    sink.flush();
    return sha256_hash(sha256);
}

// private
hash_digest transaction::version_0_signature_hash(const input_iterator& input,
    const script& sub, uint64_t value, uint8_t flags,
    bool bip143) const noexcept
{
    // bip143/v0: the way of serialization is changed.
    if (!bip143)
        return unversioned_signature_hash(input, sub, flags);

    // Set options.
    // C++14: switch in constexpr.
    const auto anyone = to_bool(flags & coverage::anyone_can_pay);
    const auto flag = mask_sighash(flags);
    const auto all = (flag == coverage::hash_all);
    const auto single = (flag == coverage::hash_single);
    const auto& self = **input;

    // Create hash writer.
    BC_PUSH_WARNING(LOCAL_VARIABLE_NOT_INITIALIZED)
    hash_digest sha256;
    BC_POP_WARNING()

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

    self.point().to_data(sink);
    sub.to_data(sink, prefixed);
    sink.write_little_endian(value);
    sink.write_little_endian(self.sequence());

    // outputs
    if (single)
        sink.write_bytes(output_hash(input));
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

hash_digest transaction::signature_hash(const input_iterator& input,
    const script& sub, uint64_t value, uint8_t flags, script_version version,
    bool bip143) const noexcept
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BC_ASSERT(!is_coinbase());

    switch (version)
    {
        case script_version::unversioned:
            return unversioned_signature_hash(input, sub, flags);
        case script_version::zero:
            return version_0_signature_hash(input, sub, value, flags, bip143);
        case script_version::reserved:
        default:
            return {};
    }
}

// This is not used internal to the library.
bool transaction::check_signature(const ec_signature& signature,
    const data_slice& public_key, const script& sub, uint32_t index,
    uint64_t value, uint8_t flags, script_version version,
    bool bip143) const noexcept
{
    if ((index >= inputs_->size()) ||
        signature.empty() || public_key.empty())
        return false;

    const auto sighash = signature_hash(input_at(index), sub, value, flags,
        version, bip143);

    // Validate the EC signature.
    return verify_signature(public_key, sighash, signature);
}

// This is not used internal to the library.
bool transaction::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& sub, uint32_t index, uint64_t value, uint8_t flags,
    script_version version, bool bip143) const noexcept
{
    if (index >= inputs_->size())
        return false;

    out.reserve(max_endorsement_size);
    const auto sighash = signature_hash(input_at(index), sub, value, flags,
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

bool transaction::is_coinbase() const noexcept
{
    return inputs_->size() == one && inputs_->front()->point().is_null();
}

bool transaction::is_internal_double_spend() const noexcept
{
    return !is_distinct(points());
}

// TODO: a pool (non-coinbase) tx must fit into a block (with a coinbase).
bool transaction::is_oversized() const noexcept
{
    return serialized_size(false) > max_block_size;
}

// Guard (contextual).
// ----------------------------------------------------------------------------

// static/private
bool transaction::segregated(const chain::inputs& inputs) noexcept
{
    const auto witnessed = [](const auto& input) noexcept
    {
        return !input.witness().stack().empty();
    };

    return std::any_of(inputs.begin(), inputs.end(), witnessed);
}

// static/private
bool transaction::segregated(const chain::input_cptrs& inputs) noexcept
{
    const auto witnessed = [](const auto& input) noexcept
    {
        return !input->witness().stack().empty();
    };

    return std::any_of(inputs.begin(), inputs.end(), witnessed);
}

bool transaction::is_segregated() const noexcept
{
    return segregated_;
}

size_t transaction::weight() const noexcept
{
    // Block weight is 3 * base size * + 1 * total size (bip141).
    return base_size_contribution * serialized_size(false) +
        total_size_contribution * serialized_size(true);
}

bool transaction::is_overweight() const noexcept
{
    return weight() > max_block_weight;
}

//*****************************************************************************
// CONSENSUS: Legacy sigops are counted in coinbase scripts despite the fact
// that coinbase input scripts are never executed. There is no need to exclude
// p2sh coinbase sigops since there is never a script to count.
//*****************************************************************************
bool transaction::is_signature_operations_limit(bool bip16,
    bool bip141) const noexcept
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;
    return signature_operations(bip16, bip141) > limit;
}

// Check (context free).
// ----------------------------------------------------------------------------

bool transaction::is_empty() const noexcept
{
    return inputs_->empty() || outputs_->empty();
}

bool transaction::is_null_non_coinbase() const noexcept
{
    BC_ASSERT(!is_coinbase());

    const auto invalid = [](const auto& input) noexcept
    {
        return input->point().is_null();
    };

    // True if not coinbase but has null previous_output(s).
    return std::any_of(inputs_->begin(), inputs_->end(), invalid);
}

bool transaction::is_invalid_coinbase_size() const noexcept
{
    BC_ASSERT(is_coinbase());

    // True if coinbase and has invalid input[0] script size.
    const auto script_size = inputs_->front()->script().serialized_size(false);
    return script_size < min_coinbase_size || script_size > max_coinbase_size;
}

// Accept (contextual).
// ----------------------------------------------------------------------------

bool transaction::is_non_final(size_t height, uint32_t timestamp,
    uint32_t median_time_past, bool bip113) const noexcept
{
    // BIP113: comparing the locktime against the median of the past 11 block
    // timestamps, rather than the timestamp of the block including the tx.
    const auto time = bip113 ? median_time_past : timestamp;

    const auto finalized = [](const auto& input) noexcept
    {
        return input->is_final();
    };

    const auto height_time = locktime_ < locktime_threshold ? height : time;

    return !(is_zero(locktime_) || locktime_ < height_time ||
        std::all_of(inputs_->begin(), inputs_->end(), finalized));
}

bool transaction::is_missing_prevouts() const noexcept
{
    BC_ASSERT(!is_coinbase());

    // Invalidity indicates not found.
    const auto missing = [](const auto& input) noexcept
    {
        return !input->prevout->is_valid();
    };

    return std::any_of(inputs_->begin(), inputs_->end(), missing);
}

uint64_t transaction::claim() const noexcept
{
    // Overflow returns max_uint64.
    const auto sum = [](uint64_t total, const auto& output) noexcept
    {
        return ceilinged_add(total, output->value());
    };

    // The amount claimed by outputs.
    return std::accumulate(outputs_->begin(), outputs_->end(), zero, sum);
}

uint64_t transaction::value() const noexcept
{
    // Overflow and coinbase (default) return max_uint64.
    const auto sum = [](uint64_t total, const auto& input) noexcept
    {
        return ceilinged_add(total, input->prevout->value());
    };

    // The amount of prevouts (referenced by inputs).
    return std::accumulate(inputs_->begin(), inputs_->end(), uint64_t{0}, sum);
}

bool transaction::is_overspent() const noexcept
{
    BC_ASSERT(!is_coinbase());

    return claim() > value();
}

//*****************************************************************************
// CONSENSUS: Genesis block is treated as forever immature (satoshi bug).
//*****************************************************************************
bool transaction::is_immature(size_t height) const noexcept
{
    BC_ASSERT(!is_coinbase());

    // Overflow returns max_size_t.
    // Zero is either genesis or not found, either is immature.
    // Spends internal to a block are handled by block validation.
    const auto immature = [=](const auto& input) noexcept
    {
        return input->prevout->coinbase && (is_zero(input->prevout->height) ||
            height < ceilinged_add(input->prevout->height, coinbase_maturity));
    };

    return std::any_of(inputs_->begin(), inputs_->end(), immature);
}

bool transaction::is_locked(size_t height,
    uint32_t median_time_past) const noexcept
{
    // BIP68: not applied to the sequence of the input of a coinbase.
    BC_ASSERT(!is_coinbase());

    // BIP68: applied to txs with a version greater than or equal to two.
    if (version_ < relative_locktime_min_version)
        return false;

    // BIP68: references to median time past are as defined by bip113.
    const auto locked = [=](const auto& input) noexcept
    {
        return input->is_locked(height, median_time_past);
    };

    // BIP68: when the relative lock time is block based, it is interpreted as
    // a minimum block height constraint over the age of the input.
    return std::any_of(inputs_->begin(), inputs_->end(), locked);
}

// Spends internal to a block are handled by block validation.
bool transaction::is_unconfirmed_spend(size_t height) const noexcept
{
    BC_ASSERT(!is_coinbase());

    // Zero is either genesis or not found.
    // Test maturity first to obtain proper error code.
    // Spends internal to a block are handled by block validation.
    const auto unconfirmed = [=](const auto& input) noexcept
    {
        return is_zero(input->prevout->height) &&
            !(height > input->prevout->height);
    };

    return std::any_of(inputs_->begin(), inputs_->end(), unconfirmed);
}

bool transaction::is_confirmed_double_spend(size_t height) const noexcept
{
    BC_ASSERT(!is_coinbase());

    // Spends internal to a block are handled by block validation.
    const auto spent = [=](const auto& input) noexcept
    {
        return input->prevout->spent && height > input->prevout->height;
    };

    return std::any_of(inputs_->begin(), inputs_->end(), spent);
}

// Guards (for tx pool without compact blocks).
// ----------------------------------------------------------------------------

code transaction::guard() const noexcept
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

code transaction::guard(const context& state) const noexcept
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

code transaction::check() const noexcept
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

code transaction::accept(const context& state) const noexcept
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

// Connect (contextual).
// ------------------------------------------------------------------------

code transaction::connect(const context& state) const noexcept
{
    code ec;

    // Cache witness hash components that don't change per input.
    initialize_hash_cache();
    
    const auto is_roller = [](const auto& input) noexcept
    {
        static const auto roll = operation{ opcode::roll };

        // Naive implementation, any op_roll in either script, late-counted.
        // TODO: precompute on script parse, tune using performance profiling.
        return contains(input.script().ops(), roll)
            || contains(input.prevout->script().ops(), roll);
    };

    // Validate scripts, skip coinbase.
    for (auto input = inputs_->begin(); input != inputs_->end(); ++input)
    {
        using namespace machine;

        // Evaluate rolling scripts with linear search but constant erase.
        // Evaluate non-rolling scripts with constant search but linear erase.
        if ((ec = is_roller(**input) ?
            interpreter<linked_stack>::connect(state, *this, input) :
            interpreter<contiguous_stack>::connect(state, *this, input)))
            return ec;
    }

    return error::transaction_success;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

transaction tag_invoke(json::value_to_tag<transaction>,
    const json::value& value) noexcept
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
    const transaction& tx) noexcept
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
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<transaction>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const transaction::cptr& tx) noexcept
{
    tag_invoke(tag, value, *tx);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
