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
#include <bitcoin/system/chain/input.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Product overflows guarded by script size limit.
static_assert(max_script_size < 
    max_size_t / multisig_default_sigops / heavy_sigops_factor,
    "input sigop overflow guard");

// Constructors.
// ----------------------------------------------------------------------------

// Default point is null_hash and point::null_index. 
// Default metadata is spent, invalid, max_size_t value. 
input::input() NOEXCEPT
  : input(
      to_shared<chain::point>(),
      to_shared<chain::script>(),
      to_shared<chain::witness>(),
      0, false)
{
}

input::input(chain::point&& point, chain::script&& script,
    uint32_t sequence) NOEXCEPT
  : input(
      to_shared(std::move(point)),
      to_shared(std::move(script)),
      to_shared<chain::witness>(),
      sequence, true)
{
}

input::input(const chain::point& point, const chain::script& script,
    uint32_t sequence) NOEXCEPT
  : input(
      to_shared(point),
      to_shared(script),
      to_shared<chain::witness>(),
      sequence, true)
{
}

input::input(const chain::point::cptr& point,
    const chain::script::cptr& script, uint32_t sequence) NOEXCEPT
  : input(
      point ? point : to_shared<chain::point>(),
      script ? script : to_shared<chain::script>(),
      to_shared<chain::witness>(),
      sequence, true)
{
}

input::input(chain::point&& point, chain::script&& script,
    chain::witness&& witness, uint32_t sequence) NOEXCEPT
  : input(
      to_shared(std::move(point)),
      to_shared(std::move(script)),
      to_shared(std::move(witness)),
      sequence, true)
{
}

input::input(const chain::point& point, const chain::script& script,
    const chain::witness& witness, uint32_t sequence) NOEXCEPT
  : input(
      to_shared(point),
      to_shared(script),
      to_shared(witness),
      sequence, true)
{
}

input::input(const chain::point::cptr& point, const chain::script::cptr& script,
    const chain::witness::cptr& witness, uint32_t sequence) NOEXCEPT
  : input(point, script, witness, sequence, true)
{
}

input::input(const data_slice& data) NOEXCEPT
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
  : input(stream::in::copy(data))
    BC_POP_WARNING()
{
}

////input::input(stream::in::fast&& stream) NOEXCEPT
////  : input(read::bytes::fast(stream))
////{
////}

input::input(stream::in::fast& stream) NOEXCEPT
  : input(read::bytes::fast(stream))
{
}

input::input(std::istream&& stream) NOEXCEPT
  : input(read::bytes::istream(stream))
{
}

input::input(std::istream& stream) NOEXCEPT
  : input(read::bytes::istream(stream))
{
}

input::input(reader&& source) NOEXCEPT
  : input(from_data(source))
{
}

input::input(reader& source) NOEXCEPT
  : input(from_data(source))
{
}

// protected
input::input(const chain::point::cptr& point, const chain::script::cptr& script,
    const chain::witness::cptr& witness, uint32_t sequence, bool valid) NOEXCEPT
  : point_(point),
    script_(script),
    witness_(witness),
    sequence_(sequence),
    valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool input::operator==(const input& other) const NOEXCEPT
{
    return (sequence_ == other.sequence_)
        && (point_ == other.point_ || *point_ == *other.point_)
        && (script_ == other.script_ || *script_ == *other.script_)
        && (witness_ == other.witness_ || *witness_ == *other.witness_);
}

bool input::operator!=(const input& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
input input::from_data(reader& source) NOEXCEPT
{
    // Witness is deserialized by transaction.
    return
    {
        to_shared<chain::point>(source),
        to_shared<chain::script>(source, true),
        to_shared<chain::witness>(),
        source.read_4_bytes_little_endian(),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk input::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size(false));

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    stream::out::copy ostream(data);
    BC_POP_WARNING()

    to_data(ostream);
    return data;
}

void input::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

// Witness is serialized by transaction.
void input::to_data(writer& sink) const NOEXCEPT
{
    point_->to_data(sink);
    script_->to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence_);
}

size_t input::serialized_size(bool witness) const NOEXCEPT
{
    // input.serialized_size(witness) provides sizing for witness, however
    // witnesses are serialized by the transaction. This is an ugly hack as a
    // consequence of bip144 not serializing witnesses as part of inputs, which
    // is logically the proper association.
    return point_->serialized_size()
        + script_->serialized_size(true)
        + (witness ? witness_->serialized_size(true) : zero)
        + sizeof(sequence_);
}

// Properties.
// ----------------------------------------------------------------------------

bool input::is_valid() const NOEXCEPT
{
    return valid_;
}

const point& input::point() const NOEXCEPT
{
    return *point_;
}

const chain::script& input::script() const NOEXCEPT
{
    return *script_;
}

const chain::witness& input::witness() const NOEXCEPT
{
    return *witness_;
}

const point::cptr& input::point_ptr() const NOEXCEPT
{
    return point_;
}

