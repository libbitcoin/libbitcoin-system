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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_VERIFY_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_VERIFY_IPP

// Based on:
// secg.org/sec1-v2.pdf (ECDSA verification)
// github.com/bitcoin/bips/blob/master/bip-0340.mediawiki

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// Verification.
// ----------------------------------------------------------------------------
// protected

constexpr bool algorithm::from_bytes(affine_t<uint64_t>& r,
    const ec_compressed& key) NOEXCEPT
{
    const auto sign = key.front();
    if (sign != ec_even_sign && sign != ec_odd_sign)
        return false;

    field_t<uint64_t> x{};
    if (!from_bytes<one>(x, key))
        return false;

    const auto odd = sign == ec_odd_sign ? max_uint64 : 0_u64;
    return f::any(lift(r, x, odd));
}

constexpr bool algorithm::from_bytes(affine_t<uint64_t>& r,
    const ec_uncompressed& key) NOEXCEPT
{
    constexpr auto size = array_count<bytes_t>;

    const auto sign = key.front();
    const auto hybrid = sign == ec_hybrid_even_sign ||
        sign == ec_hybrid_odd_sign;

    if (sign != ec_uncompressed_sign && !hybrid)
        return false;

    field_t<uint64_t> x{}, y{};
    if (!from_bytes<one>(x, key) || !from_bytes<add1(size)>(y, key))
        return false;

    if (hybrid && (f::any(is_odd(y)) != (sign == ec_hybrid_odd_sign)))
        return false;

    r = { x, y };
    return f::any(is_on_curve(r));
}

// R = (z / s)G + (r / s)Q, valid if x(R) mod n is r. Since n < p, x(R) mod n
// is r if x(R) is r, or is r + n where r + n < p, compared as X = x * Z^2.
constexpr bool algorithm::verify_ecdsa(const affine_t<uint64_t>& point,
    const bytes_t& hash, const bytes_t& r, const bytes_t& s) NOEXCEPT
{
    scalar_t scalar_r{}, scalar_s{}, scalar_z{};
    if (!from_bytes(scalar_r, r) || !from_bytes(scalar_s, s) ||
        is_zero(scalar_r) || is_zero(scalar_s))
        return false;

    /* bool */ from_bytes(scalar_z, hash);

    scalar_t w{}, u1{}, u2{};
    inverse(w, scalar_s);
    multiply(u1, scalar_z, w);
    multiply(u2, scalar_r, w);

    jacobian_t<uint64_t> sum{};
    if (f::any(multiply(sum, scalars_t<uint64_t>{ u1 }, point,
        scalars_t<uint64_t>{ u2 })))
        multiply_complete(sum, u1, point, u2);

    if (f::any(sum.infinity))
        return false;

    field_t<uint64_t> x{}, zz{}, expected{};
    normalize(sum.x);
    square(zz, sum.z);
    /* bool */ from_bytes(x, r);
    multiply(expected, x, zz);
    normalize(expected);
    if (f::any(equal(expected, sum.x)))
        return true;

    if (!is_less(scalar_r, prime_minus_order))
        return false;

    add(x, x, order_field);
    carry(x);
    multiply(expected, x, zz);
    normalize(expected);
    return f::any(equal(expected, sum.x));
}

inline hash_digest algorithm::challenge(const bytes_t& r, const bytes_t& key,
    const data_slice& message) NOEXCEPT
{
    const auto tag = sha256_hash(std::string{ "BIP0340/challenge" });
    accumulator<sha256> context{};
    context.write(tag);
    context.write(tag);
    context.write(r);
    context.write(key);
    context.write(message.size(), message.data());
    return context.flush();
}

// R = sG - eP, valid if R is finite with even y and x(R) is r.
constexpr bool algorithm::verify_schnorr(const bytes_t& key,
    const hash_digest& digest, const bytes_t& r, const bytes_t& s) NOEXCEPT
{
    field_t<uint64_t> key_x{}, r_x{};
    scalar_t scalar_s{}, scalar_e{};
    if (!from_bytes(key_x, key) || !from_bytes(r_x, r) ||
        !from_bytes(scalar_s, s))
        return false;

    affine_t<uint64_t> point{};
    if (!f::any(lift(point, key_x, 0_u64)))
        return false;

    /* bool */ from_bytes(scalar_e, digest);
    negate(scalar_e, scalar_e);

    jacobian_t<uint64_t> sum{};
    if (f::any(multiply(sum, scalars_t<uint64_t>{ scalar_s }, point,
        scalars_t<uint64_t>{ scalar_e })))
        multiply_complete(sum, scalar_s, point, scalar_e);

    if (f::any(sum.infinity))
        return false;

    affine_t<uint64_t> result{};
    to_affine(result, sum);
    return !f::any(is_odd(result.y)) && f::any(equal(result.x, r_x));
}

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
