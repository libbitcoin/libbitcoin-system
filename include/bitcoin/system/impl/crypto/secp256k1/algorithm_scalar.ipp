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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SCALAR_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SCALAR_IPP

namespace libbitcoin {
namespace system {
namespace secp256k1 {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Scalar internals.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::is_overflow(const scalar_t& a) NOEXCEPT
{
    for (auto limb = a.size(); is_nonzero(limb--);)
        if (a[limb] != order[limb])
            return a[limb] > order[limb];

    return true;
}

// Adds 2^256 - n (modulo 2^256), which subtracts n from a value not below n.
constexpr void algorithm::reduce(scalar_t& r, bool overflow) NOEXCEPT
{
    if (!overflow)
        return;

    auto carry = false;
    for (size_t limb{}; limb < r.size(); ++limb)
        carry = add_carry(r[limb], r[limb], order_complement[limb], carry);
}

// Folds limbs above 2^256 by 2^256 = 2^256 - n (mod n), from 512 to 385 to
// 258 to 257 bits, leaving at most one subtraction of n.
constexpr void algorithm::reduce(scalar_t& r, const wide_t& value) NOEXCEPT
{
    constexpr size_t folds = 3;
    constexpr size_t terms = 3;
    constexpr auto half = to_half(array_count<wide_t>);

    auto limbs = value;
    for (size_t fold{}; fold < folds; ++fold)
    {
        wide_t next{ limbs[0], limbs[1], limbs[2], limbs[3] };
        for (size_t high{}; high < half; ++high)
            for (size_t term{}; term < terms; ++term)
                multiply_add(next, high + term, limbs[half + high],
                    order_complement[term]);

        limbs = next;
    }

    r = { limbs[0], limbs[1], limbs[2], limbs[3] };
    reduce(r, is_nonzero(limbs[half]) || is_overflow(r));
}

constexpr void algorithm::multiply_add(wide_t& r, size_t position,
    uint64_t a, uint64_t b) NOEXCEPT
{
    uint64_t high{}, low{};
    mul_wide(high, low, a, b);
    auto carry = add_carry(r[position], r[position], low, false);
    carry = add_carry(r[add1(position)], r[add1(position)], high, carry);
    for (auto limb = position + two; carry && limb < r.size(); ++limb)
        carry = add_carry(r[limb], r[limb], uint64_t{}, carry);
}

constexpr void algorithm::product(wide_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    r = {};
    for (size_t left{}; left < a.size(); ++left)
        for (size_t right{}; right < b.size(); ++right)
            multiply_add(r, left + right, a[left], b[right]);
}

// r = round(a * b / 2^384).
constexpr void algorithm::multiply_shift(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    wide_t value{};
    product(value, a, b);
    const auto round = value[5] >> sub1(bits<uint64_t>);
    auto carry = add_carry(r[0], value[6], round, false);
    carry = add_carry(r[1], value[7], uint64_t{}, carry);
    r[2] = to_int<uint64_t>(carry);
    r[3] = 0;
}

// Scalar arithmetic.
// ----------------------------------------------------------------------------
// protected

constexpr void algorithm::add(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    auto carry = false;
    for (size_t limb{}; limb < r.size(); ++limb)
        carry = add_carry(r[limb], a[limb], b[limb], carry);

    reduce(r, carry || is_overflow(r));
}

// n - a as n + ~a + 1, for nonzero a less than n.
constexpr void algorithm::negate(scalar_t& r, const scalar_t& a) NOEXCEPT
{
    if (is_zero(a))
    {
        r = {};
        return;
    }

    auto carry = true;
    for (size_t limb{}; limb < r.size(); ++limb)
        carry = add_carry(r[limb], order[limb], ~a[limb], carry);
}

constexpr void algorithm::multiply(scalar_t& r, const scalar_t& a,
    const scalar_t& b) NOEXCEPT
{
    wide_t value{};
    product(value, a, b);
    reduce(r, value);
}

// a^(n - 2) by fixed four bit windows.
constexpr void algorithm::inverse(scalar_t& r, const scalar_t& a) NOEXCEPT
{
    constexpr size_t window = 4;
    constexpr auto mask = unmask_right<uint64_t>(window);
    constexpr auto per_limb = bits<uint64_t> / window;
    constexpr scalar_t exponent
    {
        system::subtract<uint64_t>(order[0], two), order[1], order[2], order[3]
    };

    std_array<scalar_t, power2(window)> powers{};
    powers[0] = { 1 };
    for (auto power = one; power < powers.size(); ++power)
        multiply(powers[power], powers[sub1(power)], a);

    scalar_t result{ 1 };
    for (auto digit = exponent.size() * per_limb; is_nonzero(digit--);)
    {
        for (size_t square{}; square < window; ++square)
            multiply(result, result, result);

        const auto limb = exponent[digit / per_limb];
        const auto shift = (digit % per_limb) * window;
        const auto index = possible_narrow_cast<size_t>(
            bit_and(shift_right(limb, shift), mask));
        if (is_nonzero(index))
            multiply(result, result, powers[index]);
    }

    r = result;
}

constexpr void algorithm::split(scalar_t& k1, scalar_t& k2,
    const scalar_t& k) NOEXCEPT
{
    scalar_t c1{}, c2{};
    multiply_shift(c1, k, g1);
    multiply_shift(c2, k, g2);
    multiply(c1, c1, minus_b1);
    multiply(c2, c2, minus_b2);
    add(k2, c1, c2);
    multiply(c1, k2, lambda);
    negate(c1, c1);
    add(k1, c1, k);
}

// Each digit is the low Bits + 1 bits less 2^Bits, which is odd and nonzero,
// and the remainder (value - digit) / 2^Bits is again odd.
template <size_t Bits, size_t Count>
constexpr void algorithm::recode(digits_t<Count>& digits,
    const scalar_t& value) NOEXCEPT
{
    static_assert(is_nonzero(Bits) && Bits < sub1(bits<int16_t>));
    static_assert(is_nonzero(Count));
    static_assert(Bits * Count <= array_count<scalar_t> * bits<uint64_t>);

    constexpr auto window = unmask_right<uint64_t>(add1(Bits));
    constexpr auto offset = power2<int64_t>(Bits);
    constexpr auto rest = bits<uint64_t> - Bits;

    auto remainder = value;
    for (size_t digit{}; digit < sub1(Count); ++digit)
    {
        const auto low = sign_cast<int64_t>(remainder[0] & window);
        digits[digit] = narrow_cast<int16_t>(low - offset);

        remainder[0] = set_right((remainder[0] >> Bits) | (remainder[1] << rest));
        remainder[1] = (remainder[1] >> Bits) | (remainder[2] << rest);
        remainder[2] = (remainder[2] >> Bits) | (remainder[3] << rest);
        remainder[3] = (remainder[3] >> Bits);
    }

    digits[sub1(Count)] = narrow_sign_cast<int16_t>(remainder[0]);
}

// Scalar predicates.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::is_zero(const scalar_t& a) NOEXCEPT
{
    return bc::is_zero(a[0] | a[1] | a[2] | a[3]);
}

constexpr bool algorithm::is_high(const scalar_t& a) NOEXCEPT
{
    for (auto limb = a.size(); is_nonzero(limb--);)
        if (a[limb] != half_order[limb])
            return a[limb] > half_order[limb];

    return false;
}

// Scalar encoding.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::from_bytes(scalar_t& r,
    const bytes_t& bytes) NOEXCEPT
{
    r = {};
    constexpr auto size = sizeof(uint64_t);
    for (size_t byte{}; byte < array_count<bytes_t>; ++byte)
    {
        auto& limb = r[sub1(r.size()) - (byte / size)];
        limb = bit_or<uint64_t>(shift_left(limb, byte_bits), bytes[byte]);
    }

    const auto overflow = is_overflow(r);
    reduce(r, overflow);
    return !overflow;
}

constexpr void algorithm::to_bytes(bytes_t& out, const scalar_t& a) NOEXCEPT
{
    constexpr auto size = sizeof(uint64_t);
    for (size_t byte{}; byte < array_count<bytes_t>; ++byte)
    {
        const auto limb = a[sub1(a.size()) - (byte / size)];
        const auto shift = to_bits(sub1(size) - (byte % size));
        out[byte] = narrow_cast<uint8_t>(shift_right(limb, shift));
    }
}

BC_POP_WARNING()

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
