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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(temporary_tests)

/// SHA hash accumulator.
template <typename SHA>
struct accumulator
{
    /// Creates initial context.
    constexpr accumulator() NOEXCEPT;

    /// Defaults.
    accumulator(accumulator&&) = default;
    accumulator(const accumulator&) = default;
    accumulator& operator=(accumulator&&) = default;
    accumulator& operator=(const accumulator&) = default;
    ~accumulator() = default;

    /// Write data to the context.
    inline bool write(size_t size, const typename SHA::byte_t* in) NOEXCEPT;

    /// Flush context to out32 (must be at least 32 bytes), resets context.
    inline void flush(typename SHA::byte_t* out32) NOEXCEPT;

private:
    using counter = data_array<SHA::count_bytes>;

    // Clear buffer after transforming state.
    constexpr void clear() NOEXCEPT;

    // Reset to initial context after flushing.
    constexpr void reset() NOEXCEPT;

    // Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    // Transform if buffer is full.
    constexpr bool is_full() NOEXCEPT;

    // SHA256 is limited to max_uint64 - to_bits(count_bytes) hashed bits.
    constexpr bool is_buffer_overflow(size_t bytes) NOEXCEPT;

    // Reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    // Append up to block_size bytes to buffer (fills gap if possible).
    constexpr size_t add_data(size_t bytes,
        const typename SHA::byte_t* data) NOEXCEPT;

    // Increment the counter for unbuffered transforms.
    constexpr void increment(size_t blocks) NOEXCEPT;

    // Serialize the hashed bit count for finalization
    inline counter serialize_counter() const NOEXCEPT;

    // Normalize state as the final hash value.
    inline void serialize_state(typename SHA::digest_t& out) const NOEXCEPT;

    size_t size_{};
    typename SHA::count_t bits_{};
    typename SHA::block_t buffer_;
    typename SHA::state_t state_{ SHA::H::get };
};

// implementation
// ----------------------------------------------------------------------------

// bits_ is either uint64_t or uint128_t
// So all expressions must support uintx_t (integer).
// TODO: use uint128_t as alias for intrinsic muint128_t when available.

// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)
template <typename SHA>
constexpr accumulator<SHA>::
accumulator() NOEXCEPT
BC_POP_WARNING()
{
}

template <typename SHA>
constexpr void accumulator<SHA>::
clear() NOEXCEPT
{
    size_ = zero;
}

template <typename SHA>
constexpr void accumulator<SHA>::
reset() NOEXCEPT
{
    size_ = zero;
    bits_ = zero;
    state_ = SHA::H::get;
}

template <typename SHA>
constexpr size_t accumulator<SHA>::
gap() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_subtract_overflow(SHA::block_bytes, size_), "size");
    return SHA::block_bytes - size_;
}

template <typename SHA>
constexpr bool accumulator<SHA>::
is_full() NOEXCEPT
{
    return is_zero(gap());
}

template <typename SHA>
constexpr bool accumulator<SHA>::
is_buffer_overflow(size_t bytes) NOEXCEPT
{
    BC_ASSERT_MSG(is_zero(bits_ % byte_bits), "unexpected bit count");

    // Accomodate type distinction between uint64_t and uint128_t.
    if constexpr (is_integral_integer<SHA::count_t>)
    {
        const auto used = to_floored_bytes(bits_);
        return is_add_overflow<uint64_t>(used, bytes) ||
            is_add_overflow<uint64_t>(SHA::count_bytes, add(used, bytes));
    }
    else
    {
        // TODO: add uintx_t overloads for add/subtract/overflow.
        return false;
    }
}

template <typename SHA>
constexpr size_t accumulator<SHA>::
pad_size() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_limited(size_, SHA::block_bytes), "unexpected size");

    constexpr auto singled = SHA::block_bytes - SHA::count_bytes;
    constexpr auto doubled = SHA::block_bytes + singled;
    return size_ < singled ? singled - size_ : doubled - size_;
}

// Copy and array index are guarded.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
template <typename SHA>
constexpr size_t accumulator<SHA>::
add_data(size_t bytes, const typename SHA::byte_t* data) NOEXCEPT
{
    // This is public because so extremely unlikely, but also guarded below.
    BC_ASSERT_MSG(!is_buffer_overflow(bytes), "hash function overflow");

    // No bytes accepted on overflow or uncleared buffer.
    if (is_buffer_overflow(bytes) || is_zero(gap()))
        return zero;

    const auto accepted = std::min(gap(), bytes);
    std::copy_n(data, accepted, &buffer_[size_]);
    size_ += accepted;
    bits_ += to_bits(accepted);
    return accepted;
}
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

template <typename SHA>
constexpr void accumulator<SHA>::
increment(size_t blocks) NOEXCEPT
{
    BC_ASSERT_MSG(!is_buffer_overflow(blocks * SHA::block_bytes), "size");
    bits_ += to_bits(blocks * SHA::block_bytes);
}

template <typename SHA>
inline typename accumulator<SHA>::counter accumulator<SHA>::
serialize_counter() const NOEXCEPT
{
    // Bit count is encoded into message block as 64/128 bit big-endian.
    // This is conventional and not subject to platform endianness.

    // Accomodate type distinction between uint64_t and uint128_t.
    if constexpr (is_integral_integer<SHA::count_t>)
    {
        return to_big_endian<SHA::count_t>(bits_);
    }
    else
    {
        // TODO: add to_*_endian<integral>(uintx_t) overloads.
        return to_big_endian_size<SHA::count_bytes>(bits_);
    }
}

template <typename SHA>
inline void accumulator<SHA>::
serialize_state(typename SHA::digest_t& out) const NOEXCEPT
{
    out = SHA::finalize(state_);
}

// public
template <typename SHA>
inline bool accumulator<SHA>::
write(size_t size, const typename SHA::byte_t* in) NOEXCEPT
{
    // Fill gap if possible and update counter.
    const auto accepted = add_data(size, in);

    // Buffer overflow, no bytes were accepted (or size was zero).
    // If accepted is non-zero then the full size value is acceptable.
    if (is_zero(accepted))
        return is_zero(size);

    // No transformation on this update unless buffer is full.
    if (!is_full())
        return true;

    // Transform and clear the buffer.
    SHA::accumulate(state_, buffer_);
    std::advance(in, accepted);
    clear();

    // No more bytes to process.
    if (is_zero((size -= accepted)))
        return true;

    // Transform all whole blocks and save remainder to empty buffer.
    const auto blocks = size / SHA::block_bytes;
    const auto remain = size % SHA::block_bytes;

    const auto& from = unsafe_vector_cast<typename SHA::block_t>(in, blocks);
    SHA::accumulate(state_, from);
    std::advance(in, size - remain);
    increment(blocks);

    // Add the remaining partial block to the empty block buffer.
    add_data(remain, in);
    return true;
}

// public
template <typename SHA>
inline void accumulator<SHA>::
flush(typename SHA::byte_t* out32) NOEXCEPT
{
    const auto counter = serialize_counter();

    // pads are stored as BE integer, but this wants to write bytes.
    write(pad_size(), SHA::pad::stream.data());
    write(SHA::count_bytes, counter.data());
    serialize_state(unsafe_array_cast<typename SHA::byte_t,
        SHA::digest_bytes>(out32));
    reset();
}

BOOST_AUTO_TEST_SUITE_END()

////#include "temporary_accumulator.ipp"
