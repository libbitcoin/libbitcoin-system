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
#ifndef LIBBITCOIN_SYSTEM_HASH_HKDF_IPP
#define LIBBITCOIN_SYSTEM_HASH_HKDF_IPP

#include <algorithm>

// based on:
// datatracker.ietf.org/doc/html/rfc5869
// [Krawczyk, Eronen]

namespace libbitcoin {
namespace system {

// rfc5869
// PRK = HMAC-Hash(salt, IKM)
TEMPLATE
inline typename CLASS::digest_t CLASS::
extract(const data_slice& material, const data_slice& salt) NOEXCEPT
{
    return hmac<Algorithm>::code(material, salt);
}

// rfc5869
// T(N) = HMAC-Hash(PRK, T(N-1) | info | N), OKM = first L octets of T.
TEMPLATE
template <size_t Size, if_not_greater<Size, hkdf_maximum_size<Algorithm>>>
inline void CLASS::
expand(data_array<Size>& out, const digest_t& key,
    const data_slice& info) NOEXCEPT
{
    constexpr auto length = array_count<digest_t>;
    constexpr auto blocks = ceilinged_divide(Size, length);
    constexpr auto remain = Size - sub1(blocks) * length;

    digest_t t{};
    auto it = out.begin();
    for (uint8_t block = 1; block <= blocks; ++block)
    {
        const data_array<one> count{ block };
        hmac<Algorithm> mac{ key };

        // rfc5869
        // T(0) = empty string (zero length).
        if (!is_one(block))
            mac.write(t);

        mac.write(info);
        mac.write(count);
        t = mac.flush();
        it = std::copy_n(t.begin(), (block == blocks ? remain : length), it);
    }
}

TEMPLATE
template <size_t Size, if_not_greater<Size, hkdf_maximum_size<Algorithm>>>
inline void CLASS::
key(data_array<Size>& out, const data_slice& material,
    const data_slice& salt, const data_slice& info) NOEXCEPT
{
    expand(out, extract(material, salt), info);
}

TEMPLATE
template <size_t Size, if_not_greater<Size, hkdf_maximum_size<Algorithm>>>
inline data_array<Size> CLASS::
key(const data_slice& material, const data_slice& salt,
    const data_slice& info) NOEXCEPT
{
    data_array<Size> okm{};
    key(okm, material, salt, info);
    return okm;
}

} // namespace system
} // namespace libbitcoin

#endif
