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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_2N_IPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_2N_IPP

#include <cstddef>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// e = 0       -> shift(-m_bytes)         zero       (m = minimum value)
// e = m_bytes -> shift(0)                identity   (m = m * base^0)
// e = e_max   -> shift(e_max - m_bytes)  maximum    (m = high m_bytes)
// e > e_max   -> n/a                     zero       (m = invalid value)

// This expansion limits the exponent to e_bits, ensuring that there is only
// one compressed representation for any given span of bits.
template <size_t Base, size_t Precision, size_t Span>
constexpr typename base2n<Base, Precision, Span>::span_type
base2n<Base, Precision, Span>::expand(small_type exponential) noexcept
{
    const auto shift = raise(shift_right(exponential, precision));
    const auto mantissa = mask_left<small_type>(exponential, e_width);

    // Zero returned if unsigned exponent is out of bounds [0..e_max].
    if (is_limited(shift, span))
        return 0;

    span_type number{ mantissa };

    shift > precision ?
        number <<= (shift - precision) :
        number >>= (precision - shift);

    return number;
}

// Only a zero value returns zero exponent and value (log(0) is undefined).
// Any other value produces a non-zero exponent and non-zero value.
// Where more than one representation may be possible, this compression
// produces a normal form, with minimal exponent selection.
template <size_t Base, size_t Precision, size_t Span>
constexpr typename base2n<Base, Precision, Span>::small_type
base2n<Base, Precision, Span>::compress(const span_type& number) noexcept
{
    if (is_zero(number))
        return 0;

    // This can only produce an exponent from [0..e_max] (zero excluded above).
    const auto shift = raise(ceilinged_log<base>(number));
    const auto mantissa = static_cast<small_type>
    (
        shift > precision ?
            number >> (shift - precision) :
            number << (precision - shift)
    );

    return bit_or(shift_left(possible_narrow_cast<small_type>(lower(shift)),
        precision), mantissa);
}

} // namespace system
} // namespace libbitcoin

#endif
