/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/input.hpp>

#include <algorithm>
#include <sstream>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/chain/witness.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::wallet;
using namespace bc::machine;

// Constructors.
//-----------------------------------------------------------------------------

input::input()
  : previous_output_{}, sequence_(0)
{
}

input::input(input&& other)
  : input(std::move(other.previous_output_), std::move(other.script_),
        std::move(other.witness_), other.sequence_)
{
}

input::input(const input& other)
  : input(other.previous_output_, other.script_, other.witness_,
        other.sequence_)
{
}

input::input(output_point&& previous_output, chain::script&& script,
    uint32_t sequence)
  : previous_output_(std::move(previous_output)), script_(std::move(script)),
    sequence_(sequence)
{
}

input::input(const output_point& previous_output, const chain::script& script,
    uint32_t sequence)
  : previous_output_(previous_output), script_(script), sequence_(sequence)
{
}

input::input(output_point&& previous_output, chain::script&& script,
    chain::witness&& witness, uint32_t sequence)
  : previous_output_(std::move(previous_output)), script_(std::move(script)),
    witness_(std::move(witness)), sequence_(sequence)
{
}

input::input(const output_point& previous_output, const chain::script& script,
    const chain::witness& witness, uint32_t sequence)
  : previous_output_(previous_output), script_(script), witness_(witness),
    sequence_(sequence)
{
}

// Operators.
//-----------------------------------------------------------------------------

input& input::operator=(input&& other)
{
    previous_output_ = std::move(other.previous_output_);
    script_ = std::move(other.script_);
    witness_ = std::move(other.witness_);
    sequence_ = other.sequence_;
    return *this;
}

input& input::operator=(const input& other)
{
    previous_output_ = other.previous_output_;
    script_ = other.script_;
    witness_ = other.witness_;
    sequence_ = other.sequence_;
    return *this;
}

bool input::operator==(const input& other) const
{
    return (sequence_ == other.sequence_)
        && (previous_output_ == other.previous_output_)
        && (script_ == other.script_)
        && (witness_ == other.witness_);
}

