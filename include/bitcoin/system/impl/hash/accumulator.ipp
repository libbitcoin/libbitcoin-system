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
#ifndef LIBBITCOIN_SYSTEM_HASH_ACCUMULATOR_IPP
#define LIBBITCOIN_SYSTEM_HASH_ACCUMULATOR_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

#define TEMPLATE template <typename Algorithm, bool Checked, \
    if_base_of<algorithm_t, Algorithm> If>
#define CLASS accumulator<Algorithm, Checked, If>

// Copy and array index are guarded.
// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)

// private
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr size_t CLASS::
next() const NOEXCEPT
{
    return size_ % block_size;
}

TEMPLATE
INLINE constexpr size_t CLASS::
gap() const NOEXCEPT
{
    return block_size - next();
}

TEMPLATE
INLINE constexpr bool CLASS::
is_full() const NOEXCEPT
{
    return is_zero(next());
}

TEMPLATE
INLINE constexpr bool CLASS::
is_empty() const NOEXCEPT
{
    // Empty and full are the same, just a matter of caller context.
    return is_full();
}

TEMPLATE
INLINE constexpr bool CLASS::
is_buffer_overflow(size_t size) const NOEXCEPT
{
    // using count_t = typename Algorithm::count_t;
    // count_t is either uint64_t (SHA1/256/RMD) or uint128_t (SHA512).
    // It is not used in the accumulator, as a performance optimization.
    // This limits the accumulator to [max_size_t/8 - 8|16] hashed bytes.
    // This is over 2M TB, so not worth guarding in Bitcoin scenarios.
    // Checked is on by default in checked builds (NDEBUG) only.
    if constexpr (Checked)
        return size > (Algorithm::limit_bytes - size_);
    else
        return false;
}

TEMPLATE
INLINE constexpr bool CLASS::
add_data(size_t& accepted, size_t size, const byte_t* data) NOEXCEPT
{
    if (is_buffer_overflow(size))
        return false;

    accepted = std::min(gap(), size);
    add_data(accepted, data);
    return true;
}

TEMPLATE
INLINE constexpr void CLASS::
add_data(size_t size, const byte_t* data) NOEXCEPT
{
    std::copy_n(data, size, &buffer_[next()]);
    size_ += size;
}

TEMPLATE
INLINE constexpr size_t CLASS::
pad_size() const NOEXCEPT
{
    constexpr auto singled = block_size - count_size;
    constexpr auto doubled = block_size + singled;

    const auto used = next();
    return (used < singled ? singled - used : doubled - used);
}

TEMPLATE
INLINE typename CLASS::counter CLASS::
serialize(size_t size) NOEXCEPT
{
    // block_t (64 bytes), counter_t (64 bits), words_t (32 bits), byte_t (8 bits).
    // One block bit count (0x0200) in RMD (LE):
    // as byte_t: { (0x00, 0x20, 0x00, 0x00), (0x00, 0x00, 0x00, 0x00) }
    // as word_t: { [0x00000200], [0x00000000] }
    //
    // block_t (64 bytes), counter_t (64 bits), words_t (32 bits), byte_t (8 bits).
    // One block bit count (0x0200) in SHA1/SHA256 (BE):
    // as byte_t: { (0x00, 0x00, 0x00, 0x00), (0x00, 0x00, 0x02, 0x00) }
    // as word_t: { [0x00000000], [0x00000200] }
    //
    // block_t (128 bytes), counter_t (128 bits), words_t (64 bits), byte_t (8 bits).
    // One block bit count (0x0400) in SHA256 (BE):
    // as byte_t: { (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00),
    //              (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00) }
    // as word_t: { [0x0000000000000000], [0x0000000000000400] }
    //
    // Below is the generalized form, converting the 64/128 bit integral count_t
    // directly to BE/LE serialization. This will then be deserialized as word_t
    // by Algorithm at the start of block processing. Algorithm padding
    // optimizations set values directly into buffer_t (in the word_t form).

    if constexpr (Algorithm::big_end_count)
    {
        // to_big_endian_size is RCONSTEXPR.
        return to_big_endian_size<count_size>(to_bits(size));
    }
    else
    {
        return to_little_endian_size<count_size>(to_bits(size));
    }
}

TEMPLATE
CONSTEVAL typename CLASS::block_t CLASS::
stream_pad() NOEXCEPT
{
    // Specifications call for set of first pad bit in the serialized form,
    // so as byte_t (for block_t) this is already normalized for endianness.
    // For RMD (LE) it will deserialize as word_t and represent as 0x00000080.
    // For SHA (BE) it will deserialize as word_t and represent as 0x80000000.

    return { bit_hi<byte_t> };
}

// public
// ----------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
reset() NOEXCEPT
{
    size_ = zero;
    state_ = Algorithm::H::get;
}

TEMPLATE
constexpr CLASS::
accumulator() NOEXCEPT
  : size_{ zero }, state_{ Algorithm::H::get }
{
}

