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
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// private

constexpr typename compact::parse
compact::to_compact(small_type small) noexcept
{
    return
    {
        get_right(small, sub1(precision)),
        narrow_cast<exponent_type>(shift_right(small, precision)),
        mask_left<small_type>(small, e_width)
    };
}

constexpr typename compact::small_type
compact::from_compact(const parse& compact) noexcept
{
    return bit_or
    (
        shift_left(wide_cast<uint32_t>(compact.exponent), precision),
        compact.mantissa
    );
}

// public

//*****************************************************************************
// CONSENSUS: Only an invalid compact number could produce a big -1.
// This is precluded (at least) because mantissa high bit cannot be set.
// This assures `++(bit_not(target) / add1(target))` in header cannot result
// in a division-by-zero condition.
// CONSENSUS: Zero is a sufficient negative/zero/overflow sentinel:
// "if (negative || overflow || big == 0) return 0;" and only if the mantissa
// is zero can a logical shift within the domain produce a zero (fail early).
// CONSENSUS: Satoshi is more permissive, allowing a 34 exponent with a single
// byte mantissa, however this is not necessary to validate any value produced
// by compression, nor is it possible for any such value to affect consensus.
//*****************************************************************************
constexpr compact::span_type
compact::expand(small_type exponential) noexcept
{
    auto compact = to_compact(exponential);
    
    if (compact.negative)
        return 0;

    // strict validation
    if (compact.exponent == add1(e_max) &&
        ceilinged_log256(compact.mantissa) == sub1(m_bytes) &&
        get_right(compact.mantissa, sub1(to_bits(sub1(m_bytes)))))
    {
        compact.exponent--;
        compact.mantissa <<= raise(one);
    }
    
    return base256e::expand(from_compact(compact));
}

//*************************************************************************
// CONSENSUS:
// Due to an implementation artifact, the representation is not uniform. A
// high bit in the mantissa is pushed into the exponent, dropping mantissa
// by one  bit (an order of magnitude). Precision is naturally lost in
// compression, but the loss is not uniform due to this shifting out of the
// "sign" bit. There is of course never an actual negative mantissa sign in
// exponential notation of an unsigned number, so this was a mistake.
//*************************************************************************
constexpr compact::small_type
compact::compress(const span_type& number) noexcept
{
    auto compact = to_compact(base256e::compress(number));

    if (compact.negative)
    {
        compact.exponent++;
        compact.mantissa >>= raise(one);
        compact.negative = false;
    }

    return from_compact(compact);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
