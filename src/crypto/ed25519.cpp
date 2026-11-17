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
#include <bitcoin/system/crypto/ed25519.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8032
// tweetnacl.cr.yp.to (point and scalar arithmetic)
// spec.torproject.org/rend-spec/keyblinding-scheme.html
// [Josefsson, Liusvaara]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Scalar digits are radix 2^8, and may be negative between carries.
constexpr auto digit_bits = 8_size;
constexpr auto digit_radix = bit_right<int64_t>(digit_bits);

// The nonce prefix of a blinded secret is derived from this and the prefix.
constexpr auto blind_string = to_array(
    "Derive temporary signing key hash input");

// ed25519
// ----------------------------------------------------------------------------

ed25519::expanded ed25519::expand(const key& seed) NOEXCEPT
{
    // The low half of the seed hash is the scalar, the high half the prefix.
    auto secret = sha512_hash(seed);
    auto& scalar = array_cast<uint8_t, key_size>(secret);
    scalar = clamp(scalar);
    return secret;
}

ed25519::key ed25519::public_key(const expanded& secret) NOEXCEPT
{
    const auto& scalar = array_cast<uint8_t, key_size>(secret);
    return encode(multiply(generator(), scalar));
}

ed25519::signature ed25519::sign(const expanded& secret, const key& point,
    const_byte_span message) NOEXCEPT
{
    const auto& scalar = array_cast<uint8_t, key_size>(secret);
    const auto& prefix = array_cast<uint8_t, key_size, key_size>(secret);

    // r = H(prefix || M) (mod l), R = rB.
    accumulator<sha512> nonce_hash{};
    nonce_hash.write(prefix);
    nonce_hash.write(message.size(), message.data());
    auto nonce = reduce(nonce_hash.flush());
    const auto commitment = encode(multiply(generator(), nonce));

    // k = H(R || A || M) (mod l).
    accumulator<sha512> challenge_hash{};
    challenge_hash.write(commitment);
    challenge_hash.write(point);
    challenge_hash.write(message.size(), message.data());
    const auto challenge = reduce(challenge_hash.flush());

    // S = r + ka (mod l).
    const auto response = multiply_add(challenge, scalar, nonce);
    wipe(nonce);
    return splice(commitment, response);
}

bool ed25519::verify(const signature& value, const key& point,
    const_byte_span message) NOEXCEPT
{
    const auto& commitment = array_cast<uint8_t, key_size>(value);
    const auto& response = array_cast<uint8_t, key_size, key_size>(value);

    extended a{};
    if (!is_canonical(response) || !decode(a, point))
        return false;

    // k = H(R || A || M) (mod l).
    accumulator<sha512> challenge_hash{};
    challenge_hash.write(commitment);
    challenge_hash.write(point);
    challenge_hash.write(message.size(), message.data());
    const auto challenge = reduce(challenge_hash.flush());

    // The encoding of SB - kA must be R (cofactorless).
    opposite(a);
    auto check = multiply(generator(), response);
    add(check, multiply(a, challenge));
    return encode(check) == commitment;
}

bool ed25519::blind(key& out, const key& point, const key& factor) NOEXCEPT
{
    extended value{};
    if (!decode(value, point))
        return false;

    out = encode(multiply(value, reduce(clamp(factor))));
    return true;
}

ed25519::expanded ed25519::blind(const expanded& secret,
    const key& factor) NOEXCEPT
{
    const auto& scalar = array_cast<uint8_t, key_size>(secret);
    const auto& prefix = array_cast<uint8_t, key_size, key_size>(secret);

    accumulator<sha512> prefix_hash{};
    prefix_hash.write(blind_string);
    prefix_hash.write(prefix);
    const auto hash = prefix_hash.flush();

    const auto blinded = multiply_add(scalar, reduce(clamp(factor)), {});
    return splice(blinded, array_cast<uint8_t, key_size>(hash));
}

// field
// ----------------------------------------------------------------------------

// The low bit of the fully reduced element (the sign of x).
bool ed25519::parity(const field& a) NOEXCEPT
{
    element bytes{};
    pack(bytes, a);
    return get_right(bytes.front());
}

