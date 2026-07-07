/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/views/transaction_view.hpp>

#include <iterator>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

constexpr auto value_size = sizeof(uint64_t);
constexpr auto version_size = sizeof(uint32_t);
constexpr auto sequence_size = sizeof(uint32_t);
constexpr auto locktime_size = sizeof(uint32_t);
constexpr auto point_size = chain::point::serialized_size();
constexpr auto sentinels_size = sizeof(witness_marker) +
    sizeof(witness_enabled);

// constructor
// ----------------------------------------------------------------------------

transaction_view::transaction_view(reader& source,
    const data_chunk& block_buffer, bool coinbase, bool witness) NOEXCEPT
  : coinbase_{ coinbase }
{
    const auto tx_start = source.get_read_position();
    tx_ptr_ = std::next(block_buffer.data(), tx_start);

    // tx.version
    source.skip_bytes(version_size);

    in_count_ = source.read_size(max_count);
    const auto has_witness =
        in_count_ == witness_marker &&
        source.peek_byte() == witness_enabled;

    if (has_witness)
    {
        source.skip_byte();
        in_count_ = source.read_size(max_count);
    }

    // tx.inputs
    in_offset_ = source.get_read_position() - tx_start;
    for (size_t input{}; input < in_count_; ++input)
    {
        source.skip_bytes(point_size);
        const auto script_size = source.read_size(max_bytes);
        source.skip_bytes(script_size + sequence_size);
        input_table_size_ += variable_size(script_size) + script_size;
    }

    // tx.outputs
    out_count_ = source.read_size(max_count);
    out_offset_ = source.get_read_position() - tx_start;
    for (size_t output{}; output < out_count_; ++output)
    {
        const auto value = source.read_8_bytes_little_endian();
        const auto script_size = source.read_size(max_bytes);
        source.skip_bytes(script_size);
        output_table_size_ += variable_size(value) +
            variable_size(script_size) + script_size;
    }

    // tx.witnessses
    const auto wit_offset = source.get_read_position() - tx_start;
    if (has_witness)
    {
        auto superfluous{ true };
        for (size_t input{}; input < in_count_; ++input)
            if (witness::skip(source, true))
                superfluous = false;

        // Transaction is non-segregated if all witnesses are empty.
        // Validatable, but treat superfluous as invalid serialization.
        if (superfluous)
            source.invalidate();
    }

    // tx.locktime
    source.skip_bytes(locktime_size);

    size_ = source.get_read_position() - tx_start;
    segregated_ = has_witness && witness;
    witnesses_size_ = size_ - (wit_offset + locktime_size);
    input_table_size_ += segregated_ ? witnesses_size_ : in_count_;
    if (is_zero(in_count_) || is_zero(out_count_))
        source.invalidate();

    // invalid
    if (!source)
    {
        tx_ptr_ = nullptr;
        return;
    }

    // set hash identifiers
    if (has_witness)
    {
        txid_ = transaction::desegregated_hash(unstripped_size(),
            stripped_size(), tx_ptr_);

        // non-witness and coinbase wtxid default to null_hash.
        if (witness && !coinbase)
            wtxid_ = bitcoin_hash(size_, tx_ptr_);
    }
    else
    {
        txid_ = bitcoin_hash(size_, tx_ptr_);
    }
}

// public
// ----------------------------------------------------------------------------
// properties

bool transaction_view::is_valid() const NOEXCEPT
{
    return !is_null(tx_ptr_);
}

bool transaction_view::is_coinbase() const NOEXCEPT
{
    return coinbase_;
}

bool transaction_view::is_null_point() const NOEXCEPT
{
    BC_ASSERT(is_valid());

    const auto point = at_inputs();
    const auto index = std::next(point, hash_size);
    return unsafe_array_cast<uint8_t, hash_size>(point) == null_hash &&
        unsafe_from_little_endian<uint32_t>(index) == chain::point::null_index;
}

bool transaction_view::is_segregated() const NOEXCEPT
{
    return segregated_;
}

size_t transaction_view::inputs() const NOEXCEPT
{
    return in_count_;
}

size_t transaction_view::outputs() const NOEXCEPT
{
    return out_count_;
}

uint32_t transaction_view::version() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return unsafe_from_little_endian<uint32_t>(tx_ptr_);
}

uint32_t transaction_view::locktime() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    const auto offset = std::next(tx_ptr_, size_ - locktime_size);
    return unsafe_from_little_endian<uint32_t>(offset);
}

size_t transaction_view::serialized_size(bool witness) const NOEXCEPT
{
    return witness && is_segregated() ? size_ : stripped_size();
}

data_chunk transaction_view::to_data(bool witness) const NOEXCEPT
{
    data_chunk data(serialized_size(witness));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, witness);
    return data;
}

void transaction_view::to_data(std::ostream& stream,
    bool witness) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void transaction_view::to_data(writer& sink, bool witness) const NOEXCEPT
{
    // Witness can be stripped but never added (mirrors chain::transaction).
    if (witness && is_segregated())
    {
        sink.write_bytes(tx_ptr_, size_);
        return;
    }

    // Stripped form drops the witness marker/flag sentinels (after version)
    // and the witness data (before locktime); inputs/outputs are unchanged.
    const auto sentinels = is_zero(witnesses_size_) ? zero : sentinels_size;
    const auto body = std::next(tx_ptr_, version_size + sentinels);
    const auto body_size = size_ - witnesses_size_ - locktime_size -
        version_size - sentinels;

    sink.write_bytes(tx_ptr_, version_size);
    sink.write_bytes(body, body_size);
    sink.write_bytes(std::next(tx_ptr_, size_ - locktime_size), locktime_size);
}

