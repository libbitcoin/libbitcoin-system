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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_ECDSA_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_ECDSA_IPP

// ECDSA
// ============================================================================
// The digest is converted to a scalar from its leftmost bits (bits2int),
// reduced modulo the order. The nonce is the rfc6979 derivation with the
// curve hash.

namespace libbitcoin {
namespace system {
namespace nist {

// public
// ----------------------------------------------------------------------------

TEMPLATE
bool CLASS::
sign(signature_t& out, const secret_t& secret, const_byte_span digest) NOEXCEPT
{
    using hmac_t = hmac<typename C::H>;
    using digest_t = typename hmac_t::digest_t;
    static const auto table = generator_table();
    constexpr auto n = scalar();
    constexpr data_array<one> zero_byte{ 0x00 };
    constexpr data_array<one> one_byte{ 0x01 };

    auto d = to_limbs(secret);
    if (!is_scalar(d))
        return false;

    const auto e = to_scalar(digest);
    const auto h1 = to_bytes(e);
    const auto em = to_montgomery(e, n);
    auto dm = to_montgomery(d, n);
    const auto mac = [](const digest_t& key, const auto&... parts) NOEXCEPT
    {
        hmac_t code{ key };
        (code.write(parts), ...);
        const auto digest = code.flush();
        wipe(code);
        return digest;
    };

    digest_t v{};
    v.fill(0x01);
    auto k = mac(digest_t{}, v, zero_byte, secret, h1);
    v = mac(k, v);
    k = mac(k, v, one_byte, secret, h1);
    v = mac(k, v);

    LCOV_EXCL_START("Repeats only for an invalid nonce (rfc6979 3.2.h).")
    while (true)
    LCOV_EXCL_STOP()
    {
        secret_t candidate{};
        for (size_t filled{}; filled < size; filled += v.size())
        {
            v = mac(k, v);
            const auto count = std::min(v.size(), size - filled);
            const auto to = std::next(candidate.begin(), filled);
            std::copy_n(v.cbegin(), count, to);
        }

        auto nonce = to_limbs(candidate);
        if (is_scalar(nonce))
        {
            limbs_t x{}, y{};
            to_affine(x, y, multiply(table, nonce));
            const auto r = reduce(x);
            const auto rm = to_montgomery(r, n);
            auto km = to_montgomery(nonce, n);
            auto total = add(em, multiply(rm, dm, n), n);
            auto sm = multiply(inverse(km, n), total, n);
            const auto s = from_montgomery(sm, n);

            if (!is_zero(r) && !is_zero(s))
            {
                out = splice(to_bytes(r), to_bytes(s));
                wipe(d);
                wipe(dm);
                wipe(k);
                wipe(v);
                wipe(candidate);
                wipe(nonce);
                wipe(km);
                wipe(total);
                wipe(sm);
                return true;
            }
        }

        LCOV_EXCL_START("Requires an invalid nonce (rfc6979 3.2.h).")
        k = mac(k, v, zero_byte);
        v = mac(k, v);
        LCOV_EXCL_STOP()
    }
}

TEMPLATE
bool CLASS::
verify(const signature_t& signature, const point_t& point,
    const_byte_span digest) NOEXCEPT
{
    static const auto generators = generator_table();
    constexpr auto n = scalar();
    const auto r = to_limbs(slice<zero, size>(signature));
    const auto s = to_limbs(slice<size, two * size>(signature));
    if (!is_scalar(r) || !is_scalar(s))
        return false;

    projective_t q{};
    if (!parse(q, point))
        return false;

    const auto w = inverse(to_montgomery(s, n), n);
    const auto e = to_scalar(digest);
    const auto u1 = from_montgomery(multiply(to_montgomery(e, n), w, n), n);
    const auto u2 = from_montgomery(multiply(to_montgomery(r, n), w, n), n);

    limbs_t x{}, y{};
    const auto combined = multiply(u1, generators, u2, make_table(q));
    if (!to_affine(x, y, combined))
        return false;

    return reduce(x) == r;
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
to_scalar(const_byte_span digest) NOEXCEPT
{
    bytes_t bytes{};
    const auto count = std::min(size, digest.size());
    const auto offset = size - count;
    std::copy_n(digest.begin(), count, std::next(bytes.begin(), offset));

    return reduce(to_limbs(bytes));
}

TEMPLATE
constexpr bool CLASS::
is_scalar(const limbs_t& value) NOEXCEPT
{
    return !is_zero(value) && is_less(value, scalar().value);
}

// A value below 2^(8 * size), which is less than twice the order.
TEMPLATE
constexpr typename CLASS::limbs_t CLASS::
reduce(const limbs_t& value) NOEXCEPT
{
    limbs_t reduced{}, out{};
    const auto borrow = difference(reduced, value, scalar().value);
    select(out, twos_complement(borrow), value, reduced);
    return out;
}

} // namespace nist
} // namespace system
} // namespace libbitcoin

#endif
