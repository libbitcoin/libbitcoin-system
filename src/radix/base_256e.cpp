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
#include <bitcoin/system/radix/base_256e.hpp>

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Derivation.
// ----------------------------------------------------------------------------

using exponent_type = uint8_t;
using mantissa_type = uint32_t;

// All parameters are derived, no magic numbers.
constexpr auto bits = 256u;
constexpr auto bytes = to_bytes(bits);
static_assert(bytes == 32u);

// exponent domain (stored [0..32] vs. logical [-3..29] exponent)
constexpr auto exponent_bits = ceilinged_log2(bytes);
constexpr auto alignment = width<exponent_type>() - exponent_bits;
constexpr auto exponent_width = alignment + exponent_bits;
static_assert(width<exponent_type>() == exponent_width);
static_assert(exponent_width == 8u);
static_assert(alignment == 2u);

// mantissa domain
static constexpr auto mantissa_width = bytes - exponent_width;
static_assert(width<mantissa_type>() == bytes);
static_assert(mantissa_width == 24u);

// exponentiation
template <typename Integer>
constexpr Integer raise(Integer value) noexcept
{
    return value * (bits / bytes);
}

static_assert(raise(one) == to_bits(one));

// shifting exponent reduces mantissa domain by aligned exponent width
static constexpr auto point = mantissa_width / raise(one);
static_assert(point == 3u);

// Implementation.
// ----------------------------------------------------------------------------

// exponent    0 implies  -3 shift, which always produces zero (min value).
// exponent    3 implies   0 shift, which is the identity (n * 256^0 == n).
// exponent   32 implies +29 shift, which produces max value (3 bytes left).
// exponent > 32 exponent is out of bounds, and produces a zero value.
// Note that the logical zero exponent (3) does not produce 1.
uint256_t expand_base256e(uint32_t small) noexcept
{
    const auto exponent = shift_right(small, mantissa_width);
    const auto mantissa = mask_left<mantissa_type>(small, exponent_width);

    // Zero returned if unsigned exponent is out of bounds [0..32].
    if (is_limited(exponent, bytes))
        return 0;

    uint256_t big{ mantissa };

    exponent > point ?
        big <<= raise(exponent - point) :
        big >>= raise(point - exponent);

    return big;
}

// Only a zero value returns zero exponent and value (log(0) is undefined).
// Any other value produces a non-zero exponent and non-zero value.
// Where more than one representation may be possible (and allowed by expand),
// this compression produces a normal form, with minimal exponent selection.
uint32_t compress_base256e(const uint256_t& big) noexcept
{
    if (is_zero(big))
        return 0;

    // This can only produce an exponent from [0..32] (zero excluded above).
    const auto exponent = ceilinged_log256(big);
    const auto mantissa = static_cast<mantissa_type>
    (
        exponent > point ?
            big >> raise(exponent - point) :
            big << raise(point - exponent)
    );

    return bit_or(shift_left(possible_narrow_cast<mantissa_type>(exponent),
        mantissa_width), mantissa);
}

} // namespace system
} // namespace libbitcoin