const chain::script::cptr& input::script_ptr() const NOEXCEPT
{
    return script_;
}

const chain::witness::cptr& input::witness_ptr() const NOEXCEPT
{
    return witness_;
}

uint32_t input::sequence() const NOEXCEPT
{
    return sequence_;
}

// Methods.
// ----------------------------------------------------------------------------

bool input::is_final() const NOEXCEPT
{
    return sequence_ == max_input_sequence;
}

// static
bool input::is_locked(uint32_t sequence, size_t height,
    uint32_t median_time_past, size_t prevout_height,
    uint32_t prevout_median_time_past) NOEXCEPT
{
    // BIP68: if bit 31 is set then no consensus meaning is applied.
    if (get_right(sequence, relative_locktime_disabled_bit))
        return false;

    // BIP68: the low 16 bits of the sequence apply to relative lock-time.
    const auto blocks = mask_left(sequence, relative_locktime_mask_left);

    // BIP68: bit 22 determines if relative lock is time or block based.
    if (get_right(sequence, relative_locktime_time_locked_bit))
    {
        // BIP68: change sequence to seconds by shift up by 9 bits (x 512).
        auto time = shift_left(blocks, relative_locktime_seconds_shift_left);
        auto age = floored_subtract(median_time_past, prevout_median_time_past);
        return age < time;
    }

    const auto age = floored_subtract(height, prevout_height);
    return age < blocks;
}

bool input::is_locked(size_t height, uint32_t median_time_past) const NOEXCEPT
{
    // Prevout must be found and height/median_time_past metadata populated.
    ////BC_ASSERT(!is_zero(metadata.height));
    return is_locked(sequence_, height, median_time_past, metadata.height,
        metadata.median_time_past);
}

bool input::reserved_hash(hash_digest& out) const NOEXCEPT
{
    if (!witness::is_reserved_pattern(witness_->stack()))
        return false;

    std::copy_n(witness_->stack().front()->begin(), hash_size, out.begin());
    return true;
}

// private
// prevout_script is only used to determine is_pay_script_hash_pattern.
bool input::extract_sigop_script(chain::script& out,
    const chain::script& prevout_script) const NOEXCEPT
{
    // There are no embedded sigops when the prevout script is not p2sh.
    if (!script::is_pay_script_hash_pattern(prevout_script.ops()))
        return false;

    // There are no embedded sigops when the input script is not push only.
    const auto& ops = script_->ops();
    if (ops.empty() || !script::is_relaxed_push(ops))
        return false;

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    out = { ops.back().data(), false };
    return true;
}

// TODO: Prior to block 79400 sigops were limited only by policy.
// TODO: Create legacy sigops fork flag and pass here, return 0 if false.
// TODO: this was an unbipped flag day soft fork, prior to BIP16/141.
// TODO: if (nHeight > 79400 && GetSigOpCount() > MAX_BLOCK_SIGOPS).
size_t input::signature_operations(bool bip16, bool bip141) const NOEXCEPT
{
    // Penalize quadratic signature operations (bip141).
    const auto factor = bip141 ? heavy_sigops_factor : one;
    const auto sigops = script_->signature_operations(false) * factor;

    // ************************************************************************
    // CONSENSUS: coinbase input cannot execute, but sigops are counted anyway.
    // ************************************************************************
    if (!prevout)
        return sigops;

    // Null prevout/input (coinbase) cannot have witness or embedded script.
    // Embedded/witness scripts are deserialized here and again on scipt eval.

    chain::script witness;
    if (bip141 && witness_->extract_sigop_script(witness, prevout->script()))
    {
        // Add sigops in the witness script (bip141).
        return ceilinged_add(sigops, witness.signature_operations(true));
    }

    chain::script embedded;
    if (bip16 && extract_sigop_script(embedded, prevout->script()))
    {
        if (bip141 && witness_->extract_sigop_script(witness, embedded))
        {
            // Add sigops in the embedded witness script (bip141).
            return ceilinged_add(sigops, witness.signature_operations(true));
        }
        else
        {
            // Add heavy sigops in the embedded script (bip16).
            return ceilinged_add(sigops, embedded.signature_operations(true) *
                factor);
        }
    }

    return sigops;
}

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

input tag_invoke(json::value_to_tag<input>, const json::value& value) NOEXCEPT
{
    return
    {
        json::value_to<chain::point>(value.at("point")),
        json::value_to<chain::script>(value.at("script")),
        json::value_to<chain::witness>(value.at("witness")),
        value.at("sequence").to_number<uint32_t>()
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const input& input) NOEXCEPT
{
    value =
    {
        { "point", input.point() },
        { "script", input.script() },
        { "witness", input.witness() },
        { "sequence", input.sequence() }
    };
}

BC_POP_WARNING()

input::cptr tag_invoke(json::value_to_tag<input::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<input>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const input::cptr& input) NOEXCEPT
{
    tag_invoke(tag, value, *input);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
