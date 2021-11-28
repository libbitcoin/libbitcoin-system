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
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// Constructors.
//-----------------------------------------------------------------------------

// Valid default used in signature hashing.
input::input()
  : input({}, {}, {}, 0, true)
{
}

input::input(input&& other)
  : input(
      std::move(other.prevout_),
      std::move(other.script_),
      std::move(other.witness_),
      other.sequence_,
      other.valid_)
{
}

input::input(const input& other)
  : input(
      other.prevout_,
      other.script_,
      other.witness_,
      other.sequence_,
      other.valid_)
{
}

input::input(point&& prevout, chain::script&& script, uint32_t sequence)
  : input(std::move(prevout), std::move(script), {}, sequence, true)
{
}

input::input(const point& prevout, const chain::script& script,
    uint32_t sequence)
  : input(prevout, script, {}, sequence, true)
{
}

input::input(point&& prevout, chain::script&& script, chain::witness&& witness,
    uint32_t sequence)
  : input(std::move(prevout), std::move(script), std::move(witness), sequence,
      true)
{
}

input::input(const point& prevout, const chain::script& script,
    const chain::witness& witness, uint32_t sequence)
  : input(prevout, script, witness, sequence, true)
{
}

// protected
input::input(point&& prevout, chain::script&& script, chain::witness&& witness,
    uint32_t sequence, bool valid)
  : prevout_(std::move(prevout)),
    script_(std::move(script)),
    witness_(std::move(witness)),
    sequence_(sequence),
    valid_(valid)
{
}

// protected
input::input(const point& prevout, const chain::script& script,
    const chain::witness& witness, uint32_t sequence, bool valid)
  : prevout_(prevout),
    script_(script),
    witness_(witness),
    sequence_(sequence),
    valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

input& input::operator=(input&& other)
{
    prevout_ = std::move(other.prevout_);
    script_ = std::move(other.script_);
    witness_ = std::move(other.witness_);
    sequence_ = other.sequence_;
    valid_ = other.valid_;
    return *this;
}

input& input::operator=(const input& other)
{
    prevout_ = other.prevout_;
    script_ = other.script_;
    witness_ = other.witness_;
    sequence_ = other.sequence_;
    valid_ = other.valid_;
    return *this;
}

bool input::operator==(const input& other) const
{
    return (sequence_ == other.sequence_)
        && (prevout_ == other.prevout_)
        && (script_ == other.script_)
        && (witness_ == other.witness_);
}

bool input::operator!=(const input& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

input input::factory(const data_chunk& data)
{
    input instance;
    instance.from_data(data);
    return instance;
}

input input::factory(std::istream& stream)
{
    input instance;
    instance.from_data(stream);
    return instance;
}

input input::factory(reader& source)
{
    input instance;
    instance.from_data(source);
    return instance;
}

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

    prevout_.from_data(source);
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
    prevout_.reset();
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
//-----------------------------------------------------------------------------

data_chunk input::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
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
    DEBUG_ONLY(const auto size = serialized_size();)
    DEBUG_ONLY(const auto start = sink.get_position();)

    prevout_.to_data(sink);
    script_.to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence_);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

// Size.
//-----------------------------------------------------------------------------

size_t input::serialized_size(bool witness) const
{
    // input.serialized_size(witness) provides sizing for witness, however
    // witnesses are serialized by the transaction. This is an ugly hack as a
    // consequence of bip144 not serializing witnesses as part of inputs, which
    // is logically the proper association.
    return prevout_.serialized_size()
        + script_.serialized_size(true)
        + (witness ? witness_.serialized_size(true) : zero)
        + sizeof(sequence_);
}

// Accessors.
//-----------------------------------------------------------------------------

const point& input::previous_output() const
{
    return prevout_;
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

bool input::extract_reserved_hash(hash_digest& out) const
{
    const auto& stack = witness_.stack();

    if (!witness::is_reserved_pattern(stack))
        return false;

    std::copy_n(stack.front().begin(), hash_size, out.begin());
    return true;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
