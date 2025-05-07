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
#include <bitcoin/system/crypto/der_parser.hpp>

#include <secp256k1.h>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)

// Based on example code in bip66, but without signature hash byte.
// Format: 0x30 [total-length] 0x02 [R-length] [R] 0x02 [S-length] [S]
// * total-length: 1-byte length descriptor of everything that follows.
// * R-length: 1-byte length descriptor of the R value that follows.
// * R: arbitrary-length big-endian encoded R value. Must use shortest.
// * S-length: 1-byte length descriptor of the S value that follows.
// * S: arbitrary-length big-endian encoded S value. Must use shortest.
bool is_valid_signature_encoding(const bc::system::data_slice& sig) NOEXCEPT
{
    // Minimum and maximum size constraints (strict DER, no sighash byte).
    if (sig.size() < 8u) return false; // Minimum DER size.
    if (sig.size() > 72u) return false; // Maximum DER size.

    // A signature is of type 0x30 (compound).
    if (sig[0] != 0x30u) return false;

    // Make sure the length covers the entire signature (no extra bytes).
    if (sig[1] != sig.size() - 2u) return false;

    // Extract the length of the R element.
    size_t lenR = sig[3];

    // Make sure the length of the S element is still inside the signature.
    if (5u + lenR >= sig.size()) return false;

    // Extract the length of the S element.
    size_t lenS = sig[5u + lenR];

    // Verify that the entire length is the sum of the sub-lengths.
    if (lenR + lenS + 6u != sig.size()) return false;

    // Check whether the R element is an integer.
    if (sig[2] != 0x02u) return false;

    // Zero-length integers are not allowed for R.
    if (lenR == 0u) return false;

    // Negative numbers are not allowed for R.
    if (sig[4u] & 0x80u) return false;

    // Null bytes at the start of R are not allowed, unless R would be negative.
    if (lenR > 1u && sig[4u] == 0x00u && !(sig[5u] & 0x80u)) return false;

    // Check whether the S element is an integer.
    if (sig[4u + lenR] != 0x02u) return false;

    // Zero-length integers are not allowed for S.
    if (lenS == 0u) return false;

    // Negative numbers are not allowed for S.
    if (sig[6u + lenR] & 0x80u) return false;

    // Null bytes at the start of S are not allowed, unless S would be negative.
    if (lenS > 1u && sig[6u + lenR] == 0x00u && !(sig[7u + lenR] & 0x80u))
        return false;

    return true;
}

/***********************************************************************
 * Copyright (c) 2015 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

bool ecdsa_signature_parse_der_lax(const secp256k1_context* ctx,
    secp256k1_ecdsa_signature* sig, const uint8_t* input,
    size_t inputlen) NOEXCEPT
{
    unsigned char tmpsig[64]{};
    size_t rpos, rlen, spos, slen;
    size_t pos = 0;
    size_t lenbyte;

    // Hack to initialize sig with a correctly-parsed but invalid signature.
    secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);

    // Sequence tag byte
    if (pos == inputlen || input[pos] != 0x30)
        return false;

    // Sequence length bytes.
    if (++pos == inputlen)
        return false;

    lenbyte = input[pos++];

    if (lenbyte & 0x80)
    {
        lenbyte -= 0x80;

        if (lenbyte > inputlen - pos)
            return false;

        pos += lenbyte;
    }

    // Integer tag byte for R.
    if (pos == inputlen || input[pos] != 0x02)
        return false;

    pos++;

    // Integer length for R.
    if (pos == inputlen)
        return false;

    lenbyte = input[pos++];

    if (lenbyte & 0x80)
    {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos)
            return false;

        while (lenbyte > 0 && input[pos] == 0)
        {
            pos++;
            lenbyte--;
        }

        if (lenbyte >= sizeof(size_t)) 
            return false;

        rlen = 0;
        while (lenbyte > 0)
        {
            rlen = (rlen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    }
    else
    {
        rlen = lenbyte;
    }

    if (rlen > inputlen - pos)
        return false;

    rpos = pos;
    pos += rlen;

    // Integer tag byte for S.
    if (pos == inputlen || input[pos] != 0x02)
        return false;

    pos++;

    // Integer length for S.
    if (pos == inputlen)
        return false;

    lenbyte = input[pos++];

    if (lenbyte & 0x80)
    {
        lenbyte -= 0x80;
        if (lenbyte > inputlen - pos)
            return false;

        while (lenbyte > 0 && input[pos] == 0)
        {
            pos++;
            lenbyte--;
        }

        if (lenbyte >= sizeof(size_t))
            return false;

        slen = 0;
        while (lenbyte > 0)
        {
            slen = (slen << 8) + input[pos];
            pos++;
            lenbyte--;
        }
    }
    else
    {
        slen = lenbyte;
    }

    if (slen > (inputlen - pos))
        return false;

    spos = pos;

    // Ignore leading zeroes in R.
    while (rlen > 0 && input[rpos] == 0)
    {
        rlen--;
        rpos++;
    }

    auto overflow = false;

    // Copy R value.
    if (rlen > 32)
        overflow = true;
    else
        std::memcpy(tmpsig + 32 - rlen, input + rpos, rlen);

    // Ignore leading zeroes in S.
    while (slen > 0 && input[spos] == 0)
    {
        slen--;
        spos++;
    }

    // Copy S value.
    if (slen > 32)
        overflow = true;
    else
        std::memcpy(tmpsig + 64 - slen, input + spos, slen);

    overflow |= (secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig) == 0);

    if (overflow)
    {
        std::memset(tmpsig, 0, 64);
        secp256k1_ecdsa_signature_parse_compact(ctx, sig, tmpsig);
    }

    return true;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()