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
#include <bitcoin/system/chain/input.hpp>

#include <algorithm>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
// ----------------------------------------------------------------------------

// Valid default used in signature hashing.
input::input()
  : input({}, {}, {}, 0, true)
{
}

input::input(input&& other)
  : input(
      std::move(other.point_),
      std::move(other.script_),
      std::move(other.witness_),
      other.sequence_,
      other.valid_)
{
}

input::input(const input& other)
  : input(
      other.point_,
      other.script_,
      other.witness_,
      other.sequence_,
      other.valid_)
{
}

input::input(chain::point&& point, chain::script&& script, uint32_t sequence)
  : input(std::move(point), std::move(script), {}, sequence, true)
{
}

input::input(const chain::point& point, const chain::script& script,
    uint32_t sequence)
  : input(point, script, {}, sequence, true)
{
}

input::input(chain::point&& point, chain::script&& script,
    chain::witness&& witness, uint32_t sequence)
  : input(std::move(point), std::move(script), std::move(witness), sequence,
      true)
{
}

input::input(const chain::point& point, const chain::script& script,
    const chain::witness& witness, uint32_t sequence)
  : input(point, script, witness, sequence, true)
{
}

input::input(const data_chunk& data)
{
    from_data(data);
}

input::input(std::istream& stream)
{
    from_data(stream);
}

input::input(reader& source)
{
    from_data(source);
}

// protected
input::input(chain::point&& point, chain::script&& script,
    chain::witness&& witness, uint32_t sequence, bool valid)
  : point_(std::move(point)),
    script_(std::move(script)),
    witness_(std::move(witness)),
    sequence_(sequence),
    valid_(valid),
    prevout{}
{
}

// protected
input::input(const chain::point& point, const chain::script& script,
    const chain::witness& witness, uint32_t sequence, bool valid)
  : point_(point),
    script_(script),
    witness_(witness),
    sequence_(sequence),
    valid_(valid),
    prevout{}
{
}

// Operators.
// ----------------------------------------------------------------------------

input& input::operator=(input&& other)
{
    point_ = std::move(other.point_);
    script_ = std::move(other.script_);
    witness_ = std::move(other.witness_);
    sequence_ = other.sequence_;
    valid_ = other.valid_;
    return *this;
}

input& input::operator=(const input& other)
{
    point_ = other.point_;
    script_ = other.script_;
    witness_ = other.witness_;
    sequence_ = other.sequence_;
    valid_ = other.valid_;
    return *this;
}

bool input::operator==(const input& other) const
{
    return (sequence_ == other.sequence_)
        && (point_ == other.point_)
        && (script_ == other.script_)
        && (witness_ == other.witness_);
}

bool input::operator!=(const input& other) const
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

bool input::from_data(const data_chunk& data)
{
    read::bytes::copy reader(data);
    return from_data(reader);
}

bool input::from_data(std::istream& stream)
{
    read::bytes::istream reader(stream);
    return from_data(reader);
}

