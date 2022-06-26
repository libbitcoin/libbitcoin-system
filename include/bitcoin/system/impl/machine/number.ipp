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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <bitcoin/system/constants.hpp>
////#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
namespace number {

constexpr uint8_t positive_sign_byte = 0x00;
constexpr uint8_t negative_sign_byte = bit_hi<uint8_t>();

// integer
// ----------------------------------------------------------------------------
// C++20: constexpr.

template <size_t Size>
inline bool integer<Size>::from_int(Integer& out, int64_t vary) noexcept
{
    out = possible_narrow_cast<Integer>(vary);

    // Disallows integers that exceed overflow constraint.
    return !is_overflow(vary);
}

template <size_t Size>
inline bool integer<Size>::from_chunk(Integer& out,
    const data_chunk& vary) noexcept
{
    out = 0;

    if (strict_zero(vary))
        return true;

    // Disallow integers exceeding overflow constraint (causes domain reduction).
    if (vary.size() > Size)
        return false;

    out = from_little_endian<Integer>(vary);

    // -minimum<signed> is undefined behavior?
    if (get_left(vary.back()))
        out = -set_right(out, sub1(to_bits(vary.size())), false);

    return true;
}

template <size_t Size>
inline bool integer<Size>::strict_zero(const data_chunk& vary) noexcept
{
    return vary.empty();
}

// ****************************************************************************
// CONSENSUS: Due to compression, a leading sign byte is required if high bit
// is set. This produces two zero representations (+/-0). The subsequent byte-
// based overflow constraint thereby restricts the stack's 64-bit integer
// domain by one value. An implementation that either avoided vectorization or
// vector compression in the first place, or later implemented the overflow
// guard over the converted integeral would have avoided this seam. These were
// both premature optimizations, as a variant stack is more efficient than
// variably-lengthed byte vector integral storage. The resulting integer domain
// reduction is captured by the is_overflow function below.
// ****************************************************************************
template <size_t Size>
inline bool integer<Size>::is_overflow(int64_t value) noexcept
{
    ////static_assert(bitcoin_min<Size>() == add1(minimum<Size>()));

    // Encoding of negative zero constricts the integer domain by one.
    return is_limited(value, bitcoin_min<Size>(), bitcoin_max<Size>());
}

// chunk
// ----------------------------------------------------------------------------

inline data_chunk chunk::from_bool(bool vary) noexcept
{
    return { bc::to_int<uint8_t>(vary) };
}

inline data_chunk chunk::from_int(int64_t vary) noexcept
{
    // Empty chunk is constexpr (stack allocated).
    if (is_zero(vary))
        return {};

    // Variably-sized (compressed) encoding of absolute value.
    auto bytes = to_little_endian_chunk(absolute(vary), one);
    const auto hi_bit = get_left(bytes.back());

    // Due to compression a leading sign byte is required if high bit is set.
    if (hi_bit && is_negative(vary))
        bytes.push_back(negative_sign_byte);
    else if (hi_bit)
        bytes.push_back(positive_sign_byte);
    else if (is_negative(vary))
        set_left_into(bytes.back());

    return bytes;
}

// boolean
// ----------------------------------------------------------------------------
// C++20: constexpr.

template <size_t Size>
inline signed_type<Size> boolean::to_int(bool vary) noexcept
{
    static_assert(Size <= sizeof(int64_t), "type constraint");

    // The cast can safely be ignored, which is why Size is defaulted.
    return bc::to_int<signed_type<Size>>(vary);
}

inline bool boolean::from_chunk(const data_chunk& vary) noexcept
{
    // Booleans are not overflow constrained, so cannot be converted as int.
    // Any length of bytes is considered valid. Negative zero is false.
    return !strict_false(vary)
        || !is_sign_byte(vary.back())
        || std::any_of(vary.begin(), std::prev(vary.end()),
            is_nonzero<uint8_t>);
}

inline bool boolean::strict_from_chunk(const data_chunk& vary) noexcept
{
    // A logical zero equates to any +/- sequence of zero bytes up to 520.
    // Strict bool tests for integral false/zero, or a single empty byte.
    return strict_false(vary);
}

inline bool boolean::to_bool(int64_t vary) noexcept
{
    return bc::to_bool(vary);
}

inline bool boolean::strict_false(const data_chunk& vary) noexcept
{
    return vary.empty();
}

inline bool boolean::is_sign_byte(uint8_t byte) noexcept
{
    return byte == positive_sign_byte || byte == negative_sign_byte;
}

} // namespace number
} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
