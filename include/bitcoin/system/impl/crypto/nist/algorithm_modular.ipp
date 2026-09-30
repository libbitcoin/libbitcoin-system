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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_MODULAR_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_MODULAR_IPP

// Modular arithmetic
// ============================================================================
// Montgomery multiplication (coarsely integrated operand scanning), with
// R = 2^(64 * words). Moduli are odd and less than R.

namespace libbitcoin {
namespace system {
namespace nist {

// Constants.
// ----------------------------------------------------------------------------

TEMPLATE
consteval typename CLASS::modulus_t CLASS::
make_modulus(const limbs_t& value) NOEXCEPT
{
    modulus_t out{};
    out.value = value;

    // -m^-1 mod 2^64, by Newton iteration (each doubles the correct bits).
    uint64_t inverse{ 1 };
    for (size_t step{}; step < 6_size; ++step)
        inverse *= 2_u64 - value.front() * inverse;

    out.inverse = twos_complement(inverse);

    // R mod m and R^2 mod m, by modular doubling.
    limbs_t power{ 1 };
    for (size_t bit{}; bit < words * bits<uint64_t>; ++bit)
        power = add(power, power, out);

    out.one = power;
    for (size_t bit{}; bit < words * bits<uint64_t>; ++bit)
        power = add(power, power, out);

    out.squared = power;

    // m - 2 (inversion) and (m + 1) / 4 (square root, for m = 3 mod 4).
    difference(out.inverse_exponent, value, limbs_t{ 2 });

    limbs_t next{};
    sum(next, value, limbs_t{ 1 });
    for (size_t word{}; word < words; ++word)
    {
        const auto above = add1(word) < words ? next[add1(word)] : 0_u64;
        const auto low = shift_right(next[word], 2);
        const auto high = shift_left(above, 62);
        out.root_exponent[word] = bit_or(low, high);
    }

    return out;
}

TEMPLATE
constexpr const typename CLASS::modulus_t& CLASS::
field() NOEXCEPT
{
    return field_modulus;
}

TEMPLATE
constexpr const typename CLASS::modulus_t& CLASS::
scalar() NOEXCEPT
{
    return scalar_modulus;
}

// Arithmetic.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
add(const limbs_t& a, const limbs_t& b, const modulus_t& m) NOEXCEPT
{
    limbs_t total{}, reduced{}, out{};
    const auto carry = sum(total, a, b);
    const auto borrow = difference(reduced, total, m.value);

    // The sum is reduced if it overflowed or is not less than the modulus.
    const auto overflow = bit_or(carry, bit_xor(borrow, bit_lo<uint64_t>));
    select(out, twos_complement(overflow), reduced, total);
    return out;
}

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
subtract(const limbs_t& a, const limbs_t& b, const modulus_t& m) NOEXCEPT
{
    limbs_t out{}, addend{};
    const auto borrow = difference(out, a, b);

    // The modulus is added back if the difference underflowed.
    select(addend, twos_complement(borrow), m.value, limbs_t{});
    sum(out, out, addend);
    return out;
}

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
multiply(const limbs_t& a, const limbs_t& b, const modulus_t& m) NOEXCEPT
{
    std_array<uint64_t, words + two> t{};
    for (size_t i{}; i < words; ++i)
    {
        // t += a * b[i].
        uint64_t carry{};
        for (size_t j{}; j < words; ++j)
            t[j] = multiply_add(t[j], a[j], b[i], carry);

        t[add1(words)] = add_carry(t[words], t[words], carry, 0_u64);

        // t = (t + u * m) / 2^64, with u chosen to clear the low word.
        const auto u = t.front() * m.inverse;
        carry = 0_u64;
        multiply_add(t.front(), u, m.value.front(), carry);
        for (auto j = one; j < words; ++j)
            t[sub1(j)] = multiply_add(t[j], u, m.value[j], carry);

        const auto top = add_carry(t[sub1(words)], t[words], carry, 0_u64);
        t[words] = t[add1(words)] + top;
        t[add1(words)] = 0_u64;
    }

    // The result is below 2m, reduced if at least m.
    limbs_t value{}, reduced{}, out{};
    std::copy_n(t.cbegin(), words, value.begin());
    const auto borrow = difference(reduced, value, m.value);
    const auto overflow = bit_or(t[words], bit_xor(borrow, bit_lo<uint64_t>));
    select(out, twos_complement(overflow), reduced, value);
    return out;
}

// Montgomery power by a public exponent.
TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
power(const limbs_t& a, const limbs_t& exponent, const modulus_t& m) NOEXCEPT
{
    auto out = m.one;
    for (auto bit = words * bits<uint64_t>; !bc::is_zero(bit); --bit)
    {
        out = multiply(out, out, m);
        const auto index = sub1(bit);
        if (get_right(exponent[index / bits<uint64_t>], index % bits<uint64_t>))
            out = multiply(out, a, m);
    }

    return out;
}

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
inverse(const limbs_t& a, const modulus_t& m) NOEXCEPT
{
    return power(a, m.inverse_exponent, m);
}

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
to_montgomery(const limbs_t& a, const modulus_t& m) NOEXCEPT
{
    return multiply(a, m.squared, m);
}

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
from_montgomery(const limbs_t& a, const modulus_t& m) NOEXCEPT
{
    return multiply(a, limbs_t{ 1 }, m);
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
