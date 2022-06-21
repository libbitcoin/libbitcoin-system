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
#include <bitcoin/system/chain/compact.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// private
bool compact::is_signed(size_t compact) noexcept
{
    return get_right(compact, sub1(mantissa_width));
}

// private
bool compact::is_valid(size_t exponent, uint32_t mantissa) noexcept
{
    //*************************************************************************
    // CONSENSUS: High mantissa bit (sign) set returns zero (invalid).
    // Zero mantissa would not be shifted and would return zero (invalid).
    // Presumably these were originally caused by "if (mantissa > 0)..."
    //*************************************************************************
    if (is_zero(mantissa) || is_signed(mantissa))
        return false;

    // Nothing to shift (above), or no left shift to cause an overflow.
    if (!(exponent > point))
        return true;

    // If (<< bits) > (256 - mantissa width) then would shift to zero.
    return !((exponent - point) > (bit_width - ceilinged_log256(mantissa)));
}

// Used only for chain_state assertion against previous block bits.
bool compact::is_valid(uint32_t compact) noexcept
{
    const auto mantissa = bit_and(compact, mantissa_bits);
    const size_t exponent = shift_right(compact, mantissa_width);
    return is_valid(exponent, mantissa);
}

uint32_t compact::compress(const uint256_t& big) noexcept
{
    // Create the exponential components.
    auto exponent = ceilinged_log256(big);
    auto mantissa = static_cast<size_t>
    (
        exponent > point ?
            big >> to_bits(exponent - point) : // / 2^(8*(004..255))
            big << to_bits(point - exponent)   // * 2^(8*(000..003))
    );

    // Drop high mantissa bit into exponent (precision loss).
    // Presumably neccesitated by unfortunate negative guard in from_compact.
    if (is_signed(mantissa))
    {
        ++exponent;
        mantissa >>= to_bits(one);             // / 2^(8*1)
    }

    // Assemble the exponential representation.
    return narrow_cast<uint32_t>(bit_or(shift_left(exponent, mantissa_width),
        mantissa));
}

bool compact::expand(uint256_t& big, uint32_t small) noexcept
{
    // Parse the exponential representation.
    const auto mantissa = bit_and(small, mantissa_bits);
    const auto exponent = shift_right(small, mantissa_width);

    //*************************************************************************
    // CONSENSUS: Only an invalid compact number could produce a -1 big number.
    // This is precluded (at least) because mantissa high bit cannot be set.
    // This assures that `add1(bit_not(target) / add1(target))` in header will
    // not result in a division-by-zero condition.
    //*************************************************************************
    if (!is_valid(exponent, mantissa))
    {
        big = zero;
        return false;
    }

    big = mantissa;

    exponent > point ?
        big <<= to_bits(exponent - point) :    // * 2^(8*(004..255))
        big >>= to_bits(point - exponent);     // / 2^(8*(000..003))

    return true;
}

uint256_t compact::expand(uint32_t small) noexcept
{
    uint256_t big;
    expand(big, small);
    return big;
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
