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
#include <bitcoin/system/x509/verify.hpp>

#include <algorithm>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/x509/certificate.hpp>

// rfc5280 6 (path validation, restricted to configured anchors)

namespace libbitcoin {
namespace system {
namespace x509 {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

static code check_time(const certificate& value, uint64_t time) NOEXCEPT
{
    if (time < value.not_before)
        return error::certificate_not_yet_valid;

    if (time > value.not_after)
        return error::certificate_expired;

    return error::x509_success;
}

// Intermediates is the number of authorities below the issuer in the path.
static code check_issuer(const certificate& issuer,
    size_t intermediates) NOEXCEPT
{
    if (!issuer.authority)
        return error::issuer_not_authority;

    if (is_zero(bit_and(issuer.key_usage, key_certificate_sign)))
        return error::key_usage_invalid;

    if (intermediates > issuer.path_length)
        return error::path_length_exceeded;

    return error::x509_success;
}

code verify(const certificates& chain, const certificates& anchors,
    uint64_t time, purpose intent) NOEXCEPT
{
    if (chain.empty())
        return error::chain_empty;

    const auto& leaf = chain.front();
    if (is_zero(bit_and(leaf.key_usage, digital_signature)))
        return error::key_usage_invalid;

    const auto usage = (intent == purpose::server) ?
        leaf.server_authentication : leaf.client_authentication;
    if (!usage)
        return error::extended_key_usage_invalid;

    size_t index{};
    while (true)
    {
        const auto& current = chain[index];
        if (const auto ec = check_time(current, time))
            return ec;

        const auto pinned = [&](const certificate& anchor) NOEXCEPT
        {
            return anchor.encoding == current.encoding;
        };

        if (std::any_of(anchors.begin(), anchors.end(), pinned))
            return error::x509_success;

        const auto issues = [&](const certificate& anchor) NOEXCEPT
        {
            return (anchor.subject == current.issuer) &&
                is_signed_by(current, anchor);
        };

        const auto anchor = std::find_if(anchors.begin(), anchors.end(),
            issues);
        if (anchor != anchors.end())
        {
            if (const auto ec = check_issuer(*anchor, index))
                return ec;

            return check_time(*anchor, time);
        }

        if (add1(index) == chain.size())
            return error::chain_untrusted;

        const auto& issuer = chain[add1(index)];
        if (issuer.subject != current.issuer)
            return error::issuer_mismatch;

        if (!is_signed_by(current, issuer))
            return error::certificate_signature;

        if (const auto ec = check_issuer(issuer, index))
            return ec;

        ++index;
    }
}

BC_POP_WARNING()

} // namespace x509
} // namespace system
} // namespace libbitcoin
