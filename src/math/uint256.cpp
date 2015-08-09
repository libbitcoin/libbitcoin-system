// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/bitcoin/math/uint256.hpp>

#include <stdio.h>
#include <string.h>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

template <unsigned int BITS>
base_uint<BITS>::base_uint(const std::vector<unsigned char>& vch)
{
    if (vch.size() != sizeof(pn))
        throw uint_error("Converting vector of wrong size to base_uint");

    memcpy(pn, &vch[0], sizeof(pn));
}

template <unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator<<=(unsigned int shift)
{
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++)
        pn[i] = 0;

    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++)
    {
        if (i + k + 1 < WIDTH && shift != 0)
            pn[i + k + 1] |= (a.pn[i] >> (32 - shift));

        if (i + k < WIDTH)
            pn[i + k] |= (a.pn[i] << shift);
    }

    return *this;
}

template <unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator>>=(unsigned int shift)
{
    base_uint<BITS> a(*this);
    for (int i = 0; i < WIDTH; i++)
        pn[i] = 0;

    int k = shift / 32;
    shift = shift % 32;
    for (int i = 0; i < WIDTH; i++)
    {
        if (i - k - 1 >= 0 && shift != 0)
            pn[i - k - 1] |= (a.pn[i] << (32 - shift));

        if (i - k >= 0)
            pn[i - k] |= (a.pn[i] >> shift);
    }

    return *this;
}

template <unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator*=(uint32_t b32)
{
    uint64_t carry = 0;
    for (int i = 0; i < WIDTH; i++)
    {
        uint64_t n = carry + (uint64_t)b32 * pn[i];
        pn[i] = n & 0xffffffff;
        carry = n >> 32;
    }

    return *this;
}

template <unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator*=(const base_uint& b)
{
    base_uint<BITS> a = *this;
    *this = 0;
    for (int j = 0; j < WIDTH; j++)
    {
        uint64_t carry = 0;
        for (int i = 0; i + j < WIDTH; i++)
        {
            uint64_t n = carry + pn[i + j] + (uint64_t)a.pn[j] * b.pn[i];
            pn[i + j] = n & 0xffffffff;
            carry = n >> 32;
        }
    }

    return *this;
}

template <unsigned int BITS>
base_uint<BITS>& base_uint<BITS>::operator/=(const base_uint& b)
{
    // make a copy, so we can shift.
    base_uint<BITS> div = b;

    // make a copy, so we can subtract.
    base_uint<BITS> num = *this; 

    // the quotient.
    *this = 0;

    int num_bits = num.bits();
    int div_bits = div.bits();
    if (div_bits == 0)
        throw uint_error("Division by zero");

    // the result is certainly 0.
    if (div_bits > num_bits)
        return *this;

    int shift = num_bits - div_bits;

    // shift so that div and nun align.
    div <<= shift;

    while (shift >= 0)
    {
        if (num >= div)
        {
            num -= div;

            // set a bit of the result.
            pn[shift / 32] |= (1 << (shift & 31));
        }

        // shift back.
        div >>= 1;
        shift--;
    }

    // num now contains the remainder of the division.
    return *this;
}

template <unsigned int BITS>
int base_uint<BITS>::CompareTo(const base_uint<BITS>& b) const
{
    for (int i = WIDTH - 1; i >= 0; i--)
    {
        if (pn[i] < b.pn[i])
            return -1;

        if (pn[i] > b.pn[i])
            return 1;
    }

    return 0;
}

template <unsigned int BITS>
bool base_uint<BITS>::EqualTo(uint64_t b) const
{
    for (int i = WIDTH - 1; i >= 2; i--)
        if (pn[i] != 0)
            return false;

    if (pn[1] != (b >> 32))
        return false;

    if (pn[0] != (b & 0xfffffffful))
        return false;

    return true;
}

template <unsigned int BITS>
unsigned int base_uint<BITS>::bits() const
{
    for (int pos = WIDTH - 1; pos >= 0; pos--)
    {
        if (pn[pos] != 0)
        {
            for (int bits = 31; bits > 0; bits--)
                if ((pn[pos] & 1 << bits) != 0)
                    return 32 * pos + bits + 1;

            return 32 * pos + 1;
        }
    }

    return 0;
}

// Explicit instantiations for base_uint<256>
template base_uint<256>::base_uint(const std::vector<unsigned char>&);
template base_uint<256>& base_uint<256>::operator<<=(unsigned int);
template base_uint<256>& base_uint<256>::operator>>=(unsigned int);
template base_uint<256>& base_uint<256>::operator*=(uint32_t b32);
template base_uint<256>& base_uint<256>::operator*=(const base_uint<256>& b);
template base_uint<256>& base_uint<256>::operator/=(const base_uint<256>& b);
template int base_uint<256>::CompareTo(const base_uint<256>&) const;
template bool base_uint<256>::EqualTo(uint64_t) const;
template unsigned int base_uint<256>::bits() const;

uint32_t uint256_t::GetCompact(bool fNegative) const
{
    int nSize = (bits() + 7) / 8;
    uint32_t nCompact = 0;

    if (nSize <= 3)
    {
        nCompact = (uint32_t)(GetLow64() << 8 * (3 - nSize));
    }
    else
    {
        uint256_t bn = *this >> 8 * (nSize - 3);
        nCompact = (uint32_t)(bn.GetLow64());
    }

    // The 0x00800000 bit denotes the sign. Thus, if it is already set,
    // divide the mantissa by 256 and increase the exponent.
    if (nCompact & 0x00800000)
    {
        nCompact >>= 8;
        nSize++;
    }

    BITCOIN_ASSERT((nCompact & ~0x007fffff) == 0);
    BITCOIN_ASSERT(nSize < 256);
    nCompact |= nSize << 24;
    nCompact |= (fNegative && (nCompact & 0x007fffff) ? 0x00800000 : 0);
    return nCompact;
}

// This implementation directly uses shifts instead of going
// through an intermediate MPI representation.
uint256_t& uint256_t::SetCompact(uint32_t nCompact, bool* pfNegative, 
    bool* pfOverflow)
{
    int nSize = nCompact >> 24;
    uint32_t nWord = nCompact & 0x007fffff;

    if (nSize <= 3) 
    {
        nWord >>= 8 * (3 - nSize);
        *this = nWord;
    }
    else 
    {
        *this = nWord;
        *this <<= 8 * (nSize - 3);
    }

    if (pfNegative != nullptr)
        *pfNegative = nWord != 0 && (nCompact & 0x00800000) != 0;

    if (pfOverflow != nullptr)
        *pfOverflow = nWord != 0 && ((nSize > 34) ||
        (nWord > 0xff && nSize > 33) ||
        (nWord > 0xffff && nSize > 32));

    return *this;
}

} // namespace libbitcoin