// Equality of fully reduced elements (not constant time).
bool ed25519::same(const field& a, const field& b) NOEXCEPT
{
    element left{};
    element right{};
    pack(left, a);
    pack(right, b);
    return left == right;
}

// The power a^((p - 5) / 8), from which a square root is derived.
constexpr void ed25519::root_power(field& out, const field& a) NOEXCEPT
{
    auto c = a;

    for (size_t index{ 251 }; index > zero; --index)
    {
        const auto exponent = sub1(index);
        square(c, c);

        if (exponent != 1)
            multiply(c, c, a);
    }

    out = c;
}

// points
// ----------------------------------------------------------------------------

constexpr ed25519::extended ed25519::identity() NOEXCEPT
{
    return { field{}, field{ 1 }, field{ 1 }, field{} };
}

constexpr ed25519::extended ed25519::generator() NOEXCEPT
{
    extended out{ base_x, base_y, field{ 1 }, field{} };
    multiply(out[3], base_x, base_y);
    return out;
}

// The opposite point (-x, y).
constexpr void ed25519::opposite(extended& p) NOEXCEPT
{
    subtract(p[0], {}, p[0]);
    subtract(p[3], {}, p[3]);
}

// Unified addition (also doubling), p may be q. All inputs are read before
// any output is written.
constexpr void ed25519::add(extended& p, const extended& q) NOEXCEPT
{
    field a{};
    field b{};
    field c{};
    field d{};
    field t{};
    subtract(a, p[1], p[0]);
    subtract(t, q[1], q[0]);
    multiply(a, a, t);
    add(b, p[0], p[1]);
    add(t, q[0], q[1]);
    multiply(b, b, t);
    multiply(c, p[3], q[3]);
    multiply(c, c, curve_d2);
    multiply(d, p[2], q[2]);
    add(d, d, d);

    field e{};
    field f{};
    field g{};
    field h{};
    subtract(e, b, a);
    subtract(f, d, c);
    add(g, d, c);
    add(h, b, a);
    multiply(p[0], e, f);
    multiply(p[1], h, g);
    multiply(p[2], g, f);
    multiply(p[3], e, h);
}

// Constant time conditional swap (bit is zero or one).
constexpr void ed25519::select(extended& p, extended& q, int64_t bit) NOEXCEPT
{
    for (size_t index{}; index < coordinates; ++index)
        select(p[index], q[index], bit);
}

// Constant time ladder over all 256 bits of the scalar.
constexpr ed25519::extended ed25519::multiply(const extended& point,
    const key& scalar) NOEXCEPT
{
    auto p = identity();
    auto q = point;

    for (size_t index{ 256 }; index > zero; --index)
    {
        const auto bit = sub1(index);
        const auto byte = scalar[shift_right(bit, 3)];
        const auto offset = bit_and(bit, 7_size);
        const auto swap = to_int<int64_t>(get_right(byte, offset));

        select(p, q, swap);
        add(q, p);
        add(p, p);
        select(p, q, swap);
    }

    return p;
}

// The y coordinate with the sign of x in its high bit (rfc8032 5.1.2).
ed25519::key ed25519::encode(const extended& p) NOEXCEPT
{
    field inverse{};
    field x{};
    field y{};
    invert(inverse, p[2]);
    multiply(x, p[0], inverse);
    multiply(y, p[1], inverse);

    key out{};
    pack(out, y);
    set_right_into(out.back(), 7, parity(x));
    return out;
}

// Decode a canonical encoding of a point on the curve (rfc8032 5.1.3).
bool ed25519::decode(extended& out, const key& point) NOEXCEPT
{
    const auto sign = get_right(point.back(), 7);
    auto encoded = point;
    set_right_into(encoded.back(), 7, false);

    // The y coordinate must be less than p.
    auto& [x, y, z, t] = out;
    element canonical{};
    unpack(y, encoded);
    pack(canonical, y);
    if (canonical != encoded)
        return false;

    // x^2 = (y^2 - 1) / (dy^2 + 1), as u / v.
    field u{};
    field v{};
    z = { 1 };
    square(u, y);
    multiply(v, u, curve_d);
    subtract(u, u, z);
    add(v, v, z);

    // x = uv^3 (uv^7)^((p - 5) / 8).
    field v2{};
    field v4{};
    field v6{};
    square(v2, v);
    square(v4, v2);
    multiply(v6, v4, v2);
    multiply(x, v6, u);
    multiply(x, x, v);
    root_power(x, x);
    multiply(x, x, u);
    multiply(x, x, v);
    multiply(x, x, v);
    multiply(x, x, v);

    // If vx^2 is not u then x * sqrt(-1) is the root, if any.
    field check{};
    square(check, x);
    multiply(check, check, v);
    if (!same(check, u))
        multiply(x, x, root);

    square(check, x);
    multiply(check, check, v);
    if (!same(check, u))
        return false;

    // There is no negative zero.
    if (sign && same(x, {}))
        return false;

    if (parity(x) != sign)
        subtract(x, {}, x);

    multiply(t, x, y);
    return true;
}