// Witness is deserialized by transaction.
bool input::from_data(reader& source)
{
    reset();

    point_.from_data(source);
    script_.from_data(source, true);
    sequence_ = source.read_4_bytes_little_endian();
    witness_.reset();

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

void input::reset()
{
    point_.reset();
    script_.reset();
    witness_.reset();
    sequence_ = 0;
    valid_ = false;
}

bool input::is_valid() const
{
    return valid_;
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk input::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(false));
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void input::to_data(std::ostream& stream) const
{
    write::bytes::ostream out(stream);
    to_data(out);
}

// Witness is serialized by transaction.
void input::to_data(writer& sink) const
{
    DEBUG_ONLY(const auto size = serialized_size(false);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    point_.to_data(sink);
    script_.to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence_);

    BITCOIN_ASSERT(sink && sink.get_position() - start == size);
}

size_t input::serialized_size(bool witness) const
{
    // input.serialized_size(witness) provides sizing for witness, however
    // witnesses are serialized by the transaction. This is an ugly hack as a
    // consequence of bip144 not serializing witnesses as part of inputs, which
    // is logically the proper association.
    return point_.serialized_size()
        + script_.serialized_size(true)
        + (witness ? witness_.serialized_size(true) : zero)
        + sizeof(sequence_);
}

// Properties.
// ----------------------------------------------------------------------------

const point& input::point() const
{
    return point_;
}

const chain::script& input::script() const
{
    return script_;
}

const chain::witness& input::witness() const
{
    return witness_;
}

uint32_t input::sequence() const
{
    return sequence_;
}

// Methods.
// ----------------------------------------------------------------------------

bool input::is_final() const
{
    return sequence_ == max_input_sequence;
}

bool input::is_locked(size_t height, uint32_t median_time_past) const
{
    // BIP68: if bit 31 is set then no consensus meaning is applied.
    if (get_right(sequence_, relative_locktime_disabled_bit))
        return false;

    // BIP68: the low 16 bits of the sequence apply to relative lock-time.
    const auto blocks = mask_left(sequence_, relative_locktime_mask_left);

    // BIP68: bit 22 determines if relative lock is time or block based.
    if (get_right(sequence_, relative_locktime_time_locked_bit))
    {
        // BIP68: change sequence to seconds by shifting up by 9 bits (x 512).
        auto time = shift_left(blocks, relative_locktime_seconds_shift_left);
        auto age = floored_subtract(median_time_past, prevout.median_time_past);
        return age < time;
    }

    auto age = floored_subtract(height, prevout.height);
    return age < blocks;
}

bool input::reserved_hash(hash_digest& out) const
{
    const auto& stack = witness_.stack();

    if (!witness::is_reserved_pattern(stack))
        return false;

    std::copy_n(stack.front().begin(), hash_size, out.begin());
    return true;
}

// private
bool input::embedded_script(chain::script& out) const
{
    const auto& ops = script_.operations();
    const auto& script = prevout.script();

    // There are no embedded sigops when the prevout script is not p2sh.
    if (!script::is_pay_script_hash_pattern(script.operations()))
        return false;

    // There are no embedded sigops when the input script is not push only.
    // The first operations access must be method-based to guarantee the cache.
    if (ops.empty() || !script::is_relaxed_push(ops))
        return false;

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    return out.from_data(ops.back().data(), false);
}

// Product overflows guarded by script size limit.
static_assert(max_script_size < 
    max_size_t / multisig_default_sigops / heavy_sigops_factor,
    "input sigop overflow guard");

// TODO: Prior to block 79400 sigops were limited only by policy.
// TODO: Create legacy sigops fork flag and pass here, return 0 if false.
// TODO: this was an unbipped flag day soft fork, prior to BIP16/141.
// TODO: if (nHeight > 79400 && GetSigOpCount() > MAX_BLOCK_SIGOPS).
size_t input::signature_operations(bool bip16, bool bip141) const
{
    if (!prevout.is_valid())
        return max_size_t;

    chain::script witness, embedded;
    const auto& output = prevout.script();

    // Penalize quadratic signature operations (bip141).
    const auto factor = bip141 ? heavy_sigops_factor : one;

    // Count heavy sigops in the input script.
    auto sigops = script_.sigops(false) * factor;

    if (bip141 && witness_.extract_sigop_script(witness, output))
    {
        // Add sigops in the witness script (bip141).
        return ceilinged_add(sigops, witness.sigops(true));
    }

    if (bip16 && embedded_script(embedded))
    {
        if (bip141 && witness_.extract_sigop_script(witness, embedded))
        {
            // Add sigops in the embedded witness script (bip141).
            return ceilinged_add(sigops, witness.sigops(true));
        }
        else
        {
            // Add heavy sigops in the embedded script (bip16).
            return ceilinged_add(sigops, embedded.sigops(true) * factor);
        }
    }

    return sigops;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
