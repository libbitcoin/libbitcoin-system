/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_SHA_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_SHA_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_128.hpp>

namespace libbitcoin {
namespace system {

// HAVE_SHANI implies HAVE_SSE41, which defines xint128_t (__m128i).
#if defined(HAVE_SHANI)

// SHA1 (SHA160)
// ----------------------------------------------------------------------------

INLINE xint128_t sha160_message1(xint128_t a, xint128_t b) NOEXCEPT
{
    // "Perform an intermediate calculation for the next four SHA1 message
    // values (unsigned 32-bit integers) using previous message values from a
    // and b, and store the result in destination." - Intel
    return mm_sha1msg1_epu32(a, b);
}

INLINE xint128_t sha160_message2(xint128_t a, xint128_t b) NOEXCEPT
{
    // "Perform the final calculation for the next four SHA1 message values
    // (unsigned 32-bit integers) using the intermediate result in a and the
    // previous message values in b, and store the result in destination."
    // - Intel
    return mm_sha1msg2_epu32(a, b);
}

template <int Functor,
    if_not_lesser<Functor, 0> = true, if_not_greater<Functor,3> = true>
INLINE xint128_t sha160_four_rounds(xint128_t a, xint128_t b) NOEXCEPT
{
    // "Perform four rounds of SHA1 operation using an initial SHA1 state
    // (A,B,C,D) from a and some pre-computed sum of the next 4 round message
    // values (unsigned 32-bit integers), and state variable E from b, and
    // store the updated SHA1 state (A,B,C,D) in destination. Functor contains
    // the logic functions and round constants." - Intel
    return mm_sha1rnds4_epu32(a, b, Functor);
}

INLINE xint128_t sha160_next_e(xint128_t a, xint128_t b) NOEXCEPT
{
    // "Calculate SHA1 state variable E after four rounds of operation from
    // the current SHA1 state variable a, add that value to the scheduled
    // values (unsigned 32-bit integers) in b, and store the result in
    // destination." - Intel
    return mm_sha1nexte_epu32(a, b);
}

// SHA256
// ----------------------------------------------------------------------------

INLINE xint128_t sha256_message1(xint128_t a, xint128_t b) NOEXCEPT
{
    // Perform an intermediate calculation for the next four SHA256 message
    // values (unsigned 32-bit integers) using previous message values from a
    // and b, and store the result in destination." - Intel
    return mm_sha256msg1_epu32(a, b);
}

INLINE xint128_t sha256_message2(xint128_t a, xint128_t b) NOEXCEPT
{
    // Perform the final calculation for the next four SHA256 message values
    // (unsigned 32-bit integers) using previous message values from a and b,
    // and store the result in destination." - Intel
    return mm_sha256msg2_epu32(a, b);
}

// wk is w+k in a two (x 32 bit) lane vector (sha256rnds2 performs two rounds).
INLINE xint128_t sha256_two_rounds(xint128_t a, xint128_t b, xint128_t wk) NOEXCEPT
{
    // Perform 2 rounds of SHA256 operation using an initial SHA256 state
    // (C,D,G,H) from a, an initial SHA256 state (A,B,E,F) from b, and a
    // pre-computed sum of the next 2 round message values (unsigned 32-bit
    // integers) and the corresponding round constants from k, and store the
    // updated SHA256 state (A,B,E,F) in destination." - Intel
    return mm_sha256rnds2_epu32(a, b, wk);
}

#endif

} // namespace system
} // namespace libbitcoin

#endif
