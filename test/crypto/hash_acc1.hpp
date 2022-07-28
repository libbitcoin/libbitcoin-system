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

BOOST_AUTO_TEST_SUITE(sha_tests)

/// Accumulator for SHA/RMD/MD# streaming hash algorithms.
template <typename Algorithm, bool Checked = checked_build>
struct accumulator
{
    DEFAULT5(accumulator);
    using byte_t = typename Algorithm::byte_t;
    using state_t = typename Algorithm::state_t;
    using digest_t = typename Algorithm::digest_t;

    /// Sets initial state to Hash initialization vector.
    constexpr accumulator() NOEXCEPT;

    /// Accepts an initial state and count of blocks it has accumulated.
    constexpr accumulator(size_t blocks, const state_t& state) NOEXCEPT;

    /// Write data to accumulator.
    inline bool write(const data_slice& data) NOEXCEPT;
    inline bool write(size_t size, const byte_t* data) NOEXCEPT;

    /// Flush accumulator state to digest.
    constexpr digest_t flush() NOEXCEPT;
    inline void flush(byte_t* digest) NOEXCEPT;

    // Reset accumulator to initial state (to reuse after flushing).
    constexpr void reset() NOEXCEPT;

private:
    using block_t = typename Algorithm::block_t;
    using counter = data_array<Algorithm::count_bytes>;

    // Position of next write in the buffer.
    constexpr size_t next() const NOEXCEPT;

    // Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    // Accumulator is limited to [max_size_t/8 - 8|16] hashed bytes.
    constexpr bool is_buffer_overflow(size_t bytes) NOEXCEPT;

    // Append up to block_size bytes to buffer.
    constexpr size_t add_data(size_t bytes, const byte_t* data) NOEXCEPT;

    // Increment the counter for unbuffered transforms.
    constexpr void increment(size_t blocks) NOEXCEPT;

    // Compute pad size, reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    // Serialize the hashed byte count for finalization
    static constexpr counter serialize(size_t bytes) NOEXCEPT;

    // Precomputed streaming pad buffer.
    static CONSTEVAL block_t stream_pad() NOEXCEPT;

    // Number of bytes in a block and in counter.
    static constexpr auto block_size = array_count<block_t>;
    static constexpr auto count_size = array_count<counter>;

    size_t size_;
    state_t state_;
    block_t buffer_;
};

// private
// ----------------------------------------------------------------------------

#define TEMPLATE template <typename Algorithm, bool Checked>
#define CLASS accumulator<Algorithm, Checked>

// Copy and array index are guarded.
// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)

TEMPLATE
constexpr CLASS::accumulator() NOEXCEPT
  : size_{ zero }, state_{ Algorithm::H::get }
{
}

TEMPLATE
constexpr CLASS::accumulator(size_t blocks, const state_t& state) NOEXCEPT
  : size_{ blocks }, state_{ state }
{
}

TEMPLATE
constexpr void CLASS::reset() NOEXCEPT
{
    size_ = zero;
    state_ = Algorithm::H::get;
}

TEMPLATE
constexpr size_t CLASS::next() const NOEXCEPT
{
    return size_ % block_size;
}

TEMPLATE
constexpr size_t CLASS::gap() const NOEXCEPT
{
    return block_size - next();
}

TEMPLATE
constexpr bool CLASS::is_buffer_overflow(size_t bytes) NOEXCEPT
{
    if constexpr (Checked)
    {
        return bytes > (Algorithm::limit_bytes - size_);
    }
    else
    {
        // using count_t = typename Algorithm::count_t;
        // count_t is either uint64_t (SHA1/256/RMD) or uint128_t (SHA512).
        // It is not used in the accumulator, as a performance optimization.
        // This limits the accumulator to [max_size_t/8 - 8|16] hashed bytes.
        // This is over 2M TB, so not worth guarding in Bitcoin scenarios.
        // Checked is on by default in checked builds (NDEBUG) only.
        return false;
    }
}

