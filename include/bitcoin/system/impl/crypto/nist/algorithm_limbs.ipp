/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_LIMBS_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_LIMBS_IPP

// Limbs
// ============================================================================
// Carries and masks are computed without branches, so that the time of an
// operation does not depend on its operands.

namespace libbitcoin {
namespace system {
namespace nist {

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
to_limbs(const bytes_t& bytes) NOEXCEPT
{
    limbs_t limbs{};
    for (size_t word{}; word < words; ++word)
    {
        for (size_t byte{}; byte < sizeof(uint64_t); ++byte)
        {
            const auto index = size - add1(word * sizeof(uint64_t) + byte);
            const auto value = shift_left<uint64_t>(bytes[index],
                byte * byte_bits);
            limbs[word] = bit_or(limbs[word], value);
        }
    }

    return limbs;
}

TEMPLATE
constexpr typename CLASS::bytes_t CLASS::
to_bytes(const limbs_t& limbs) NOEXCEPT
{
    bytes_t bytes{};
    for (size_t word{}; word < words; ++word)
    {
        for (size_t byte{}; byte < sizeof(uint64_t); ++byte)
        {
            const auto index = size - add1(word * sizeof(uint64_t) + byte);
            const auto value = shift_right(limbs[word], byte * byte_bits);
            bytes[index] = narrow_cast<uint8_t>(value);
        }
    }

    return bytes;
}

// The carry is the high bit of the majority of a, b and not out.
TEMPLATE
INLINE constexpr uint64_t CLASS::
add_carry(uint64_t& out, uint64_t a, uint64_t b, uint64_t carry) NOEXCEPT
{
    out = a + b + carry;
    const auto both = bit_and(a, b);
    const auto either = bit_and(bit_or(a, b), bit_not(out));
    return shift_right(bit_or(both, either), sub1(bits<uint64_t>));
}

// The borrow is the high bit of the majority of not a, b and out.
TEMPLATE
INLINE constexpr uint64_t CLASS::
subtract_borrow(uint64_t& out, uint64_t a, uint64_t b,
    uint64_t borrow) NOEXCEPT
{
    out = a - b - borrow;
    const auto under = bit_and(bit_not(a), b);
    const auto equal = bit_and(bit_not(bit_xor(a, b)), out);
    return shift_right(bit_or(under, equal), sub1(bits<uint64_t>));
}

// t + a * b + carry, low word returned and high word to carry (no overflow).
TEMPLATE
INLINE constexpr uint64_t CLASS::
multiply_add(uint64_t t, uint64_t a, uint64_t b, uint64_t& carry) NOEXCEPT
{
    uint64_t hi{}, lo{};
    mul_wide(hi, lo, a, b);
    lo += t;
    hi += to_int<uint64_t>(lo < t);
    lo += carry;
    hi += to_int<uint64_t>(lo < carry);
    carry = hi;
    return lo;
}

// out = mask ? a : b, with mask all ones or all zeros.
TEMPLATE
INLINE constexpr void CLASS::
select(limbs_t& out, uint64_t mask, const limbs_t& a, const limbs_t& b) NOEXCEPT
{
    for (size_t word{}; word < words; ++word)
    {
        const auto from_a = bit_and(a[word], mask);
        const auto from_b = bit_and(b[word], bit_not(mask));
        out[word] = bit_or(from_a, from_b);
    }
}

// All ones if zero, otherwise all zeros.
TEMPLATE
INLINE constexpr uint64_t CLASS::
zero_mask(const limbs_t& a) NOEXCEPT
{
    uint64_t any{};
    for (const auto word: a)
        any = bit_or(any, word);

    // The high bit of (any | -any) is set if and only if any is nonzero.
    const auto both = bit_or(any, twos_complement(any));
    const auto nonzero = shift_right(both, sub1(bits<uint64_t>));
    return twos_complement(bit_xor(nonzero, bit_lo<uint64_t>));
}

TEMPLATE
constexpr bool CLASS::
is_zero(const limbs_t& a) NOEXCEPT
{
    return !bc::is_zero(zero_mask(a));
}

TEMPLATE
constexpr bool CLASS::
is_less(const limbs_t& a, const limbs_t& b) NOEXCEPT
{
    limbs_t unused{};
    return !bc::is_zero(difference(unused, a, b));
}

// out = a + b, wrapping, carry returned.
TEMPLATE
constexpr uint64_t CLASS::
sum(limbs_t& out, const limbs_t& a, const limbs_t& b) NOEXCEPT
{
    uint64_t carry{};
    for (size_t word{}; word < words; ++word)
        carry = add_carry(out[word], a[word], b[word], carry);

    return carry;
}

// out = a - b, wrapping, borrow returned.
TEMPLATE
constexpr uint64_t CLASS::
difference(limbs_t& out, const limbs_t& a, const limbs_t& b) NOEXCEPT
{
    uint64_t borrow{};
    for (size_t word{}; word < words; ++word)
        borrow = subtract_borrow(out[word], a[word], b[word], borrow);

    return borrow;
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
