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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SIGN_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_SIGN_IPP

// Based on:
// secg.org/sec1-v2.pdf (ECDSA signing and public key recovery)
// github.com/bitcoin/bips/blob/master/bip-0340.mediawiki

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// Keys and signing.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::linear(affine_t<uint64_t>& r, const scalar_t& g,
    const affine_t<uint64_t>& a, const scalar_t& k) NOEXCEPT
{
    jacobian_t<uint64_t> sum{};
    if (f::any(multiply(sum, scalars_t<uint64_t>{ g }, a,
        scalars_t<uint64_t>{ k })))
        multiply_complete(sum, g, a, k);

    if (f::any(sum.infinity))
        return false;

    to_affine(r, sum);
    return true;
}

constexpr bool algorithm::linear(affine_t<uint64_t>& r, const scalar_t& g,
    const affine_t<uint64_t>& a) NOEXCEPT
{
    uint64_t faults{};
    jacobian_t<uint64_t> sum{};
    sum.infinity = max_uint64;
    add_comb(sum, g, faults);
    declassify(faults);
    if (is_nonzero(faults))
    {
        LCOV_EXCL_START("A comb from infinity by a scalar below n is regular.")
        multiply_complete(sum, g, a, {});
        LCOV_EXCL_STOP()
    }

    add_complete(sum, sum, a);
    if (f::any(sum.infinity))
        return false;

    to_affine_power(r, sum);
    return true;
}

constexpr void algorithm::secret_multiply(affine_t<uint64_t>& r,
    const scalar_t& k, const scalar_t& m) NOEXCEPT
{
    scalar_t masked{};
    negate(masked, m);
    add(masked, masked, k);

    uint64_t faults{};
    jacobian_t<uint64_t> sum{};
    sum.infinity = max_uint64;
    add_comb(sum, masked, faults);
    add_comb(sum, m, faults);
    declassify(faults);
    if (is_nonzero(faults))
    {
        jacobian_t<uint64_t> blind{};
        multiply_complete(sum, masked, generator, {});
        multiply_complete(blind, m, generator, {});
        add_complete(sum, sum, blind);
    }

    to_affine_power(r, sum);
    wipe(masked);
}

// k * a = (k / m) * (m * a).
constexpr void algorithm::secret_multiply(affine_t<uint64_t>& r,
    const scalar_t& k, const affine_t<uint64_t>& a,
    const scalar_t& m) NOEXCEPT
{
    scalar_t quotient{};
    inverse_power(quotient, m);
    multiply(quotient, quotient, k);

    const scalars_t<uint64_t> blinds{ m };
    jacobian_t<uint64_t> blinded{};
    auto faults = multiply_windows<point_bits>(blinded, a, blinds);
    declassify(faults);
    if (f::any(faults))
        multiply_complete(blinded, {}, a, m);

    affine_t<uint64_t> point{};
    to_affine_power(point, blinded);

    const scalars_t<uint64_t> quotients{ quotient };
    jacobian_t<uint64_t> product{};
    faults = multiply_windows<point_bits>(product, point, quotients);
    declassify(faults);
    if (f::any(faults))
        multiply_complete(product, {}, point, quotient);

    to_affine_power(r, product);
    wipe(quotient);
}

// a^-1 = (a * m)^-1 * m.
constexpr void algorithm::secret_inverse(scalar_t& r, const scalar_t& a,
    const scalar_t& m) NOEXCEPT
{
    scalar_t product{};
    multiply(product, a, m);
    inverse_power(product, product);
    multiply(r, product, m);
    wipe(product);
}

constexpr void algorithm::to_affine_power(affine_t<uint64_t>& r,
    const jacobian_t<uint64_t>& a) NOEXCEPT
{
    field_t<uint64_t> inverse_z{};
    inverse_power(inverse_z, a.z);
    to_affine(r, a, inverse_z);
}

template <typename Container>
constexpr void algorithm::wipe(Container& secret) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        secret = {};
    }
    else
    {
        BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        volatile auto* data = secret.data();
        for (size_t index{}; index < secret.size(); ++index)
            data[index] = 0;
        BC_POP_WARNING()
        BC_POP_WARNING()

        classify(secret.data(), secret.size() *
            sizeof(typename Container::value_type));
    }
}

template <typename Value>
constexpr void algorithm::declassify(
    [[maybe_unused]] const Value& value) NOEXCEPT
{
    declassify(pointer_cast<const uint8_t>(&value), sizeof(value));
}

constexpr void algorithm::declassify(const uint8_t* data,
    size_t size) NOEXCEPT
{
    if (!std::is_constant_evaluated())
        system::declassify(data, size);
}

// R = kG, r = x(R) mod n, s = (z + r d) / k, with s made low. The recovery id
// is the parity of y(R), and 2 where x(R) is not below n.
constexpr bool algorithm::sign_ecdsa(scalar_t& r, scalar_t& s, uint8_t& id,
    const scalar_t& d, const scalar_t& z, const scalar_t& k,
    const scalar_t& m, const scalar_t& b) NOEXCEPT
{
    affine_t<uint64_t> point{};
    secret_multiply(point, k, m);

    bytes_t x{};
    to_bytes(x, point.x);
    const auto overflow = !from_bytes(r, x);
    auto odd = f::any(is_odd_element(point.y));

    scalar_t numerator{}, inverse_k{};
    multiply(numerator, r, d);
    add(numerator, numerator, z);
    secret_inverse(inverse_k, k, b);
    multiply(s, inverse_k, numerator);
    wipe(numerator);
    wipe(inverse_k);

    const auto high = is_high(s);
    scalar_t low{};
    negate(low, s);
    select(s, to_mask(high), low, s);
    odd = odd != high;

    const auto parity = to_int<uint8_t>(odd);
    const auto excess = to_int<uint8_t>(overflow);
    id = narrow_cast<uint8_t>(two * excess + parity);
    const auto zero = to_mask(is_zero_scalar(r)) | to_mask(is_zero_scalar(s));
    declassify(zero);
    return !to_bool(zero);
}

constexpr void algorithm::nonce_schnorr(bytes_t& r_x, scalar_t& k,
    const scalar_t& m) NOEXCEPT
{
    affine_t<uint64_t> point{};
    secret_multiply(point, k, m);

    scalar_t negated{};
    negate(negated, k);
    select(k, is_odd_element(point.y), negated, k);
    to_bytes(r_x, point.x);
}

// Q = (s / r)R - (z / r)G, where x(R) is r, or r + n where id includes 2.
constexpr bool algorithm::recover(affine_t<uint64_t>& r, const scalar_t& z,
    const scalar_t& sig_r, const scalar_t& sig_s, uint8_t id) NOEXCEPT
{
    if (is_zero_scalar(sig_r) || is_zero_scalar(sig_s))
        return false;

    field_t<uint64_t> x{};
    to_field(x, sig_r);
    if (get_right(id, one))
    {
        if (!is_less(sig_r, prime_minus_order))
            return false;

        add(x, x, order_field);
        normalize(x);
    }

    affine_t<uint64_t> point{};
    const auto odd = get_right(id, zero) ? max_uint64 : 0_u64;
    if (!f::any(lift(point, x, odd)))
        return false;

    scalar_t inverse_r{}, u1{}, u2{};
    inverse(inverse_r, sig_r);
    multiply(u1, inverse_r, z);
    negate(u1, u1);
    multiply(u2, inverse_r, sig_s);
    return linear(r, u1, point, u2);
}

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