// scalars
// ----------------------------------------------------------------------------

// Clear the low three bits and the high bit, set the next highest bit.
constexpr ed25519::key ed25519::clamp(const key& scalar) NOEXCEPT
{
    auto clamped = scalar;
    mask_right_into(clamped.front(), 3);
    mask_left_into(clamped.back(), 1);
    set_right_into(clamped.back(), 6);
    return clamped;
}

// The scalar is less than l (not constant time).
constexpr bool ed25519::is_canonical(const key& scalar) NOEXCEPT
{
    for (auto index = key_size; index > zero; --index)
    {
        const auto digit = sub1(index);
        if (scalar[digit] != order[digit])
            return scalar[digit] < order[digit];
    }

    return false;
}

// Reduce modulo l = 2^252 + c. Each digit above 2^256 folds into the digits
// below it, as 2^256 = -16c (mod l). The sixteen digits of c are followed by
// four digits of carry. The mask of a signed digit is its floored remainder.
constexpr ed25519::key ed25519::reduce(product& x) NOEXCEPT
{
    constexpr auto high_bits = 4_size;
    constexpr auto high = bit_right<int64_t>(high_bits);
    constexpr auto span = 20_size;
    constexpr auto mask = sub1(digit_radix);
    const auto half = to_half(digit_radix);

    for (auto index = sub1(digits); index >= key_size; --index)
    {
        int64_t carry{};
        const auto first = index - key_size;
        auto digit = first;

        for (; digit < first + span; ++digit)
        {
            x[digit] += carry - high * x[index] * order[digit - first];
            carry = shift_right(x[digit] + half, digit_bits);
            x[digit] -= carry * digit_radix;
        }

        x[digit] += carry;
        x[index] = 0;
    }

    // Subtract the multiple of l carried in the high nibble of digit 31.
    int64_t carry{};
    for (size_t digit{}; digit < key_size; ++digit)
    {
        const auto top = shift_right(x[sub1(key_size)], high_bits);
        x[digit] += carry - top * order[digit];
        carry = shift_right(x[digit], digit_bits);
        x[digit] = bit_and(x[digit], mask);
    }

    for (size_t digit{}; digit < key_size; ++digit)
        x[digit] -= carry * order[digit];

    key out{};
    for (size_t digit{}; digit < key_size; ++digit)
    {
        x[add1(digit)] += shift_right(x[digit], digit_bits);
        out[digit] = possible_narrow_and_sign_cast<uint8_t>(
            bit_and(x[digit], mask));
    }

    return out;
}

constexpr ed25519::key ed25519::reduce(const key& scalar) NOEXCEPT
{
    product x{};
    for (size_t digit{}; digit < key_size; ++digit)
        x[digit] = scalar[digit];

    return reduce(x);
}

constexpr ed25519::key ed25519::reduce(const expanded& wide) NOEXCEPT
{
    product x{};
    for (size_t digit{}; digit < digits; ++digit)
        x[digit] = wide[digit];

    return reduce(x);
}

// (ab + c) (mod l).
constexpr ed25519::key ed25519::multiply_add(const key& a, const key& b,
    const key& c) NOEXCEPT
{
    product x{};
    for (size_t digit{}; digit < key_size; ++digit)
        x[digit] = c[digit];

    for (size_t i{}; i < key_size; ++i)
    {
        const int64_t left = a[i];
        for (size_t j{}; j < key_size; ++j)
            x[i + j] += left * b[j];
    }

    return reduce(x);
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
