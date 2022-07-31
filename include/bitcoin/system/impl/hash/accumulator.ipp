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

#define TEMPLATE template <typename Algorithm, bool Checked>
#define CLASS accumulator<Algorithm, Checked>

// Copy and array index are guarded.
// Buffer initialization is not required due to logical sizing.
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_UNINITIALZIED_MEMBER)

// private
// ----------------------------------------------------------------------------

TEMPLATE
constexpr void CLASS::
reset() NOEXCEPT
{
    size_ = zero;
    state_ = Algorithm::H::get;
}

TEMPLATE
constexpr size_t CLASS::
next() const NOEXCEPT
{
    return size_ % block_size;
}

TEMPLATE
constexpr size_t CLASS::
gap() const NOEXCEPT
{
    return block_size - next();
}

TEMPLATE
constexpr bool CLASS::
is_buffer_overflow(size_t bytes) const NOEXCEPT
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
constexpr size_t CLASS::
add_data(size_t bytes, const byte_t* data) NOEXCEPT
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
constexpr void CLASS::
increment(size_t blocks) NOEXCEPT
{
    BC_ASSERT_MSG(!is_buffer_overflow(blocks * block_size), "overflow");
    BC_ASSERT_MSG(!is_multiply_overflow(blocks, block_size), "overflow");

    // Guarded by proper (protected) response to preceding add_data call.
    // Caller must not increment more blocks than implied by add_data result.
    size_ += (blocks * block_size);
}

TEMPLATE
constexpr size_t CLASS::
pad_size() const NOEXCEPT
{
    constexpr auto singled = block_size - count_size;
    constexpr auto doubled = block_size + singled;

    const auto used = next();
    return (used < singled ? singled - used : doubled - used);
}

TEMPLATE
RCONSTEXPR typename CLASS::counter CLASS::
serialize(size_t bytes) NOEXCEPT
{
    if constexpr (Algorithm::big_end_count)
    {
        // to_big_endian_size is RCONSTEXPR.
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
        BC_ASSERT_MSG(!is_buffer_overflow(blocks * block_size), "overflow");
        BC_ASSERT_MSG(!is_multiply_overflow(blocks, block_size), "overflow");

        if (is_buffer_overflow(blocks * block_size) ||
            is_multiply_overflow(blocks, block_size))
            reset();
    }
}

TEMPLATE
inline bool CLASS::
write(const data_slice& data) NOEXCEPT
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
CONSTEVAL typename CLASS::block_t CLASS::
stream_pad() NOEXCEPT
{
    return { bit_hi<byte_t> };
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
inline bool CLASS::
write(size_t size, const byte_t* data) NOEXCEPT
{
    // TODO: data_slice pointer/size overload.
    return write(data_slice(data, std::next(data, size)));
}

TEMPLATE
inline void CLASS::
flush(byte_t* digest) NOEXCEPT
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

} // namespace system
} // namespace libbitcoin

#endif
