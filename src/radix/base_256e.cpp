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
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace base256e {

template <typename Integer>
constexpr Integer raise(Integer value) noexcept
{
    return value * (from / to);
}

template <typename Integer>
constexpr Integer lower(Integer value) noexcept
{
    return value / (from / to);
}

static_assert(raise(one) == to_bits(one));
static_assert(lower(to_bits(one)) == one);

// Implementation.
// ----------------------------------------------------------------------------

// exponent    0 implies  -3 shift, which always produces zero (min value).
// exponent    3 implies   0 shift, which is the identity (n * 256^0 == n).
// exponent   32 implies +29 shift, which produces max value (3 bytes left).
// exponent > 32 exponent is out of bounds, and produces a zero value.
number_type expand(compact_type exponential) noexcept
{
    const auto exponent = raise(shift_right(exponential, precision));
    const auto mantissa = mask_left<compact_type>(exponential, exponent_width);

    // Zero returned if unsigned exponent is out of bounds [0..32].
    if (is_limited(exponent, from))
        return 0;

    number_type number{ mantissa };

    exponent > precision ?
        number <<= exponent - precision :
        number >>= precision - exponent;

    return number;
}

// Only a zero value returns zero exponent and value (log(0) is undefined).
// Any other value produces a non-zero exponent and non-zero value.
// Where more than one representation may be possible (and allowed by expand),
// this compression produces a normal form, with minimal exponent selection.
compact_type compress(const number_type& number) noexcept
{
    if (is_zero(number))
        return 0;

    // This can only produce an exponent from [0..32] (zero excluded above).
    const auto exponent = raise(ceilinged_log256(number));
    const auto mantissa = static_cast<compact_type>
    (
        exponent > precision ?
            number >> exponent - precision :
            number << precision - exponent
    );

    return bit_or(shift_left(possible_narrow_cast<compact_type>(
        lower(exponent)), precision), mantissa);
}

} // namespace base256e
} // namespace system
} // namespace libbitcoin
