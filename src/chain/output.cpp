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
#include <bitcoin/system/chain/output.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// Product overflows guarded by script size limit.
static_assert(max_script_size <
    max_size_t / multisig_default_sigops / heavy_sigops_factor,
    "output sigop overflow guard");

// This is a consensus critical value that must be set on reset.
const uint64_t output::not_found = sighash_null_value;

// Constructors.
// ----------------------------------------------------------------------------

// Invalid default used in signature hashing (validity ignored).
// Invalidity is also used to determine that a prevout is not found.
output::output() NOEXCEPT
  : output(output::not_found, to_shared<chain::script>(), false)
{
}

output::output(uint64_t value, chain::script&& script) NOEXCEPT
  : output(value, to_shared(std::move(script)), true)
{
}

output::output(uint64_t value, const chain::script& script) NOEXCEPT
  : output(value, to_shared(script), true)
{
}

output::output(uint64_t value, const chain::script::cptr& script) NOEXCEPT
  : output(value, script ? script : to_shared<chain::script>(), true)
{
}

output::output(stream::in::fast&& stream) NOEXCEPT
  : output(read::bytes::fast(stream))
{
}

output::output(stream::in::fast& stream) NOEXCEPT
  : output(read::bytes::fast(stream))
{
}

output::output(std::istream&& stream) NOEXCEPT
  : output(read::bytes::istream(stream))
{
}

output::output(std::istream& stream) NOEXCEPT
  : output(read::bytes::istream(stream))
{
}

output::output(reader&& source) NOEXCEPT
  : output(source)
{
}

output::output(reader& source) NOEXCEPT
  : value_(source.read_8_bytes_little_endian()),
    script_(CREATE(chain::script, source.get_allocator(), source, true)),
    valid_(source),
    size_(serialized_size(*script_, value_))
{
}

// protected
output::output(uint64_t value, const chain::script::cptr& script,
    bool valid) NOEXCEPT
  : value_(value),
    script_(script),
    valid_(valid),
    size_(serialized_size(*script, value))
{
}

// Operators.
// ----------------------------------------------------------------------------

bool output::operator==(const output& other) const NOEXCEPT
{
    return (value_ == other.value_)
        && (script_ == other.script_ || *script_ == *other.script_);
}

bool output::operator!=(const output& other) const NOEXCEPT
{
    return !(*this == other);
}

// Deserialization.
// ----------------------------------------------------------------------------

// Serialization.
// ----------------------------------------------------------------------------

data_chunk output::to_data() const NOEXCEPT
{
    data_chunk data(serialized_size());
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out);
    return data;
}

void output::to_data(std::ostream& stream) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out);
}

void output::to_data(writer& sink) const NOEXCEPT
{
    sink.write_8_bytes_little_endian(value_);
    script_->to_data(sink, true);
}

// static/private
size_t output::serialized_size(const chain::script& script,
    uint64_t value) NOEXCEPT
{
    return ceilinged_add(sizeof(value), script.serialized_size(true));
}

size_t output::serialized_size() const NOEXCEPT
{
    return size_;
}

// Properties.
// ----------------------------------------------------------------------------

bool output::is_valid() const NOEXCEPT
{
    return valid_;
}

uint64_t output::value() const NOEXCEPT
{
    return value_;
}

const chain::script& output::script() const NOEXCEPT
{
    return *script_;
}

const chain::script::cptr& output::script_ptr() const NOEXCEPT
{
    return script_;
}

// Methods.
// ----------------------------------------------------------------------------

hash_digest output::hash() const NOEXCEPT
{
    hash_digest out{};
    stream::out::fast stream{ out };
    hash::sha256::fast sink{ stream };
    to_data(sink);
    sink.flush();
    return out;
}

// NOT THREAD SAFE
// Proves benefit only with multiple tapscript hash_single per input script.
const hash_digest& output::get_hash() const NOEXCEPT
{
    if (!cache_)
        cache_ = std::make_shared<const hash_digest>(hash());

    return *cache_;
}

bool output::committed_hash(hash_cref& out) const NOEXCEPT
{
    const auto& ops = script_->ops();
    if (!script::is_commitment_pattern(ops))
        return false;

    // Offset four bytes for witness commitment head [bip141].
    const auto start = std::next(ops[1].data().data(), sizeof(witness_head));

    // Guarded by is_commitment_pattern.
    out = unsafe_array_cast<uint8_t, hash_size>(start);
    return true;
}

size_t output::signature_operations(bool bip141) const NOEXCEPT
{
    // Sigops in the current output script, input script, and P2SH embedded
    // script are counted at four times their previous value (heavy) [bip141].
    const auto factor = bip141 ? heavy_sigops_factor : one;

    // Count heavy sigops in the output script (inaccurate).
    return script_->signature_operations(false) * factor;
}

bool output::is_dust(uint64_t minimum_value) const NOEXCEPT
{
    // If provably unspendable it does not expand the unspent output set. Dust
    // is all about prunability. Miners can be expected take the largest fee
    // independent of dust, so this is an attempt to prevent miners from seeing
    // transactions with unprunable outputs.
    return value_ < minimum_value && !script_->is_unspendable();
}

BC_POP_WARNING()
BC_POP_WARNING()

// JSON value convertors.
// ----------------------------------------------------------------------------

namespace json = boost::json;

// boost/json will soon have NOEXCEPT: github.com/boostorg/json/pull/636
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

output tag_invoke(json::value_to_tag<output>,
    const json::value& value) NOEXCEPT
{
    return
    {
        value.at("value").to_number<uint64_t>(),
        json::value_to<chain::script>(value.at("script"))
    };
}

void tag_invoke(json::value_from_tag, json::value& value,
    const output& output) NOEXCEPT
{
    value =
    {
        { "value", output.value() },
        { "script", json::value_from(output.script()) },
    };
}

BC_POP_WARNING()

output::cptr tag_invoke(json::value_to_tag<output::cptr>,
    const json::value& value) NOEXCEPT
{
    return to_shared(tag_invoke(json::value_to_tag<output>{}, value));
}

// Shared pointer overload is required for navigation.
BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

void tag_invoke(json::value_from_tag tag, json::value& value,
    const output::cptr& output) NOEXCEPT
{
    tag_invoke(tag, value, *output);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
