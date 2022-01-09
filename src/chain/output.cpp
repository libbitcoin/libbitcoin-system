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
#include <iterator>
#include <memory>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// This is a consensus critical value that must be set on reset.
const uint64_t output::not_found = sighash_null_value;

// Constructors.
// ----------------------------------------------------------------------------

// Invalid default used in signature hashing (validity ignored).
// Invalidity is also used to determine that a prevout is not found.
output::output() noexcept
  : output(output::not_found, to_shared<chain::script>(), false)
{
}

output::output(output&& other) noexcept
  : output(other)
{
}

output::output(const output& other) noexcept
  : output(other.value_, other.script_, other.valid_)
{
}

output::output(uint64_t value, chain::script&& script) noexcept
  : output(value, to_shared(std::move(script)), true)
{
}

output::output(uint64_t value, const chain::script& script) noexcept
  : output(value, to_shared(script), true)
{
}

output::output(uint64_t value, const chain::script::ptr& script) noexcept
  : output(value, script ? script : to_shared<chain::script>(), true)
{
}

output::output(const data_slice& data) noexcept
  : output(stream::in::copy(data))
{
}

output::output(std::istream&& stream) noexcept
  : output(read::bytes::istream(stream))
{
}

output::output(std::istream& stream) noexcept
  : output(read::bytes::istream(stream))
{
}

output::output(reader&& source) noexcept
  : output(from_data(source))
{
}

output::output(reader& source) noexcept
  : output(from_data(source))
{
}

// protected
output::output(uint64_t value, const chain::script::ptr& script,
    bool valid) noexcept
  : value_(value), script_(script), valid_(valid)
{
}

// Operators.
// ----------------------------------------------------------------------------

output& output::operator=(output&& other) noexcept
{
    *this = other;
    return *this;
}

output& output::operator=(const output& other) noexcept
{
    value_ = other.value_;
    script_ = other.script_;
    valid_ = other.valid_;
    return *this;
}

bool output::operator==(const output& other) const noexcept
{
    return (value_ == other.value_)
        && (*script_ == *other.script_);
}

bool output::operator!=(const output& other) const noexcept
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// static/private
output output::from_data(reader& source) noexcept
{
    return
    {
        source.read_8_bytes_little_endian(),
        to_shared(new chain::script{ source, true }),
        source
    };
}

// Serialization.
// ----------------------------------------------------------------------------

data_chunk output::to_data() const noexcept
{
    data_chunk data(no_fill_byte_allocator);
    data.resize(serialized_size());
    stream::out::copy ostream(data);
    to_data(ostream);
    return data;
}

void output::to_data(std::ostream& stream) const noexcept
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void output::to_data(writer& sink) const noexcept
{
    sink.write_8_bytes_little_endian(value_);
    script_->to_data(sink, true);
}

size_t output::serialized_size() const noexcept
{
    return sizeof(value_) + script_->serialized_size(true);
}

// Properties.
// ----------------------------------------------------------------------------

bool output::is_valid() const noexcept
{
    return valid_;
}

uint64_t output::value() const noexcept
{
    return value_;
}

const chain::script& output::script() const noexcept
{
    return *script_;
}

const chain::script::ptr output::script_ptr() const noexcept
{
    return script_;
}

// Methods.
// ----------------------------------------------------------------------------

bool output::committed_hash(hash_digest& out) const noexcept
{
    const auto& ops = script_->ops();
    if (!script::is_commitment_pattern(ops))
        return false;

    // The four byte offset for the witness commitment hash (bip141).
    const auto start = std::next(ops[1].data().begin(), sizeof(witness_head));
    std::copy_n(start, hash_size, out.begin());
    return true;
}

// Product overflows guarded by script size limit.
static_assert(max_script_size < max_size_t / multisig_default_sigops / 
    heavy_sigops_factor, "output sigop overflow guard");

size_t output::signature_operations(bool bip141) const noexcept
{
    // Penalize quadratic signature operations (bip141).
    const auto factor = bip141 ? heavy_sigops_factor : one;

    // Count heavy sigops in the output script.
    return script_->sigops(false) * factor;
}

bool output::is_dust(uint64_t minimum_value) const noexcept
{
    // If provably unspendable it does not expand the unspent output set. Dust
    // is all about prunability. Miners can be expected take the largest fee
    // independent of dust, so this is an attempt to prevent miners from seeing
    // transactions with unprunable outputs.
    return value_ < minimum_value && !script_->is_unspendable();
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
