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

// constructor
// ----------------------------------------------------------------------------

transaction_view::transaction_view(reader& tx_source,
    const data_chunk& block_buffer, bool witness) NOEXCEPT
{
    const auto tx_start = tx_source.get_read_position();
    tx_ptr_ = std::next(block_buffer.data(), tx_start);
    tx_source.skip_bytes(version_size);

    in_count_ = tx_source.read_size(max_count);
    const auto segregated =
        in_count_ == witness_marker &&
        tx_source.peek_byte() == witness_enabled;

    if (segregated)
    {
        tx_source.skip_byte();
        in_count_ = tx_source.read_size(max_count);
    }

    in_offset_ = tx_source.get_read_position() - tx_start;
    for (size_t input{}; input < in_count_; ++input)
    {
        tx_source.skip_bytes(point_size);
        tx_source.skip_bytes(tx_source.read_size(max_bytes));
        tx_source.skip_bytes(sequence_size);
    }

    out_count_ = tx_source.read_size(max_count);
    out_offset_ = tx_source.get_read_position() - tx_start;
    for (size_t output{}; output < out_count_; ++output)
    {
        tx_source.skip_bytes(value_size);
        tx_source.skip_bytes(tx_source.read_size(max_bytes));
    }

    const auto witness_offset = tx_source.get_read_position() - tx_start;

    if (segregated)
    {
        if (witness)
        {
            for (size_t input{}; input < in_count_; ++input)
            {
                // Witness stack size cannot use the count limiter.
                const auto stack = tx_source.read_size(max_bytes);
                if (is_zero(stack))
                    tx_source.invalidate();

                for (size_t element{}; element < stack; ++element)
                    tx_source.skip_bytes(tx_source.read_size(max_bytes));
            }
        }
        else
        {
            witness::skip(tx_source, true);
        }
    }

    tx_source.skip_bytes(locktime_size);
    size_ = tx_source.get_read_position() - tx_start;
    witness_size_ = size_ - witness_offset - locktime_size;

    if (segregated)
    {
        txid_ = transaction::desegregated_hash(serialized_size(true),
            serialized_size(false), tx_ptr_);

        if (witness)
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

bool transaction_view::is_empty() const NOEXCEPT
{
    return !is_zero(inputs()) && !is_zero(outputs());
}

bool transaction_view::is_coinbase() const NOEXCEPT
{
    BC_ASSERT(!is_empty());

    // Input is guaranteed to be at least point-sized by construction.
    const auto point = at_inputs();
    const auto index = std::next(point, hash_size);
    return unsafe_array_cast<uint8_t, hash_size>(point) == null_hash &&
        unsafe_from_little_endian<uint32_t>(index) == chain::point::null_index;
}

bool transaction_view::is_segregated() const NOEXCEPT
{
    // Parsed buffer contained witnesses, and witness was specified by caller.
    return wtxid_ != null_hash;
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
    BC_ASSERT(!is_empty());
    return unsafe_from_little_endian<uint32_t>(tx_ptr_);
}

uint32_t transaction_view::locktime() const NOEXCEPT
{
    BC_ASSERT(!is_empty());
    const auto offset = std::next(tx_ptr_, size_ - locktime_size);
    return unsafe_from_little_endian<uint32_t>(offset);
}

size_t transaction_view::serialized_size(bool witness) const NOEXCEPT
{
    constexpr auto sentinels = sizeof(witness_marker) + sizeof(witness_enabled);
    return witness ? size_ : (size_ - witness_size_ - sentinels);
}

const hash_digest& transaction_view::hash(bool witness) const NOEXCEPT
{
    return witness ? wtxid_ : txid_;
}

// public
// ----------------------------------------------------------------------------
// methods

bool transaction_view::get_witness_commitment(
    hash_cref& commitment) const NOEXCEPT
{
    BC_ASSERT(!is_empty());

    const auto output = at_outputs();
    stream::in::fast istream(output, size_ - out_offset_);
    read::bytes::fast reader(istream);

    for (size_t out{}; out < sub1(out_count_); ++out)
    {
        reader.skip_variable();
        reader.skip_bytes(reader.read_size(max_bytes));
    }

    reader.skip_variable();
    const auto size = reader.read_size(max_bytes);
    const auto script = std::next(output, reader.get_read_position());
    if (!is_commitment_pattern(script, size))
        return false;

    const auto offset = std::next(script, commitment_pattern_size);
    commitment = unsafe_array_cast<uint8_t, hash_size>(offset);
    return true;
}

bool transaction_view::get_witness_reservation(
    hash_cref& reservation) const NOEXCEPT
{
    BC_ASSERT(!is_empty());

    const auto witness = at_witnesses();
    if (!is_reserved_pattern(witness, witness_size_))
        return false;

    const auto offset = std::next(witness, reserved_pattern_size);
    reservation = unsafe_array_cast<uint8_t, hash_size>(offset);
    return true;
}

// public
// ----------------------------------------------------------------------------
// streamers/istreams

// static
// Store writer doesn't write point with input.
void transaction_view::write_input_script(flipper& sink,
    reader& source) NOEXCEPT
{
    // skip point
    source.skip_bytes(point_size);

    // script size
    const auto size = source.read_size(max_bytes);
    sink.write_variable(size);

    // script
    sink.write_bytes(source.read_bytes(size));
}

// static
void transaction_view::write_input(flipper& sink, reader& source) NOEXCEPT
{
    // point
    sink.write_bytes(source.read_bytes(point_size));

    // script size
    const auto size = source.read_size(max_bytes);
    sink.write_variable(size);

    // script
    sink.write_bytes(source.read_bytes(size));
}

// static
// Store writes value as variable size (caution).
void transaction_view::write_output(flipper& sink, reader& source) NOEXCEPT
{
    // value
    sink.write_bytes(source.read_bytes(value_size));

    // script size
    const auto size = source.read_size(max_bytes);
    sink.write_variable(size);

    // script
    sink.write_bytes(source.read_bytes(size));
}

// static
void transaction_view::write_witness(flipper& sink, reader& source) NOEXCEPT
{
    // stack size
    const auto stack = source.read_size(max_bytes);
    sink.write_variable(stack);

    // stack
    for (size_t element{}; element < stack; ++element)
    {
        // element size
        const auto size = source.read_size(max_bytes);
        sink.write_variable(size);

        // element
        sink.write_bytes(source.read_bytes(size));
    }
}

stream::in::fast transaction_view::get_inputs_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(size_ - in_offset_);
    return { at_inputs(), limit };
}

stream::in::fast transaction_view::get_outputs_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(size_ - out_offset_);
    return { at_outputs(), limit };
}

stream::in::fast transaction_view::get_witnesses_stream() const NOEXCEPT
{
    const auto limit = possible_narrow_sign_cast<ptrdiff_t>(witness_size_);
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
    BC_ASSERT(!is_empty());
    return std::next(tx_ptr_, in_offset_);
}

const uint8_t* transaction_view::at_outputs() const NOEXCEPT
{
    BC_ASSERT(!is_empty());
    return std::next(tx_ptr_, out_offset_);
}

const uint8_t* transaction_view::at_witnesses() const NOEXCEPT
{
    BC_ASSERT(!is_empty());
    constexpr auto locktime_size = sizeof(uint32_t);
    return std::next(tx_ptr_, size_ - witness_size_ - locktime_size);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
