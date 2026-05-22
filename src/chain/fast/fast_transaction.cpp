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
#include <bitcoin/system/chain/fast/fast_transaction.hpp>

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

// constructor
// ----------------------------------------------------------------------------

fast_transaction::fast_transaction(reader& source, const data_chunk& buffer,
    bool witness) NOEXCEPT
{
    constexpr auto version_size = sizeof(uint32_t);
    constexpr auto sequence_size = sizeof(uint32_t);
    constexpr auto locktime_size = sizeof(uint32_t);
    const auto tx_start = source.get_read_position();
    source.skip_bytes(version_size);

    in_count_ = source.read_size(max_count);
    const auto segregated =
        in_count_ == witness_marker &&
        source.peek_byte() == witness_enabled;

    if (segregated)
    {
        source.skip_byte();
        in_count_ = source.read_size(max_count);
    }

    in_offset_ = source.get_read_position() - tx_start;
    for (size_t input{}; input < in_count_; ++input)
    {
        source.skip_bytes(chain::point::serialized_size());
        source.skip_bytes(source.read_size(max_bytes));
        source.skip_bytes(sequence_size);
    }

    out_count_ = source.read_size(max_count);
    out_offset_ = source.get_read_position() - tx_start;
    for (size_t output{}; output < out_count_; ++output)
    {
        source.skip_variable();
        source.skip_bytes(source.read_size(max_bytes));
    }

    const auto witness_offset = source.get_read_position() - tx_start;

    if (segregated)
    {
        if (witness)
        {
            for (size_t input{}; input < in_count_; ++input)
            {
                // Witness stack size cannot use the count limiter.
                const auto stack = source.read_size(max_bytes);
                if (is_zero(stack))
                    source.invalidate();

                for (size_t element{}; element < stack; ++element)
                    source.skip_bytes(source.read_size(max_bytes));
            }
        }
        else
        {
            witness::skip(source, true);
        }
    }

    source.skip_bytes(locktime_size);
    start_ptr_ = std::next(buffer.data(), tx_start);
    size_ = source.get_read_position() - tx_start;
    witness_size_ = size_ - witness_offset - locktime_size;

    if (segregated)
    {
        txid_ = transaction::desegregated_hash(serialized_size(true),
            serialized_size(false), start_ptr_);

        if (witness)
            wtxid_ = bitcoin_hash(size_, start_ptr_);
    }
    else
    {
        txid_ = bitcoin_hash(size_, start_ptr_);
    }
}

// public
// ----------------------------------------------------------------------------

bool fast_transaction::is_coinbase() const NOEXCEPT
{
    if (is_zero(in_count_))
        return false;

    // Input is guaranteed to be at least point-sized by construction.
    const auto point = to_inputs();
    const auto index = std::next(point, hash_size);

    return unsafe_array_cast<uint8_t, hash_size>(point) == null_hash &&
        unsafe_from_little_endian<uint32_t>(index) == chain::point::null_index;
}

bool fast_transaction::is_witnessed() const NOEXCEPT
{
    return to_bool(witness_size_);
}

const hash_digest& fast_transaction::id() const NOEXCEPT
{
    return txid_;
}

const hash_digest& fast_transaction::witness_id() const NOEXCEPT
{
    return wtxid_;
}

size_t fast_transaction::serialized_size(bool witness) const NOEXCEPT
{
    constexpr auto sentinels = sizeof(witness_marker) + sizeof(witness_enabled);
    return witness ? size_ : (size_ - witness_size_ - sentinels);
}

bool fast_transaction::get_witness_commitment(
    hash_cref& commitment) const NOEXCEPT
{
    if (is_zero(out_count_))
        return false;

    const auto output = to_outputs();
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

bool fast_transaction::get_witness_reservation(
    hash_cref& reservation) const NOEXCEPT
{
    const auto witness = to_witnesses();
    if (!is_reserved_pattern(witness, witness_size_))
        return false;

    const auto offset = std::next(witness, reserved_pattern_size);
    reservation = unsafe_array_cast<uint8_t, hash_size>(offset);
    return true;
}

// private/static
// ----------------------------------------------------------------------------
// witness commitment

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

bool fast_transaction::is_commitment_pattern(const uint8_t* script,
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

bool fast_transaction::is_reserved_pattern(const uint8_t* stack,
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

const uint8_t* fast_transaction::to_inputs() const NOEXCEPT
{
    return std::next(start_ptr_, in_offset_);
}

const uint8_t* fast_transaction::to_outputs() const NOEXCEPT
{
    return std::next(start_ptr_, out_offset_);
}

const uint8_t* fast_transaction::to_witnesses() const NOEXCEPT
{
    constexpr auto locktime_size = sizeof(uint32_t);
    return std::next(start_ptr_, size_ - witness_size_ - locktime_size);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
