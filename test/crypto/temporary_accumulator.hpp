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

#if defined(NDEBUG)
constexpr auto checked = false;
#else
constexpr auto checked = true;
#endif

/// Hash accumulator.
template <typename Hash, bool Checked = checked>
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
    inline bool write(size_t size, const typename Hash::byte_t* in) NOEXCEPT;

    /// Flush context to out32 (must be at least 32 bytes), resets context.
    inline void flush(typename Hash::byte_t* out32) NOEXCEPT;

private:
    using counter = data_array<Hash::count_bytes>;

    // Clear buffer after transforming state.
    constexpr void clear() NOEXCEPT;

    // Reset to initial context after flushing.
    constexpr void reset() NOEXCEPT;

    // Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    // Transform if buffer is full.
    constexpr bool is_full() NOEXCEPT;

    // SHA512 limited to [max_uint128 - 128] hashed bits.
    // SHA1/256/RMD limited to [max_uint64 - 64] hashed bits.
    constexpr bool is_buffer_overflow(size_t bytes) NOEXCEPT;

    // Reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    // Append up to block_size bytes to buffer.
    constexpr size_t add_data(size_t bytes,
        const typename Hash::byte_t* data) NOEXCEPT;

    // Increment the counter for unbuffered transforms.
    constexpr void increment(size_t blocks) NOEXCEPT;

    // Serialize the hashed bit count for finalization
    inline counter serialize_counter() const NOEXCEPT;

    // Normalize state as the final hash value.
    inline void serialize_state(typename Hash::digest_t& out) const NOEXCEPT;

    size_t size_{};
    typename Hash::count_t bits_{};
    typename Hash::block_t buffer_;
    typename Hash::state_t state_{ Hash::H::get };
};

// implementation
// ----------------------------------------------------------------------------
// bits_ (count_t) is either uint64_t (SHA-1/256) or uint128_t (SHA).

// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)
template <typename Hash, bool Checked>
constexpr accumulator<Hash, Checked>::
accumulator() NOEXCEPT
BC_POP_WARNING()
{
}

template <typename Hash, bool Checked>
constexpr void accumulator<Hash, Checked>::
clear() NOEXCEPT
{
    // TODO: reduce to single size_ counter, use size_ % block_bytes for gap.
    // TODO: this also eliminates the need to clear the buffer.
    size_ = zero;
}

template <typename Hash, bool Checked>
constexpr void accumulator<Hash, Checked>::
reset() NOEXCEPT
{
    size_ = zero;
    bits_ = zero;
    state_ = Hash::H::get;
}

template <typename Hash, bool Checked>
constexpr size_t accumulator<Hash, Checked>::
gap() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_subtract_overflow(Hash::block_bytes, size_), "fault");
    return Hash::block_bytes - size_;
}

template <typename Hash, bool Checked>
constexpr bool accumulator<Hash, Checked>::
is_full() NOEXCEPT
{
    return is_zero(gap());
}

template <typename Hash, bool Checked>
constexpr bool accumulator<Hash, Checked>::
is_buffer_overflow(size_t bytes) NOEXCEPT
{
    BC_ASSERT_MSG(!is_zero(bits_ % byte_bits), "fault");

    if constexpr (Checked)
    {
        using counter = Hash::count_t;
        constexpr auto size = possible_wide_cast<counter>(Hash::count_bytes);
        const auto more = possible_wide_cast<counter>(bytes);
        const auto used = to_floored_bytes(bits_);
        return
            is_add_overflow<counter>(used, more) ||
            is_add_overflow<counter>(size, used + more);
    }
    else
    {
        // SHA512 limited to [max_uint128 - 128] hashed bits.
        // SHA1/256/RMD limited to [max_uint64 - 64] hashed bits.
        // This is over 2M TB with a 64 bit counter, so it is not worth
        // guarding in Bitcoin scenarios. Checked is off by default in NDEBUG.
        return false;
    }
}

template <typename Hash, bool Checked>
constexpr size_t accumulator<Hash, Checked>::
pad_size() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_limited(size_, Hash::block_bytes), "fault");
    constexpr auto singled = Hash::block_bytes - Hash::count_bytes;
    constexpr auto doubled = Hash::block_bytes + singled;
    return size_ < singled ? singled - size_ : doubled - size_;
}

// Copy and array index are guarded.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
template <typename Hash, bool Checked>
constexpr size_t accumulator<Hash, Checked>::
add_data(size_t bytes, const typename Hash::byte_t* data) NOEXCEPT
{
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

template <typename Hash, bool Checked>
constexpr void accumulator<Hash, Checked>::
increment(size_t blocks) NOEXCEPT
{
    // This is guarded by proper response to prior call to add_data.
    BC_ASSERT_MSG(!is_buffer_overflow(blocks * Hash::block_bytes), "overflow");
    bits_ += to_bits(blocks * Hash::block_bytes);
}

template <typename Hash, bool Checked>
inline typename accumulator<Hash, Checked>::counter accumulator<Hash, Checked>::
serialize_counter() const NOEXCEPT
{
    // TODO: conversions are suboptimal in order to support uint128_t (512).
    if constexpr (Hash::big_end_count)
    {
        return to_big_endian_size<Hash::count_bytes>(bits_);
    }
    else
    {
        return to_little_endian_size<Hash::count_bytes>(bits_);
    }
}

template <typename Hash, bool Checked>
inline void accumulator<Hash, Checked>::
serialize_state(typename Hash::digest_t& out) const NOEXCEPT
{
    out = Hash::finalize(state_);
}

// public
template <typename Hash, bool Checked>
inline bool accumulator<Hash, Checked>::
write(size_t size, const typename Hash::byte_t* in) NOEXCEPT
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
    Hash::accumulate(state_, buffer_);
    std::advance(in, accepted);
    clear();

    // No more bytes to process.
    if (is_zero((size -= accepted)))
        return true;

    // Transform all whole blocks and save remainder to empty buffer.
    const auto blocks = size / Hash::block_bytes;
    const auto remain = size % Hash::block_bytes;

    // This could instead be passed as a pointer and cast into blocks.
    const auto from = unsafe_vector_cast<typename Hash::block_t>(in, blocks);
    Hash::accumulate(state_, from);
    std::advance(in, size - remain);
    increment(blocks);

    // Add the remaining partial block to the empty block buffer.
    add_data(remain, in);
    return true;
}

// public
template <typename Hash, bool Checked>
inline void accumulator<Hash, Checked>::
flush(typename Hash::byte_t* out32) NOEXCEPT
{
    using to = typename Hash::byte_t;
    const auto counter = serialize_counter();
    write(pad_size(), Hash::pad::stream.data());
    write(Hash::count_bytes, counter.data());
    serialize_state(unsafe_array_cast<to, Hash::digest_bytes>(out32));
    reset();
}

BOOST_AUTO_TEST_SUITE_END()

////#include "temporary_accumulator.ipp"