TEMPLATE
constexpr size_t CLASS::add_data(size_t bytes, const byte_t* data) NOEXCEPT
{
    // No bytes accepted on overflow (if checked) or uncleared buffer.
    if (is_buffer_overflow(bytes) || is_zero(gap()))
        return zero;

    const auto accepted = std::min(gap(), bytes);
    std::copy_n(data, accepted, &buffer_[next()]);
    size_ += accepted;
    return accepted;
}

TEMPLATE
constexpr void CLASS::increment(size_t blocks) NOEXCEPT
{
    BC_ASSERT_MSG(!is_buffer_overflow(blocks * block_size), "overflow");
    BC_ASSERT_MSG(!is_multiply_overflow(blocks, block_size), "overflow");

    // Guarded by proper (protected) response to preceding add_data call.
    // Caller must not increment more blocks than implied by add_data result.
    size_ += (blocks * block_size);
}

TEMPLATE
constexpr size_t CLASS::pad_size() const NOEXCEPT
{
    constexpr auto singled = block_size - count_size;
    constexpr auto doubled = block_size + singled;

    const auto used = next();
    return (used < singled ? singled - used : doubled - used);
}

TEMPLATE
constexpr typename CLASS::counter
CLASS::serialize(size_t bytes) NOEXCEPT
{
    if constexpr (Algorithm::big_end_count)
    {
        return to_big_endian_size<count_size>(to_bits(bytes));
    }
    else
    {
        return to_little_endian_size<count_size>(to_bits(bytes));
    }
}

// public
// ----------------------------------------------------------------------------

TEMPLATE
inline bool CLASS::write(const data_slice& data) NOEXCEPT
{
    auto size = data.size();
    auto pdata = data.data();

    // Fill gap if possible and update counter.
    const auto accepted = add_data(size, pdata);

    // No bytes accepted: buffer overflow (if checked) or size is zero.
    // If accepted is non-zero then the full size value is acceptable.
    if (is_zero(accepted))
        return is_zero(size);

    // No transformation on this write unless buffer is full.
    if (!is_zero(next()))
        return true;

    // Transform (updates state and clears buffer).
    Algorithm::accumulate(state_, buffer_);
    std::advance(pdata, accepted);

    // No more bytes to process.
    if (is_zero((size -= accepted)))
        return true;

    // Get count of whole blocks and remaining bytes.
    const auto count = size / block_size;
    const auto bytes = size % block_size;

    // write is inline vs constexpr because of this cast. This could instead
    // be passed as a pointer, but then Hash (algorithm) would have to cast.
    // Vector cast constructs a vector holding a ref (ponter) to each block.
    const auto blocks = unsafe_vector_cast<block_t>(pdata, count);

    // Transform all whole blocks and save remainder to cleared buffer.
    Algorithm::accumulate(state_, blocks);
    std::advance(pdata, size - bytes);
    increment(count);

    // Add the remaining partial block to the cleared block buffer.
    // There is no point in testing the add_data return value here.
    add_data(bytes, pdata);
    return true;
}

TEMPLATE
CONSTEVAL CLASS::block_t CLASS::stream_pad() NOEXCEPT
{
    return { bit_hi<byte_t> };
}

TEMPLATE
constexpr CLASS::digest_t CLASS::flush() NOEXCEPT
{
    constexpr auto pad = stream_pad();

    const auto size = size_;
    write(pad_size(), pad.data());
    write(count_size, serialize(size).data());
    return Algorithm::finalize(state_);
}

TEMPLATE
inline bool CLASS::write(size_t size, const byte_t* data) NOEXCEPT
{
    // TODO: data_slice pointer/size overload.
    return write(data_slice(data, std::next(data, size)));
}

TEMPLATE
inline void CLASS::flush(byte_t* digest) NOEXCEPT
{
    // TODO: could also provide a data_slab overload.
    unsafe_array_cast<byte_t, array_count<digest_t>>(digest) = flush();
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

BOOST_AUTO_TEST_SUITE_END()

////#include "temporary_accumulator.ipp"
