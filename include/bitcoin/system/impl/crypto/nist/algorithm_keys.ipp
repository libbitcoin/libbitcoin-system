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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_KEYS_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_KEYS_IPP

// Keys
// ============================================================================

namespace libbitcoin {
namespace system {
namespace nist {

TEMPLATE
typename CLASS::secret_t CLASS::
generate() NOEXCEPT
{
    secret_t secret{};
    do
    {
        maybe_random::fill(secret);
    }
    while (!is_scalar(to_limbs(secret)));
    return secret;
}

TEMPLATE
bool CLASS::
public_key(point_t& out, const secret_t& secret) NOEXCEPT
{
    static const auto table = generator_table();
    auto d = to_limbs(secret);
    if (!is_scalar(d))
        return false;

    limbs_t x{}, y{};
    to_affine(x, y, multiply(table, d));
    out = serialize(x, y);
    wipe(d);
    return true;
}

TEMPLATE
bool CLASS::
is_valid(const point_t& point) NOEXCEPT
{
    projective_t unused{};
    return parse(unused, point);
}

TEMPLATE
bool CLASS::
compress(compressed_t& out, const point_t& point) NOEXCEPT
{
    if (!is_valid(point))
        return false;

    const auto prefix = bit_or(0x02_u8, bit_and(point.back(), 0x01_u8));
    out = splice(data_array<one>{ prefix }, slice<one, add1(size)>(point));
    return true;
}

TEMPLATE
bool CLASS::
decompress(point_t& out, const compressed_t& point) NOEXCEPT
{
    constexpr auto m = field();
    if (point.front() != 0x02 && point.front() != 0x03)
        return false;

    const auto x = to_limbs(slice<one, add1(size)>(point));
    if (!is_less(x, m.value))
        return false;

    // y = rhs^((p + 1) / 4), a square root if rhs is a square (p = 3 mod 4).
    const auto rhs = curve(to_montgomery(x, m));
    const auto root = power(rhs, m.root_exponent, m);
    if (multiply(root, root, m) != rhs)
        return false;

    auto y = from_montgomery(root, m);
    if (get_right(y.front()) != get_right(point.front()))
        difference(y, m.value, y);

    out = serialize(x, y);
    return true;
}

TEMPLATE
bool CLASS::
agree(shared_t& out, const secret_t& secret, const point_t& point) NOEXCEPT
{
    projective_t q{};
    if (!parse(q, point))
        return false;

    auto d = to_limbs(secret);
    if (!is_scalar(d))
        return false;

    limbs_t x{}, y{};
    const auto valid = to_affine(x, y, multiply(make_table(q), d));
    out = to_bytes(x);
    wipe(d);
    wipe(x);
    wipe(y);
    return valid;
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