TEMPLATE
constexpr CLASS::
accumulator(size_t blocks, const state_t& state) NOEXCEPT
  : size_{ blocks * block_size }, state_{ state }
{
    if constexpr (Checked)
    {
        if (is_buffer_overflow(blocks * block_size) ||
            is_multiply_overflow(blocks, block_size))
            reset();
    }
}

TEMPLATE
template <size_t Size>
inline bool CLASS::
write(const std_array<byte_t, Size>& data) NOEXCEPT
{
    return write(Size, data.data());
}

TEMPLATE
inline bool CLASS::
write(const data_chunk& data) NOEXCEPT
{
    return write(data.size(), data.data());
}

TEMPLATE
inline bool CLASS::
write_slice(const data_slice& data) NOEXCEPT
{
    return write(data.size(), data.data());
}

TEMPLATE
inline bool CLASS::
write(size_t size, const byte_t* data) NOEXCEPT
{
    if (is_zero(size))
        return true;

    // With an empty buffer and at least one block, accumulate all.
    if (accumulate(size, data))
        return true;

    // False only on overflow.
    // Accept up to the number of bytes required to fill the buffer.
    size_t accepted{};
    if (!add_data(accepted, size, data))
        return false;

    // Nothing more to do.
    if (!is_full())
        return true;

    // Empty the full buffer and advance pointers by amount accepted.
    Algorithm::accumulate(state_, buffer_);
    std::advance(data, accepted);
    size -= accepted;

    // With at least one block (buffer is empty), accumulate all.
    if (accumulate(size, data))
        return true;

    add_data(size, data);
    return true;
}

// private
TEMPLATE
inline bool CLASS::
accumulate(size_t size, const byte_t* data) NOEXCEPT
{
    // Only accumulates one or more blocks with an empty buffer.
    if (!is_empty() || (size < block_size))
        return false;

    const auto blocks = size / block_size;
    const auto remain = size % block_size;
    const auto bytes  = size - remain;
    Algorithm::accumulate(state_, unsafe_vector_cast<block_t>(data, blocks));

    // Update the counter and add any remaining bytes to the buffer.
    size_ += bytes;
    add_data(remain, std::next(data, bytes));
    return true;
}

TEMPLATE
constexpr typename CLASS::digest_t CLASS::
flush() NOEXCEPT
{
    constexpr auto pad = stream_pad();
    const auto size = size_;
    write(pad_size(), pad.data());
    write(count_size, serialize(size).data());
    return Algorithm::finalize(state_);
}

TEMPLATE
inline void CLASS::
flush(digest_t& digest) NOEXCEPT
{
    constexpr auto pad = stream_pad();
    const auto size = size_;
    write(pad_size(), pad.data());
    write(count_size, serialize(size).data());
    Algorithm::finalize(digest, state_);
}

TEMPLATE
inline void CLASS::
flush(byte_t* digest) NOEXCEPT
{
    flush(unsafe_array_cast<byte_t, array_count<digest_t>>(digest));
}

TEMPLATE
inline void CLASS::
flush(data_chunk& digest) NOEXCEPT
{
    flush(digest.data());
}

TEMPLATE
inline void CLASS::
hash(byte_t* digest, const data_slice& data) NOEXCEPT
{
    accumulator<Algorithm> context{};
    context.write(data.size(), data.data());
    context.flush(digest);
}

TEMPLATE
template <size_t Size>
inline typename CLASS::digest_t CLASS::
hash(const std_array<byte_t, Size>& data) NOEXCEPT
{
    constexpr auto half = array_count<half_t>;
    constexpr auto full = array_count<block_t>;

    if constexpr (Size == half || Size == full)
    {
        return Algorithm::hash(data);
    }
    else
    {
        accumulator<Algorithm> context{};
        context.write(data);
        return context.flush();
    }
}

TEMPLATE
inline typename CLASS::digest_t CLASS::
hash(const data_chunk& data) NOEXCEPT
{
    accumulator<Algorithm> context{};
    context.write(data.size(), data.data());
    return context.flush();
}

TEMPLATE
inline typename CLASS::digest_t CLASS::
hash_digest(const data_slice& data) NOEXCEPT
{
    accumulator<Algorithm> context{};
    context.write(data.size(), data.data());
    return context.flush();
}

TEMPLATE
template <size_t Size>
inline data_chunk CLASS::
hash_chunk(const std_array<byte_t, Size>& data) NOEXCEPT
{
    constexpr auto size = array_count<digest_t>;
    data_chunk digest(size);
    auto& out = unsafe_array_cast<byte_t, size>(digest.data());
    out = hash(data);
    return digest;
}

TEMPLATE
inline data_chunk CLASS::
hash_chunk(const data_chunk& data) NOEXCEPT
{
    constexpr auto size = array_count<digest_t>;
    data_chunk digest(size);
    auto& out = unsafe_array_cast<byte_t, size>(digest.data());
    out = hash(data);
    return digest;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

} // namespace system
} // namespace libbitcoin

#endif
