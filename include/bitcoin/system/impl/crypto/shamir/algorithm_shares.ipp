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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_SHARES_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_SHARES_IPP

// Shares
// ============================================================================
// A sharing of threshold t is a polynomial of degree t-1 over GF(256), given
// by t-2 random shares, the digest share and the secret. The secret and its
// digest are interpolated from any t shares, and the digest authenticates the
// secret, so an incorrect or substituted share is detected on recovery.

namespace libbitcoin {
namespace system {
namespace shamir {

// public
// ----------------------------------------------------------------------------

TEMPLATE
constexpr size_t CLASS::
entropy_size(size_t threshold) NOEXCEPT
{
    // A single share is the secret, and requires no randomness.
    if (threshold < two)
        return zero;

    const auto randoms = sub1(sub1(threshold));
    return randoms * secret_bytes + random_bytes;
}

TEMPLATE
bool CLASS::
split(shares_t& out, const secret_t& secret, size_t threshold, size_t count,
    const data_slice& entropy) NOEXCEPT
{
    if (!is_valid_sharing(threshold, count))
        return false;

    if (entropy.size() != entropy_size(threshold))
        return false;

    out.clear();
    out.reserve(count);

    if (is_one(threshold))
    {
        for (auto index = zero; index < count; ++index)
            out.push_back({ narrow_cast<uint8_t>(index), secret });

        return true;
    }

    // The random shares are shares, and with the digest share and the secret
    // define the polynomial from which the remaining shares are interpolated.
    shares_t base{};
    base.reserve(threshold);
    const auto randoms = sub1(sub1(threshold));
    auto next = entropy.begin();

    for (auto index = zero; index < randoms; ++index)
    {
        share_t share{ narrow_cast<uint8_t>(index), {} };
        std::copy_n(next, secret_bytes, share.value.begin());
        std::advance(next, secret_bytes);
        out.push_back(share);
        base.push_back(share);
    }

    random_t random{};
    std::copy_n(next, random_bytes, random.begin());
    const auto code = digest(random, secret);
    base.push_back({ digest_index, splice(code, random) });
    base.push_back({ secret_index, secret });

    for (auto index = randoms; index < count; ++index)
    {
        const auto coordinate = narrow_cast<uint8_t>(index);
        out.push_back({ coordinate, interpolate(coordinate, base) });
    }

    return true;
}

TEMPLATE
bool CLASS::
recover(secret_t& out, const shares_t& shares, size_t threshold) NOEXCEPT
{
    if (is_zero(threshold) || shares.size() != threshold)
        return false;

    if (!is_distinct(shares))
        return false;

    if (is_one(threshold))
    {
        out = shares.front().value;
        return true;
    }

    const auto secret = interpolate(secret_index, shares);
    const auto share = interpolate(digest_index, shares);
    const auto code = slice<zero, digest_bytes>(share);
    const auto random = slice<digest_bytes, secret_bytes>(share);

    if (code != digest(random, secret))
        return false;

    out = secret;
    return true;
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::digest_t CLASS::
digest(const data_slice& random, const secret_t& secret) NOEXCEPT
{
    const auto code = hmac<sha256>::code(secret, random);
    return slice<zero, digest_bytes>(code);
}

TEMPLATE
bool CLASS::
is_valid_sharing(size_t threshold, size_t count) NOEXCEPT
{
    return !is_zero(threshold) && threshold <= count && count <= digest_index;
}

TEMPLATE
bool CLASS::
is_distinct(const shares_t& shares) NOEXCEPT
{
    for (auto share = shares.cbegin(); share != shares.cend(); ++share)
        for (auto other = std::next(share); other != shares.cend(); ++other)
            if (other->index == share->index)
                return false;

    return true;
}

} // namespace shamir
} // namespace system
} // namespace libbitcoin

#endif
