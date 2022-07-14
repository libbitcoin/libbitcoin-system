/***********************************************************************
 * Copyright (c) 2015 Pieter Wuille                                    *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/
#include <bitcoin/system/crypto/external/lax_der_parsing.hpp>

#include <secp256k1.h>
#include <bitcoin/system/define.hpp>

BC_PUSH_WARNING(USE_NOT_NULL)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)

bool ecdsa_signature_parse_der_lax(const secp256k1_context* ctx,
    secp256k1_ecdsa_signature* sig, const unsigned char *input,
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