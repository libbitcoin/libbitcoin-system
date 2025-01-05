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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_FUNCTIONS_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_FUNCTIONS_IPP

// 4.1 Functions
// ============================================================================
// The integral [f]unction namespace is implemented in math/function.hpp.
// When available vector overrides (e.g. xint128_t, xint256_t, xint512_t) are
// implemented in `intrinsics/xcpu/...` and `intrinsics/arm/...` (for example).

namespace libbitcoin {
namespace system {
namespace sha {
    
// Function implementations.
// ----------------------------------------------------------------------------
// protected

TEMPLATE
INLINE constexpr auto CLASS::
parity(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form, unmodified.
    return f::xor_(f::xor_(x, y), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
choice(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form, optimized.
    ////return (x & y) ^ (~x & z);
    return f::xor_(f::and_(x, f::xor_(y, z)), z);
}

TEMPLATE
INLINE constexpr auto CLASS::
majority(auto x, auto y, auto z) NOEXCEPT
{
    // Normal form, optimized.
    ////return (x & y) ^ (x & z) ^ (y & z);
    return f::or_(f::and_(x, f::or_(y, z)), f::and_(y, z));
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
sigma(auto x) NOEXCEPT
{
    // Normal form, unmodified.
    constexpr auto s = SHA::word_bits;
    return f::xor_(f::xor_(f::ror<A, s>(x), f::ror<B, s>(x)), f::shr<C, s>(x));
}

TEMPLATE
template <unsigned int A, unsigned int B, unsigned int C>
INLINE constexpr auto CLASS::
Sigma(auto x) NOEXCEPT
{
    // Specialized for non-vector destructive ror.
    // "Because the ror instruction is destructive (that is, it overwrites the
    // operand), implementing the above as written would involve a number of
    // source register copy operations. If, however, the expressions are
    // rewritten as... Then the number of register copies can be minimized."
    // intel.com/content/dam/www/public/us/en/documents/white-papers/
    // sha-256-implementations-paper.pdf
    // AVX optimal (sha256)
    //
    // Examples:
    // S0(a) = (a >>> 2) ^ (a >>> 13) ^ (a >>> 22)
    // S1(e) = (e >>> 6) ^ (e >>> 11) ^ (e >>> 25)
    // S0(a) = ((((a >>>  9) ^ a) >>> 11) ^ a) >>> 2
    // S1(e) = ((((e >>> 14) ^ e) >>>  5) ^ e) >>> 6
    //
    // Generalization:
    // S (n) = (n >>> x) ^ (n >>> y) ^ (n >>> z)
    // S'(n) = ((((n >>> X) ^ n) >>> Y) ^ n) >>> Z
    // X = z - y
    // Y = y - x
    // Z = x
    // S'(n) = ((((n >>> (z-y)) ^ n) >>> (y-x)) ^ n) >>> x

    // This Sigma refactoring reduces normal form processing time by ~10%.
    // Normal form, optimized.
    //return (x >>> A) ^ (x >>> B) ^ (x >>> C)
    constexpr auto s = SHA::word_bits;
    return f::ror<A, s>(f::xor_(f::ror<B - A, s>(f::xor_(f::ror<C - B, s>(x), x)), x));
}

// Sigma dispatch
// ----------------------------------------------------------------------------
// protected

TEMPLATE
INLINE constexpr auto CLASS::
sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<7, 18, 3>(x);
    else
        return sigma<1, 8, 7>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return sigma<17, 19, 10>(x);
    else
        return sigma<19, 61, 6>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma0(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<2, 13, 22>(x);
    else
        return Sigma<28, 34, 39>(x);
}

TEMPLATE
INLINE constexpr auto CLASS::
Sigma1(auto x) NOEXCEPT
{
    if constexpr (SHA::strength == 256)
        return Sigma<6, 11, 25>(x);
    else
        return Sigma<14, 18, 41>(x);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
