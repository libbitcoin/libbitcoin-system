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
#include <bitcoin/system/crypto/x25519.hpp>

#include <bitcoin/system/crypto/maybe_random.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc7748
// tweetnacl.cr.yp.to (ladder)
// [Bernstein, Josefsson, Liusvaara]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// x25519
// ----------------------------------------------------------------------------

bool x25519::multiply(key& out, const key& scalar, const key& point) NOEXCEPT
{
    // The Montgomery ladder constant (a - 2) / 4 = 121665 in radix 2^16.
    constexpr field a24{ 0xdb41, 1 };

    // Clamp scalar: clear low three bits, clear high bit, set next highest bit
    // (rfc7748 decodeScalar25519).
    auto clamped = scalar;
    mask_right_into(clamped[0], 3);
    mask_left_into(clamped[31], 1);
    set_right_into(clamped[31], 6);

    field x{};
    unpack(x, point);

    field a{};
    field b = x;
    field c{};
    field d{};
    field e{};
    field f{};
    a[zero] = 1;
    d[zero] = 1;

    // Montgomery ladder over the scalar bits, from 254 down to 0.
    for (size_t index{ 255 }; index > zero; --index)
    {
        const auto bit = sub1(index);
        const auto byte = clamped[shift_right(bit, 3)];
        const auto swap = to_int<int64_t>(get_right(byte, bit_and(bit, 7_size)));

        select(a, b, swap);
        select(c, d, swap);
        add(e, a, c);
        subtract(a, a, c);
        add(c, b, d);
        subtract(b, b, d);
        square(d, e);
        square(f, a);
        multiply(a, c, a);
        multiply(c, b, e);
        add(e, a, c);
        subtract(a, a, c);
        square(b, a);
        subtract(c, d, f);
        multiply(a, c, a24);
        add(a, a, d);
        multiply(c, c, a);
        multiply(a, d, f);
        multiply(d, b, x);
        square(b, e);
        select(a, b, swap);
        select(c, d, swap);
    }

    invert(c, c);
    multiply(a, a, c);
    pack(out, a);
    wipe(clamped);
    wipe(a);
    wipe(b);
    wipe(c);
    wipe(d);
    wipe(e);
    wipe(f);

    // All zero result is low order point (all bytes read).
    uint8_t bits{};
    for (const auto byte: out)
        bits |= byte;

    return is_nonzero(bits);
}

bool x25519::multiply(key& out, const key& scalar) NOEXCEPT
{
    constexpr key base{ 9 };
    return multiply(out, scalar, base);
}

// Sacrifices about 1.1 bits of entropy, out of 251 (acceptable).
// About one third of Z85 key encodings contain hash sign (config comment).
static bool allowed(const x25519::key& key, bool sanitize) NOEXCEPT
{
    if (!sanitize)
        return true;

    std::string encoded{};
    return encode_base85(encoded, key) && !contains(encoded, '#');
}

void x25519::generate(key& private_key, key& public_key, bool sanitize) NOEXCEPT
{
    // A low order result is astronomically improbable.
    do
    {
        maybe_random::fill(private_key);
    }
    while (!multiply(public_key, private_key) ||
        !allowed(private_key, sanitize) || !allowed(public_key, sanitize));
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
