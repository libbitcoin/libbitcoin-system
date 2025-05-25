/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <numeric>
#include <utility>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
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

// Constructors.
// ----------------------------------------------------------------------------

transaction::transaction() NOEXCEPT
  : transaction(0,
      to_shared<input_cptrs>(),
      to_shared<output_cptrs>(),
      0, false, false)
{
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

transaction::transaction(uint32_t version, const inputs_cptr& inputs,
    const outputs_cptr& outputs, uint32_t locktime) NOEXCEPT
  : transaction(version, inputs, outputs, locktime, segregated(*inputs), true)
{
}

transaction::transaction(stream::in::fast&& stream, bool witness) NOEXCEPT
  : transaction(read::bytes::fast(stream), witness)
{
}

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
  : transaction(source, witness)
{
}

transaction::transaction(reader& source, bool witness) NOEXCEPT
  : version_(source.read_4_bytes_little_endian()),
    inputs_(CREATE(input_cptrs, source.get_allocator())),
    outputs_(CREATE(output_cptrs, source.get_allocator()))
{
    assign_data(source, witness);
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

// private
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
void transaction::assign_data(reader& source, bool witness) NOEXCEPT
{
    auto& allocator = source.get_allocator();
    auto ins = to_non_const_raw_ptr(inputs_);
    auto count = source.read_size(max_block_size);
    ins->reserve(count);
    for (size_t in = 0; in < count; ++in)
        ins->emplace_back(CREATE(input, allocator, source));

    // Expensive repeated recomputation, so cache segregated state.
    // Detect witness as no inputs (marker) and expected flag [bip144].
    segregated_ = 
        inputs_->size() == witness_marker &&
        source.peek_byte() == witness_enabled;

    if (segregated_)
    {
        // Skip over the peeked witness flag.
        source.skip_byte();

        count = source.read_size(max_block_size);
        ins->reserve(count);
        for (size_t in = 0; in < count; ++in)
            ins->emplace_back(CREATE(input, allocator, source));

        auto outs = to_non_const_raw_ptr(outputs_);
        count = source.read_size(max_block_size);
        outs->reserve(count);
        for (size_t out = 0; out < count; ++out)
            outs->emplace_back(CREATE(output, allocator, source));

        // Read or skip witnesses as specified.
        if (witness)
        {
            for (auto& input: *inputs_)
                to_non_const_raw_ptr(input)->set_witness(source);
        }
        else
        {
            // Default witness is populated on input construct.
            for (size_t in = 0; in < inputs_->size(); ++in)
                witness::skip(source, true);
        }
    }
    else
    {
        auto outs = to_non_const_raw_ptr(outputs_);
        count = source.read_size(max_block_size);
        outs->reserve(count);
        for (size_t out = 0; out < count; ++out)
            outs->emplace_back(CREATE(output, allocator, source));
    }

    locktime_ = source.read_4_bytes_little_endian();
    size_ = serialized_size(*inputs_, *outputs_, segregated_);
    valid_ = source;
}
BC_POP_WARNING()

// Serialization.
// ----------------------------------------------------------------------------

// Transactions with empty witnesses always use old serialization [bip144].
// If no inputs are witness programs then witness hash is tx hash [bip141].
data_chunk transaction::to_data(bool witness) const NOEXCEPT
{
    witness &= segregated_;

    data_chunk data(serialized_size(witness));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, witness);
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
transaction::sizes transaction::serialized_size(const input_cptrs& inputs,
    const output_cptrs& outputs, bool segregated) NOEXCEPT
{
    sizes size{ zero, zero };

    std::for_each(inputs.begin(), inputs.end(), [&](const auto& in) NOEXCEPT
    {
        size.nominal = ceilinged_add(size.nominal, in->nominal_size());
        if (segregated)
            size.witnessed = ceilinged_add(size.witnessed, in->witnessed_size());
    });

    const auto outs = [](size_t total, const auto& output) NOEXCEPT
    {
        return ceilinged_add(total, output->serialized_size());
    };

    constexpr auto base_const_size = sizeof(version_) + sizeof(locktime_);
    constexpr auto witness_const_size = sizeof(witness_marker) +
        sizeof(witness_enabled);

    const auto base_size =
        ceilinged_add(ceilinged_add(ceilinged_add(base_const_size,
            variable_size(inputs.size())), variable_size(outputs.size())),
            std::accumulate(outputs.begin(), outputs.end(), zero, outs));

    const auto nominal_size = ceilinged_add(base_size, size.nominal);

    // For non-segregated transactions, witnessed_size is nominal_size.
    const auto witnessed_size = segregated ? ceilinged_add(ceilinged_add(
        base_size, witness_const_size), size.witnessed) : nominal_size;

    // For non-segregated transactions, values are the same.
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

size_t transaction::spends() const NOEXCEPT
{
    return is_coinbase() ? zero : inputs_->size();
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
    // Overflow returns max_size_t.
    const auto in = [=](size_t total, const auto& input) NOEXCEPT
    {
        const auto add = input->signature_operations(bip16, bip141);
        return ceilinged_add(total, add);
    };

    // Overflow returns max_size_t.
    const auto out = [=](size_t total, const auto& output) NOEXCEPT
    {
        const auto add = output->signature_operations(bip141);
        return ceilinged_add(total, add);
    };

    // Overflow returns max_size_t.
    return ceilinged_add(
        std::accumulate(inputs_->begin(), inputs_->end(), zero, in),
        std::accumulate(outputs_->begin(), outputs_->end(), zero, out));
}

// private
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

// Signatures (public).
// ----------------------------------------------------------------------------

transaction::input_iterator transaction::input_at(
    uint32_t index) const NOEXCEPT
{
    // Guarded by check_signature and create_endorsement.
    BC_ASSERT_MSG(index < inputs_->size(), "invalid input index");

    return std::next(inputs_->begin(), index);
}

// This is not used internal to the library.
bool transaction::check_signature(const ec_signature& signature,
    const data_slice& public_key, const script& subscript, uint32_t index,
    uint64_t value, uint8_t sighash_flags, script_version version,
    uint32_t flags) const NOEXCEPT
{
    if ((index >= inputs_->size()) || signature.empty() || public_key.empty())
        return false;

    hash_digest sighash{};
    const hash_cptr unused{};
    if (!signature_hash(sighash, input_at(index), subscript, value,
        unused, version, sighash_flags, flags))
        return false;

    // Validate the ECDSA signature.
    return ecdsa::verify_signature(public_key, sighash, signature);
}

// This is not used internal to the library.
bool transaction::create_endorsement(endorsement& out, const ec_secret& secret,
    const script& subscript, uint32_t index, uint64_t value,
    uint8_t sighash_flags, script_version version,
    uint32_t flags) const NOEXCEPT
{
    if (index >= inputs_->size())
        return false;

    hash_digest sighash{};
    const hash_cptr unused{};
    out.reserve(max_endorsement_size);
    if (!signature_hash(sighash, input_at(index), subscript, value, unused,
        version, sighash_flags, flags))
        return false;

    // Create the ECDSA signature and encode as DER.
    ec_signature signature;
    if (!ecdsa::sign(signature, secret, sighash) ||
        !ecdsa::encode_signature(out, signature))
        return false;

    // Add the sighash type to the end of the DER signature -> endorsement.
    out.push_back(sighash_flags);
    ////out.shrink_to_fit();
    return true;
}

// Signature hashing (common).
// ----------------------------------------------------------------------------

uint32_t transaction::input_index(const input_iterator& input) const NOEXCEPT
{
    return possible_narrow_and_sign_cast<uint32_t>(
        std::distance(inputs_->begin(), input));
}

//*****************************************************************************
// CONSENSUS: if index exceeds outputs in signature hash, return one_hash.
// Related Bug: bitcointalk.org/index.php?topic=260595
// Exploit: joncave.co.uk/2014/08/bitcoin-sighash-single/
//*****************************************************************************
bool transaction::output_overflow(size_t input) const NOEXCEPT
{
    return input >= outputs_->size();
}

// There are three versions of signature hashing and verification.
// Version: (unversioned) original, (v0) bip143/segwit, (v1) bip341/taproot.
bool transaction::signature_hash(hash_digest& out, const input_iterator& input,
    const script& subscript, uint64_t value, const hash_cptr& tapleaf,
    script_version version, uint8_t sighash_flags, uint32_t flags) const NOEXCEPT
{
    // There is no rational interpretation of a signature hash for a coinbase.
    BC_ASSERT(!is_coinbase());

    // bip143: the method of signature hashing is changed for v0 scripts.
    // bip342: the method of signature hashing is changed for v1 scripts.
    const auto bip143 = script::is_enabled(flags, flags::bip143_rule);
    const auto bip342 = script::is_enabled(flags, flags::bip342_rule);

    // This is where the connection between bip141 and bip143 is made. If a
    // versioned 1 program (segwit) extracted by bip141 but bip143 (segwit
    // hashing) is not active, then drop down to unversioned signature hashing.
    if (bip143 && version == script_version::segwit)
        return version0_sighash(out, input, subscript, value, sighash_flags);

    // This is where the connection between bip341 and bip342 is made. If a
    // version 2 program (taproot) extracted by bip341 but bip342 (tapscript)
    // is not active then drop down to unversioned signature hashing. 
    if (bip342 && version == script_version::taproot)
        return version1_sighash(out, input, subscript, value, tapleaf,
            sighash_flags);

    // Given above forks are documented to activate together, this distinction
    // is moot, however these are distinct BIPs and therefore must be either be
    // differentiated as such in code, or the BIP distiction would be ignored.
    return unversioned_sighash(out, input, subscript, sighash_flags);
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
bool transaction::segregated(const input_cptrs& inputs) NOEXCEPT
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
    // Block weight is 3 * base size * + 1 * total size [bip141].
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
bool transaction::is_signature_operations_limited(bool bip16,
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

bool transaction::is_absolute_locked(size_t height, uint32_t timestamp,
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

// static
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

// static
bool transaction::is_relative_locktime_applied(bool coinbase, uint32_t version,
    uint32_t sequence) NOEXCEPT
{
    // BIP68: not applied to the sequence of the input of a coinbase.
    // BIP68: if bit 31 is set then no consensus meaning is applied.
    // BIP68: applied to txs with a version greater than or equal to two.
    return !coinbase && input::is_relative_locktime_applied(sequence) &&
        (version >= relative_locktime_min_version);
}

bool transaction::is_internally_locked(const input& in) const NOEXCEPT
{
    // BIP68: not applied to the sequence of the input of a coinbase.
    BC_ASSERT(!is_coinbase());

    // BIP68: applied to txs with a version greater than or equal to two.
    if (version_ < relative_locktime_min_version)
        return false;

    // Internal spends have no relative height/mtp (own metadata vs. itself).
    return in.is_relative_locked(in.metadata.height,
        in.metadata.median_time_past);
}

bool transaction::is_relative_locked(size_t height,
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
        return input->is_relative_locked(height, median_time_past);
    };

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
    if (is_signature_operations_limited(bip16, bip141))
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

    if (is_absolute_locked(ctx.height, ctx.timestamp, ctx.median_time_past, bip113))
        return error::absolute_time_locked;

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
    if (bip68 && is_relative_locked(ctx.height, ctx.median_time_past))
        return error::relative_time_locked;
    if (is_immature(ctx.height))
        return error::coinbase_maturity;
    if (is_unconfirmed_spend(ctx.height))
        return error::unconfirmed_spend;
    if (is_confirmed_double_spend(ctx.height))
        return error::confirmed_double_spend;

    return error::transaction_success;
}

// Delegated.
// ----------------------------------------------------------------------------

code transaction::connect_input(const context& ctx,
    const input_iterator& it) const NOEXCEPT
{
    using namespace machine;

    // TODO: evaluate performance tradeoff.
    if ((*it)->is_roller())
    {
        // Evaluate rolling scripts with linear search but constant erase.
        return interpreter<linked_stack>::connect(ctx, *this, it);
    }

    // Evaluate non-rolling scripts with constant search but linear erase.
    return interpreter<contiguous_stack>::connect(ctx, *this, it);
}

// Connect (contextual).
// ----------------------------------------------------------------------------
// TODO: accumulate sigops from each connect result and add coinbase.
// TODO: return in override with out parameter. more impactful with segwit.

// forks

code transaction::connect(const context& ctx) const NOEXCEPT
{
    ////BC_ASSERT(!is_coinbase());

    if (is_coinbase())
        return error::transaction_success;

    for (auto in = inputs_->begin(); in != inputs_->end(); ++in)
        if (const auto ec = connect_input(ctx, in))
            return ec;

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
        { "inputs", json::value_from(*tx.inputs_ptr()) },
        { "outputs", json::value_from(*tx.outputs_ptr()) },
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
