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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_IPP

#include <algorithm>
#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {
    
// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)
constexpr context::context() NOEXCEPT
BC_POP_WARNING()
{
}

constexpr void context::clear() NOEXCEPT
{
    size_ = zero;
}

constexpr void context::reset() NOEXCEPT
{
    size_ = zero;
    bits_ = 0_u64;
    state_ = initial;
}

constexpr size_t context::gap() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_subtract_overflow(block_size, size_), "unexpected size");
    return block_size - size_;
}

constexpr bool context::is_full() NOEXCEPT
{
    return is_zero(gap());
}

constexpr bool context::is_buffer_overflow(size_t bytes) NOEXCEPT
{
    BC_ASSERT_MSG(is_zero(bits_ % byte_bits), "unexpected bit count");

    const auto used = to_floored_bytes(bits_);
    return is_add_overflow<uint64_t>(used, bytes) ||
        is_add_overflow<uint64_t>(counter_size, add(used, bytes));
}

constexpr size_t context::pad_size() const NOEXCEPT
{
    BC_ASSERT_MSG(!is_limited(size_, block_size), "unexpected size");

    constexpr auto singled = block_size - counter_size;
    constexpr auto doubled = block_size + singled;
    return size_ < singled ? singled - size_ : doubled - size_;
}

// Copy and array index are guarded.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
constexpr size_t context::add_data(size_t bytes, const uint8_t* data) NOEXCEPT
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

constexpr void context::increment(size_t blocks) NOEXCEPT
{
    BC_ASSERT_MSG(!is_buffer_overflow(blocks * block_size), "unexpected size");

    bits_ += to_bits(blocks * block_size);
}

inline context::counter context::serialize_counter() const NOEXCEPT
{
    // Bit count is encoded into message block as 64 bit big-endian.
    // This is conventional and not subject to platform endianness.
    return to_big_endian(bits_);
}

inline void context::serialize_state(digest& out) const NOEXCEPT
{
    finalize(state_, array_cast<uint8_t>(out).data());
}

// public
inline bool context::write(size_t size, const uint8_t* in) NOEXCEPT
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
    update(state_, one, buffer_.data());
    std::advance(in, accepted);
    clear();

    // No more bytes to process.
    if (is_zero((size -= accepted)))
        return true;

    // Transform all whole blocks and save remainder to empty buffer.
    const auto blocks = size / block_size;
    const auto remain = size % block_size;

    update(state_, blocks, in);
    std::advance(in, size - remain);
    increment(blocks);

    // Add the remaining partial block to the empty block buffer.
    add_data(remain, in);
    return true;
}

// public
inline void context::flush(uint8_t* out32) NOEXCEPT
{
    const auto counter = serialize_counter();

    write(pad_size(), stream_pad.data());
    write(counter_size, counter.data());
    serialize_state(unsafe_array_cast<uint8_t, digest_size>(out32));
    reset();
}

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
