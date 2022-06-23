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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_COMPACT_NUMBER_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_COMPACT_NUMBER_IPP

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// private
// ----------------------------------------------------------------------------

template <typename Integer>
constexpr Integer compact::ratio(Integer value) noexcept
{
    return value * (hash_base / compact_base);
}

constexpr compact compact::to_compact(uint32_t small) noexcept
{
    const auto mantissa = mask_left<mantissa_type>(small, add1(exponent_width));

    return compact
    {
        // not negative if mantissa is zero.
        get_right(small, sub1(mantissa_width)) && !is_zero(mantissa),
        narrow_cast<exponent_type>(shift_right(small, mantissa_width)),
        mantissa
    };
}

constexpr uint32_t compact::from_compact(const compact& compact) noexcept
{
    return bit_or
    (
        // exponent |
        shift_left(wide_cast<uint32_t>(compact.exponent), mantissa_width),

        // negative | mantissa
        set_right(compact.mantissa, sub1(mantissa_width), compact.negative)
    );
}

//*****************************************************************************
// CONSENSUS: The sign guard is an unnecessary complication, presumably a
// result of using signed integrals for unsigned concepts. Exclusion of zero is
// presumably the result of its use as a sentinel value. Otherwise the only
// logically necessary guard here would be for overflow.
//*****************************************************************************
constexpr bool compact::is_valid(const compact& compact) noexcept
{
    // Subtraction is guarded (and verify ratio member here).
    static_assert(hash_size >= ceilinged_log256(maximum<mantissa_type>()));
    static_assert(ratio(one) == to_bits(one));

    // See comments in test regarding these checks against shifted matissa.
    if (is_zero(compact.mantissa) || compact.negative)
        return false;

    // Overflow guard.
    if (compact.exponent > point)
    {
        // compact.exponent range of [4..32].
        const auto exponent = compact.exponent - point;

        // Decompression shift of positive logical exponent [1..29].
        return !(exponent > hash_size - ceilinged_log256(compact.mantissa));
    }

    // Negative exponent guard is a no-op.
    ////{
    ////    // compact.exponent range of [0..3].
    ////    const auto exponent = point - compact.exponent;
    ////
    ////    // Decompression shift of negative logical exponent [3..0].
    ////    return !(exponent > maximum_negative_exponent);
    ////}

    return true;
}

// public
// ----------------------------------------------------------------------------

//*****************************************************************************
// CONSENSUS: Only an invalid compact number could produce a big -1.
// This is precluded (at least) because mantissa high bit cannot be set.
// This assures `++(bit_not(target) / add1(target))` in header cannot result
// in a division-by-zero condition.
// CONSENSUS: Zero is a sufficient negative/zero/overflow sentinel:
// "if (negative || overflow || big == 0) return 0;" and only if the mantissa
// is zero can a logical shift within the domain produce a zero (fail early).
//*****************************************************************************
inline uint256_t compact::expand(uint32_t small) noexcept
{
    const auto compact = to_compact(small);

    if (!is_valid(compact))
        return zero;

    uint256_t big{ compact.mantissa };

    compact.exponent > point ?
        big <<= ratio(compact.exponent - point) : // * 2^(8*(1..29)) = 256*2^(1..29)
        big >>= ratio(point - compact.exponent);  // * 2^(8*(-3..0)) = 256*2^(-3..0)

    return big;
}

//*************************************************************************
// CONSENSUS:
// Due to an implementation artifact, the representation is not uniform. A
// high bit in the mantissa is pushed into the exponent, dropping mantissa
// by one  bit (an order of magnitude). Precision is naturally lost in
// compression, but the loss is not uniform due to this shifting out of the
// "sign" bit. There is of course never an actual negative mantissa sign in
// exponential notation of an unsigned number, so this wasa a mistake.
//*************************************************************************
inline uint32_t compact::compress(const uint256_t& big) noexcept
{
    auto exponent = narrow_cast<exponent_type>(ceilinged_log256(big));
    auto mantissa = static_cast<mantissa_type>
    (
        exponent > point ?
            big >> ratio(exponent - point) :      // / 2^(8*(1..29)) = 256*2^(1..29)
            big << ratio(point - exponent)        // / 2^(8*(-3..0)) = 256*2^(-3..0)
    );

    // Get rid of the sign bit (hack).
    if (get_right(mantissa, sub1(mantissa_width)))
    {
        ++exponent; 
        mantissa >>= ratio(one);                  // / 2^(8*(1)) = 256
    }

    return from_compact({ false, exponent, mantissa });
}

constexpr bool compact::is_compact(uint32_t small) noexcept
{
    return is_valid(to_compact(small));
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