const hash_digest& transaction_view::hash(bool witness) const NOEXCEPT
{
    return witness && is_segregated() ? wtxid_ : txid_;
}

// public
// ----------------------------------------------------------------------------
// store helpers

size_t transaction_view::input_table_size() const NOEXCEPT
{
    return input_table_size_;
}

size_t transaction_view::output_table_size() const NOEXCEPT
{
    return output_table_size_;
}

// public
// ----------------------------------------------------------------------------
// methods

// Last output of commitment pattern holds the committed value [bip141].
bool transaction_view::get_witness_commitment(
    hash_cref& commitment) const NOEXCEPT
{
    const auto output = at_outputs();
    stream::in::fast istream(output, outputs_size());
    read::bytes::fast reader(istream);
    bool found{};

    for (size_t out{}; out < out_count_; ++out)
    {
        reader.skip_bytes(value_size);
        const auto size = reader.read_size();
        const auto script = std::next(output, reader.get_read_position());

        if (is_commitment_pattern(script, size))
        {
            const auto offset = std::next(script, commitment_pattern_size);
            commitment = unsafe_array_cast<uint8_t, hash_size>(offset);
            found = true;
        }

        reader.skip_bytes(size);
    }

    return found;
}

// Coinbase input witness must be 32 byte witness reserved value [bip141].
bool transaction_view::get_witness_reservation(
    hash_cref& reservation) const NOEXCEPT
{
    const auto witness = at_witnesses();
    if (!is_reserved_pattern(witness, witnesses_size()))
        return false;

    const auto offset = std::next(witness, reserved_pattern_size);
    reservation = unsafe_array_cast<uint8_t, hash_size>(offset);
    return true;
}

// public/static
// ----------------------------------------------------------------------------
// streamers

void transaction_view::write_input_script(flipper& sink,
    reader& source) NOEXCEPT
{
    // skip point (stored independently)
    source.skip_bytes(point_size);

    // script size
    const auto size = source.read_size();
    sink.write_variable(size);

    // script
    sink.write_bytes(source.read_bytes(size));
}

void transaction_view::write_witness(flipper& sink, reader& source) NOEXCEPT
{
    // stack size
    const auto stack = source.read_size();
    sink.write_variable(stack);

    // stack
    for (size_t element{}; element < stack; ++element)
    {
        // element size
        const auto size = source.read_size();
        sink.write_variable(size);

        // element
        sink.write_bytes(source.read_bytes(size));
    }
}

size_t transaction_view::read_witness_size(reader& source) NOEXCEPT
{
    // stack size
    const auto stack = source.read_size();
    auto total = variable_size(stack);

    // stack
    for (size_t element{}; element < stack; ++element)
    {
        // element size
        const auto size = source.read_size();
        total += variable_size(size) + size;

        // element
        source.skip_bytes(size);
    }

    return total;
}

// public
// ----------------------------------------------------------------------------
// istreams

stream::in::fast transaction_view::get_inputs_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(inputs_size());
    return { at_inputs(), limit };
}

stream::in::fast transaction_view::get_outputs_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(outputs_size());
    return { at_outputs(), limit };
}

stream::in::fast transaction_view::get_witnesses_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(witnesses_size());
    return { at_witnesses(), limit };
}

// private/static
// ----------------------------------------------------------------------------
// witness commitment

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

bool transaction_view::is_commitment_pattern(const uint8_t* script,
    size_t size) NOEXCEPT
{
    if (commitment_pattern_size + hash_size > size)
        return false;

    constexpr auto header = to_big_endian(chain::witness_head);
    return script[0] == static_cast<uint8_t>(opcode::op_return)
        && script[1] == static_cast<uint8_t>(opcode::push_size_36)
        && script[2] == header[0]
        && script[3] == header[1]
        && script[4] == header[2]
        && script[5] == header[3];
}

bool transaction_view::is_reserved_pattern(const uint8_t* stack,
    size_t size) NOEXCEPT
{
    if (reserved_pattern_size + hash_size > size)
        return false;

    // First byte is stack size, second byte is size of single stack element.
    return stack[0] == one
        && stack[1] == hash_size;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// private
// ----------------------------------------------------------------------------
// buffer offsets

const uint8_t* transaction_view::at_inputs() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return std::next(tx_ptr_, in_offset_);
}

const uint8_t* transaction_view::at_outputs() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    return std::next(tx_ptr_, out_offset_);
}

const uint8_t* transaction_view::at_witnesses() const NOEXCEPT
{
    BC_ASSERT(is_valid());
    const auto witness_offset = size_ - (witnesses_size() + locktime_size);
    return std::next(tx_ptr_, witness_offset);
}

// private
// ----------------------------------------------------------------------------
// computed sizes

size_t transaction_view::inputs_size() const NOEXCEPT
{
    return size_ - in_offset_ - (witnesses_size_ + locktime_size);
}

size_t transaction_view::outputs_size() const NOEXCEPT
{
    return size_ - out_offset_ - (witnesses_size_ + locktime_size);
}

size_t transaction_view::witnesses_size() const NOEXCEPT
{
    return witnesses_size_;
}

size_t transaction_view::unstripped_size() const NOEXCEPT
{
    return size_;
}

size_t transaction_view::stripped_size() const NOEXCEPT
{
    return is_zero(witnesses_size_) ? size_ :
        size_ - (witnesses_size_ + sentinels_size);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
