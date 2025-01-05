/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_PBKD_IPP
#define LIBBITCOIN_SYSTEM_HASH_PBKD_IPP

#include <algorithm>

// based on:
// datatracker.ietf.org/doc/html/rfc8018
// [Moriarty, Kaliski, Rusch]

namespace libbitcoin {
namespace system {
    
// Bugus use constexpr warning with consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

// static/protected
TEMPLATE
template <size_t Length>
constexpr auto CLASS::
xor_n(data_array<Length>& to, const data_array<Length>& from) NOEXCEPT
{
    // rfc8018
    // F (P, S, c, i) = U_1 \xor U_2 \xor ... \xor U_c
    for (size_t i = 0; i < Length; ++i)
        to[i] ^= from[i];
};

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// pkcs5 pbkdf2 finalized code
// ---------------------------------------------------------------------------

// rfc8018
// If dkLen > (2^32 - 1) * hLen, output "derived key too long" and stop.
// hLen denotes the length in octets of the pseudorandom function [PRF] output.
// [Algorithm = PRF, Size = dkLen (public parameter naming)]
TEMPLATE
template <size_t Size, if_not_greater<Size, pbkd_maximum_size<Algorithm>>>
inline void CLASS::
key(data_array<Size>& out, const data_slice& password,
    const data_slice& salt, size_t count) NOEXCEPT
{
    constexpr auto hlen = array_count<typename Algorithm::digest_t>;

    // rfc8018
    // l = CEIL (dkLen / hLen)
    // Let l be the number of hLen-octet blocks in the derived key, rounding up.
    constexpr auto l = ceilinged_divide(Size, hlen);

    // rfc8018
    // r = dkLen - (l - 1) * hLen
    // let r be the number of octets in the last block.
    constexpr auto r = Size - sub1(l) * hlen;

    // rfc8018
    // and the concatenation of [PS] and the block index i: where INT (i) is a
    // four-octet encoding of the integer i [because of the 1-based algorithm,
    // position 0 is wasted], most significant octet first [big-endian].
    constexpr auto words = to_big_endians(sequence<uint32_t, add1(l)>);

    // Cast precomputed array of block indexes as an array of 4 byte BE arrays.
    const auto& index = array_cast<std_array<uint8_t, sizeof(uint32_t)>>(words);

    // rfc8018
    // Output the derived key DK.
    auto it = out.begin();

    // rfc8018
    // applied to the password P and the concatenation of the salt S [PS].
    // [salted and unsalted hmac accumulators are saved here for reuse.]
    const hmac<Algorithm> hmac_p(password);
    auto hmac_ps = hmac_p;
    hmac_ps.write(salt);

    // rfc8018
    // apply the function F to P, S, c, and index [i] to compute the block:
    // F (P, S, c, i) = U_1 \xor U_2 \xor ... \xor U_c
    for (size_t i = 1; i <= l; ++i)
    {
        // rfc8018
        // where F is defined as the exclusive-or sum of the first c iterates
        // of PRF applied to P and the concatenation of S and block index i.

        // U_1 = PRF (P, S || INT (i))
        auto ps = hmac_ps;
        ps.write(index.at(i));
        auto u = ps.flush();
        auto t = u;

        for (size_t c = 2; c <= count; ++c)
        {
            // rfc8018
            // U_c = PRF (P, U_{c-1})
            auto p = hmac_p;
            p.write(u);
            u = p.flush();

            // rfc8018
            // F (P, S, c, i) = U_1 \xor U_2 \xor ... \xor U_c
            xor_n(t, u);
        }

        // rfc8018
        // Concatenate blocks and extract first dkLen octets to produce DK.
        // DK = T_1 || T_2 ||  ...  || T_l<0..r-1>
        it = std::copy_n(t.begin(), (i == l ? r : t.size()), it);
    }
}

TEMPLATE
template <size_t Size, if_not_greater<Size, pbkd_maximum_size<Algorithm>>>
inline data_array<Size> CLASS::
key(const data_slice& password, const data_slice& salt,
    size_t count) NOEXCEPT
{
    data_array<Size> dk{};
    key(dk, password, salt, count);
    return dk;
}

} // namespace system
} // namespace libbitcoin

#endif
