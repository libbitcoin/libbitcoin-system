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

input::input()
  : previous_output_{},
    script_{},
    sequence_(0)
{
}

input::input(input&& other)
  : previous_output_(std::move(other.previous_output_)),
    script_(std::move(other.script_)),
    witness_(std::move(other.witness_)),
    sequence_(other.sequence_)
{
}

input::input(const input& other)
  : previous_output_(other.previous_output_),
    script_(std::move(other.script_)),
    witness_(other.witness_),
    sequence_(other.sequence_)
{
}

input::input(point&& previous_output, chain::script&& script,
    uint32_t sequence)
  : previous_output_(std::move(previous_output)),
    script_(std::move(script)),
    sequence_(sequence)
{
}

input::input(const point& previous_output, const chain::script& script,
    uint32_t sequence)
  : previous_output_(previous_output),
    script_(script),
    sequence_(sequence)
{
}

input::input(point&& previous_output, chain::script&& script,
    chain::witness&& witness, uint32_t sequence)
  : previous_output_(std::move(previous_output)),
    script_(std::move(script)),
    witness_(std::move(witness)),
    sequence_(sequence)
{
}

input::input(const point& previous_output, const chain::script& script,
    const chain::witness& witness, uint32_t sequence)
  : previous_output_(previous_output),
    script_(script),
    witness_(witness),
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

input input::factory(const data_chunk& data, bool witness)
{
    input instance;
    instance.from_data(data, witness);
    return instance;
}

input input::factory(std::istream& stream, bool witness)
{
    input instance;
    instance.from_data(stream, witness);
    return instance;
}

input input::factory(reader& source, bool witness)
{
    input instance;
    instance.from_data(source, witness);
    return instance;
}

bool input::from_data(const data_chunk& data, bool witness)
{
    read::bytes::copy reader(data);
    return from_data(reader, witness);
}

bool input::from_data(std::istream& stream, bool witness)
{
    read::bytes::istream reader(stream);
    return from_data(reader, witness);
}

bool input::from_data(reader& source, bool witness)
{
    reset();

    previous_output_.from_data(source);
    script_.from_data(source, true);
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
    return sequence_ != 0 ||
        previous_output_.is_valid() ||
        script_.is_valid() ||
        witness_.is_valid();
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk input::to_data(bool witness) const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size(witness));
    stream::out::copy ostream(data);
    to_data(ostream, witness);
    return data;
}

void input::to_data(std::ostream& stream, bool witness) const
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void input::to_data(writer& sink, bool witness) const
{
    DEBUG_ONLY(const auto size = serialized_size(witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    previous_output_.to_data(sink);
    script_.to_data(sink, true);
    sink.write_4_bytes_little_endian(sequence_);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

// Size.
//-----------------------------------------------------------------------------

size_t input::serialized_size(bool witness) const
{

    return previous_output_.serialized_size()
        + script_.serialized_size(true)
        + (witness ? witness_.serialized_size(true) : zero)
        + sizeof(sequence_);
}

// Accessors.
//-----------------------------------------------------------------------------

const point& input::previous_output() const
{
    return previous_output_;
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
