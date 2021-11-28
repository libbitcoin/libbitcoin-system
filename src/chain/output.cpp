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
#include <bitcoin/system/chain/output.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include  <iterator>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// This is a consensus critical value that must be set on reset.
const uint64_t output::not_found = sighash_null_value;

// Constructors.
//-----------------------------------------------------------------------------

// Valid default used in signature hashing.
output::output()
  : output(output::not_found, {}, true)
{
}

output::output(output&& other)
  : output(other.value_, std::move(other.script_), other.valid_)
{
}

output::output(const output& other)
  : output(other.value_, other.script_, other.valid_)
{
}

output::output(uint64_t value, chain::script&& script)
  : output(value, std::move(script), true)
{
}

output::output(uint64_t value, const chain::script& script)
  : output(value, script, true)
{
}

// protected
output::output(uint64_t value, chain::script&& script, bool valid)
  : value_(value), script_(std::move(script)), valid_(valid)
{
}

// protected
output::output(uint64_t value, const chain::script& script, bool valid)
  : value_(value), script_(script), valid_(valid)
{
}

// Operators.
//-----------------------------------------------------------------------------

output& output::operator=(output&& other)
{
    value_ = other.value_;
    script_ = std::move(other.script_);
    valid_ = other.valid_;
    return *this;
}

output& output::operator=(const output& other)
{
    value_ = other.value_;
    script_ = other.script_;
    valid_ = other.valid_;
    return *this;
}

bool output::operator==(const output& other) const
{
    return (value_ == other.value_)
        && (script_ == other.script_);
}

bool output::operator!=(const output& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

output output::factory(const data_chunk& data)
{
    output instance;
    instance.from_data(data);
    return instance;
}

output output::factory(std::istream& stream)
{
    output instance;
    instance.from_data(stream);
    return instance;
}

output output::factory(reader& source)
{
    output instance;
    instance.from_data(source);
    return instance;
}

bool output::from_data(const data_chunk& data)
{
    stream::in::copy istream(data);
    return from_data(istream);
}

bool output::from_data(std::istream& stream)
{
    read::bytes::istream source(stream);
    return from_data(source);
}

bool output::from_data(reader& source)
{
    reset();

    value_ = source.read_8_bytes_little_endian();
    script_.from_data(source, true);

    if (!source)
        reset();

    valid_ = source;
    return valid_;
}

// protected
void output::reset()
{
    // This value is required by signature hashing for output clearance.
    value_ = output::not_found;
    script_.reset();
    valid_ = false;
}

bool output::is_valid() const
{
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk output::to_data() const
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void output::to_data(std::ostream& stream) const
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void output::to_data(writer& sink) const
{
    DEBUG_ONLY(const auto size = serialized_size();)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_8_bytes_little_endian(value_);
    script_.to_data(sink, true);

    BITCOIN_ASSERT(sink.get_position() - start == size);
}

// Size.
//-----------------------------------------------------------------------------

size_t output::serialized_size() const
{
    return sizeof(value_) + script_.serialized_size(true);
}

// Accessors.
//-----------------------------------------------------------------------------

uint64_t output::value() const
{
    return value_;
}

const chain::script& output::script() const
{
    return script_;
}

// Validation helpers.
//-----------------------------------------------------------------------------

size_t output::signature_operations(bool bip141) const
{
    // Penalize quadratic signature operations (bip141).
    const auto sigops_factor = bip141 ? fast_sigops_factor : one;

    // Count heavy sigops in the output script.
    return script_.sigops(false) * sigops_factor;
}

bool output::is_dust(uint64_t minimum_value) const
{
    // If provably unspendable it does not expand the unspent output set.
    return value_ < minimum_value && !script_.is_unspendable();
}

bool output::extract_committed_hash(hash_digest& out) const
{
    const auto& ops = script_.operations();

    if (!script::is_commitment_pattern(ops))
        return false;

    // The four byte offset for the witness commitment hash (bip141).
    const auto start = std::next(ops[1].data().begin(), sizeof(witness_head));
    std::copy_n(start, hash_size, out.begin());
    return true;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