bool input::operator!=(const input& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

input input::factory_from_data(const data_chunk& data, bool wire, bool witness)
{
    input instance;
    instance.from_data(data, wire, witness);
    return instance;
}

input input::factory_from_data(std::istream& stream, bool wire, bool witness)
{
    input instance;
    instance.from_data(stream, wire, witness);
    return instance;
}

input input::factory_from_data(reader& source, bool wire, bool witness)
{
    input instance;
    instance.from_data(source, wire, witness);
    return instance;
}

bool input::from_data(const data_chunk& data, bool wire, bool witness)
{
    data_source istream(data);
    return from_data(istream, wire, witness);
}

bool input::from_data(std::istream& stream, bool wire, bool witness)
{
    istream_reader source(stream);
    return from_data(source, wire, witness);
}

bool input::from_data(reader& source, bool wire, bool witness)
{
    reset();

    if (!previous_output_.from_data(source, wire))
        return false;

    script_.from_data(source, true);

    // Transaction from_data handles the discontiguous wire witness decoding.
    if (witness && !wire)
        witness_.from_data(source, true);

    sequence_ = source.read_4_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

void input::reset()
{
    previous_output_.reset();
    script_.reset();
    witness_.reset();
    sequence_ = 0;
}

// Since empty scripts and zero sequence are valid this relies on the prevout.
bool input::is_valid() const
{
    return sequence_ != 0 || previous_output_.is_valid() ||
        script_.is_valid() || witness_.is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk input::to_data(bool wire, bool witness) const
{
    data_chunk data;
    const auto size = serialized_size(wire, witness);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, wire, witness);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void input::to_data(std::ostream& stream, bool wire, bool witness) const
{
    ostream_writer sink(stream);
    to_data(sink, wire, witness);
}

void input::to_data(writer& sink, bool wire, bool witness) const
{
    previous_output_.to_data(sink, wire);
    script_.to_data(sink, true);

    // Transaction to_data handles the discontiguous wire witness encoding.
    if (witness && !wire)
        witness_.to_data(sink, true);

    sink.write_4_bytes_little_endian(sequence_);
}

// Size.
//-----------------------------------------------------------------------------

size_t input::serialized_size(bool wire, bool witness) const
{
    // Witness size added in both contexts despite that tx writes wire witness.
    // Prefix is written for both wire and store/other contexts.
    return previous_output_.serialized_size(wire)
        + script_.serialized_size(true)
        + (witness ? witness_.serialized_size(true) : 0)
        + sizeof(sequence_);
}

// Accessors.
//-----------------------------------------------------------------------------

output_point& input::previous_output()
{
    return previous_output_;
}

const output_point& input::previous_output() const
{
    return previous_output_;
}

void input::set_previous_output(const output_point& value)
{
    previous_output_ = value;
}

void input::set_previous_output(output_point&& value)
{
    previous_output_ = std::move(value);
}

chain::script& input::script()
{
    return script_;
}

const chain::script& input::script() const
{
    return script_;
}

void input::set_script(const chain::script& value)
{
    script_ = value;
    invalidate_cache();
}

void input::set_script(chain::script&& value)
{
    script_ = std::move(value);
    invalidate_cache();
}

chain::witness& input::witness()
{
    return witness_;
}

const chain::witness& input::witness() const
{
    return witness_;
}

void input::set_witness(const chain::witness& value)
{
    witness_ = value;
    invalidate_cache();
}

void input::set_witness(chain::witness&& value)
{
    witness_ = std::move(value);
    invalidate_cache();
}

uint32_t input::sequence() const
{
    return sequence_;
}

void input::set_sequence(uint32_t value)
{
    sequence_ = value;
}

// protected
void input::invalidate_cache() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (address_)
    {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        address_.reset();
        //---------------------------------------------------------------------
        mutex_.unlock_and_lock_upgrade();
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

payment_address input::address() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    mutex_.lock_upgrade();

    if (!address_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();

        // TODO: expand to include segregated witness address extraction.
        address_ = std::make_shared<payment_address>(
            payment_address::extract_input(script_));

        mutex_.unlock_and_lock_upgrade();
        //---------------------------------------------------------------------
    }

    const auto address = *address_;
    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    return address;
}

// Utilities.
//-----------------------------------------------------------------------------

void input::strip_witness()
{
    witness_.clear();
}

// Validation helpers.
//-----------------------------------------------------------------------------

bool input::is_final() const
{
    return sequence_ == max_input_sequence;
}

bool input::is_segregated() const
{
    // If no block tx is has witness data the commitment is optional (bip141).
    return !witness_.empty();
}

bool input::is_locked(size_t block_height, uint32_t median_time_past) const
{
    if ((sequence_ & relative_locktime_disabled) != 0)
        return false;

    // bip68: a minimum block-height constraint over the input's age.
    const auto minimum = (sequence_ & relative_locktime_mask);
    const auto& prevout = previous_output_.validation;

    if ((sequence_ & relative_locktime_time_locked) != 0)
    {
        // Median time past must be monotonically-increasing by block.
        BITCOIN_ASSERT(median_time_past >= prevout.median_time_past);
        const auto age_seconds = median_time_past - prevout.median_time_past;
        return age_seconds < (minimum << relative_locktime_seconds_shift);
    }

    BITCOIN_ASSERT(block_height >= prevout.height);
    const auto age_blocks = block_height - prevout.height;
    return age_blocks < minimum;
}

// This requires that previous outputs have been populated.
// This cannot overflow because each total is limited by max ops.
size_t input::signature_operations(bool bip16, bool bip141) const
{
    chain::script witness, embedded;
    const auto& prevout = previous_output_.validation.cache.script();
    ////BITCOIN_ASSERT_MSG(!bip141 || bip16, "bip141 implies bip16");

    // Penalize quadratic signature operations (bip141).
    const auto sigops_factor = bip141 ? fast_sigops_factor : 1u;

    // Count heavy sigops in the input script.
    auto sigops = script_.sigops(false) * sigops_factor;

    if (bip141 && extract_witness_script(witness, prevout))
    {
        // Add sigops in the witness (bip141).
        return sigops + witness.sigops(true);
    }

    if (bip16 && extract_embedded_script(embedded))
    {
        if (bip141 && extract_witness_script(witness, embedded))
        {
            // Add sigops in the embedded witness (bip141).
            return sigops + witness.sigops(true);
        }
        else
        {
            // Add heavy sigops in the embedded script (bip16).
            return sigops + embedded.sigops(true) * sigops_factor;
        }
    }

    return sigops;
}

// Extract bare (P2WPKH) or embedded (P2WSH) witness script, based on program.
bool input::extract_witness_script(chain::script& out,
    const chain::script& prevout) const
{
    switch (prevout.version())
    {
        case script_version::zero:
        {
            const auto& witness_ops = witness_.operations();
            const auto program_size = prevout.witness_program().size();

            if (program_size == hash_size)
            {
                // TODO: It would be nice if witness was derived from script.
                out.from_operations(witness_ops);
                return true;
            }

            if (program_size == short_hash_size && !witness_ops.empty())
            {
                // This cannot fail because there is no prefix.
                return out.from_data(witness_ops.back().data(), false);
            }

            return false;
        }

        case script_version::reserved:
        case script_version::unversioned:
        default:
            return false;
    }
}

// This requires that previous outputs have been populated.
bool input::extract_embedded_script(chain::script& out) const
{
    const auto& ops = script_.operations();
    const auto& prevout_script = previous_output_.validation.cache.script();

    // TODO: prevent unit test case breaks here.
    ////BITCOIN_ASSERT(previous_output_.is_valid());

    // There are no embedded sigops when the prevout script is not p2sh.
    if (!prevout_script.is_pay_to_script_hash(rule_fork::bip16_rule))
        return false;

    // There are no embedded sigops when the input script is not push only.
    // The first operations access must be method-based to guarantee the cache.
    if (ops.empty() || !script::is_relaxed_push(ops))
        return false;

    // Parse the embedded script from the last input script item (data).
    // This cannot fail because there is no prefix to invalidate the length.
    return out.from_data(ops.back().data(), false);
}

bool input::extract_reserved_hash(hash_digest& out) const
{
    const auto& ops = witness_.operations();

    if (!witness::is_reserved_pattern(ops))
        return false;

    std::copy_n(ops.front().data().begin(), hash_size, out.begin());
    return true;
}

} // namespace chain
} // namespace libbitcoin
