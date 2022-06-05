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
#include <bitcoin/system/chain/input.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
// ----------------------------------------------------------------------------

// Default point is null_hash and point::null_index. 
// Default prevout (metadata) is spent, invalid, max_size_t value. 
input::input() noexcept
  : input(
      to_shared<chain::point>(),
      to_shared<chain::script>(),
      to_shared<chain::witness>(), 0, false,
      to_shared<chain::prevout>())
{
}

input::input(chain::point&& point, chain::script&& script,
    uint32_t sequence) noexcept
  : input(
      to_shared(std::move(point)),
      to_shared(std::move(script)),
      to_shared<chain::witness>(), sequence, true,
      to_shared<chain::prevout>())
{
}

input::input(const chain::point& point, const chain::script& script,
    uint32_t sequence) noexcept
  : input(
      to_shared(point),
      to_shared(script),
      to_shared<chain::witness>(), sequence, true,
      to_shared<chain::prevout>())
{
}

input::input(const chain::point::cptr& point,
    const chain::script::cptr& script, uint32_t sequence) noexcept
  : input(
      point ? point : to_shared<chain::point>(),
      script ? script : to_shared<chain::script>(),
      std::make_shared<chain::witness>(), sequence, true,
      std::make_shared<chain::prevout>())
{
}

input::input(chain::point&& point, chain::script&& script,
    chain::witness&& witness, uint32_t sequence) noexcept
  : input(
      to_shared(std::move(point)),
      to_shared(std::move(script)),
      to_shared(std::move(witness)), sequence, true,
      to_shared<chain::prevout>())
{
}

input::input(const chain::point& point, const chain::script& script,
    const chain::witness& witness, uint32_t sequence) noexcept
  : input(
      to_shared(point),
      to_shared(script),
      to_shared(witness), sequence, true,
      to_shared<chain::prevout>())
{
}

input::input(const chain::point::cptr& point, const chain::script::cptr& script,
    const chain::witness::cptr& witness, uint32_t sequence) noexcept
  : input(point, script, witness, sequence, true, to_shared<chain::prevout>())
{
}

input::input(const data_slice& data) noexcept
  : input(stream::in::copy(data))
{
}

input::input(std::istream&& stream) noexcept
  : input(read::bytes::istream(stream))
{
}

input::input(std::istream& stream) noexcept
  : input(read::bytes::istream(stream))
{
}

input::input(reader&& source) noexcept
  : input(from_data(source))
{
}

input::input(reader& source) noexcept
  : input(from_data(source))
{
}

// protected
input::input(const chain::point::cptr& point, const chain::script::cptr& script,
    const chain::witness::cptr& witness, uint32_t sequence, bool valid,
    const chain::prevout::cptr& prevout) noexcept
  : point_(point),
    script_(script),
    witness_(witness),
    sequence_(sequence),
    valid_(valid),
    prevout(prevout)
{
}

// Operators.
// ----------------------------------------------------------------------------

bool input::operator==(const input& other) const noexcept
{
    return (sequence_ == other.sequence_)
        && (*point_ == *other.point_)
        && (*script_ == *other.script_)
        && (*witness_ == *other.witness_);
}

bool input::operator!=(const input& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
input input::from_data(reader& source) noexcept
{
    // Witness is deserialized by transaction.
    return
    {
        to_shared(new chain::point{ source }),
        to_shared(new chain::script{ source, true }),
        to_shared<chain::witness>(),
        source.read_4_bytes_little_endian(),
        source,
        to_shared<chain::prevout>()
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk input::to_data() const noexcept
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(false));
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void input::to_data(std::ostream& stream) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out);
}

// Witness is serialized by transaction.
void input::to_data(writer& sink) const noexcept
{
    point_->to_data(sink);
    script_->to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence_);
}

size_t input::serialized_size(bool witness) const noexcept
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

bool input::is_valid() const noexcept
{
    return valid_;
}

const point& input::point() const noexcept
{
    return *point_;
}

const chain::script& input::script() const noexcept
{
    return *script_;
}

const chain::witness& input::witness() const noexcept
{
    return *witness_;
}

const point::cptr& input::point_ptr() const noexcept
{
    return point_;
}

const chain::script::cptr& input::script_ptr() const noexcept
{
    return script_;
}

const chain::witness::cptr& input::witness_ptr() const noexcept
{
    return witness_;
}

uint32_t input::sequence() const noexcept
{
    return sequence_;
}

// Methods.
// ----------------------------------------------------------------------------

bool input::is_final() const noexcept
{
    return sequence_ == max_input_sequence;
}

bool input::is_locked(size_t height, uint32_t median_time_past) const noexcept
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
        auto age = floored_subtract(median_time_past, prevout->median_time_past);
        return age < time;
    }

    auto age = floored_subtract(height, prevout->height);
    return age < blocks;
}

bool input::reserved_hash(hash_digest& out) const noexcept
{
    if (!witness::is_reserved_pattern(witness_->stack()))
        return false;

    std::copy_n(witness_->stack().front()->begin(), hash_size, out.begin());
    return true;
}

// private
bool input::embedded_script(chain::script& out) const noexcept
{
    const auto& ops = script_->ops();
    const auto& script = prevout->script();

    // There are no embedded sigops when the prevout script is not p2sh.
    if (!script::is_pay_script_hash_pattern(script.ops()))
        return false;

    // There are no embedded sigops when the input script is not push only.
    // The first operations access must be method-based to guarantee the cache.
    if (ops.empty() || !script::is_relaxed_push(ops))
        return false;

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    out = { ops.back().data(), false };
    return true;
}

// Product overflows guarded by script size limit.
static_assert(max_script_size < 
    max_size_t / multisig_default_sigops / heavy_sigops_factor,
    "input sigop overflow guard");

// TODO: Prior to block 79400 sigops were limited only by policy.
// TODO: Create legacy sigops fork flag and pass here, return 0 if false.
// TODO: this was an unbipped flag day soft fork, prior to BIP16/141.
// TODO: if (nHeight > 79400 && GetSigOpCount() > MAX_BLOCK_SIGOPS).
size_t input::signature_operations(bool bip16, bool bip141) const noexcept
{
    if (bip141 && !prevout->is_valid())
        return max_size_t;

    chain::script witness, embedded;

    // Penalize quadratic signature operations (bip141).
    const auto factor = bip141 ? heavy_sigops_factor : one;

    // Count heavy sigops in the input script.
    auto sigops = script_->sigops(false) * factor;

    if (bip141 && witness_->extract_sigop_script(witness, prevout->script()))
    {
        // Add sigops in the witness script (bip141).
        return ceilinged_add(sigops, witness.sigops(true));
    }

    if (bip16 && embedded_script(embedded))
    {
        if (bip141 && witness_->extract_sigop_script(witness, embedded))
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

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have noexcept: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

input tag_invoke(json::value_to_tag<input>, const json::value& value) noexcept
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
    const input& input) noexcept
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
    const json::value& value) noexcept
{
    return to_shared(tag_invoke(json::value_to_tag<input>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const input::cptr& input) noexcept
{
    tag_invoke(tag, value, *input);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
