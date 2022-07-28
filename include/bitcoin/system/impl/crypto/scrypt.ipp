/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SCRYPT_IPP

#include <atomic>
#include <algorithm>
#include <bit>
#include <execution>
#include <memory>
#include <bitcoin/system/crypto/pbkd_sha256.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

#define TEMPLATE \
template<size_t W, size_t R, size_t P, bool Concurrent, \
    bool_if<is_scrypt_args<W, R, P>> If>
#define CLASS scrypt<W, R, P, Concurrent, If>

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// private
// ----------------------------------------------------------------------------

TEMPLATE
constexpr auto CLASS::concurrency() NOEXCEPT
{
    if constexpr (Concurrent)
        return std::execution::par_unseq;
    else
        return std::execution::seq;
}

TEMPLATE
template<typename Block>
inline auto CLASS::allocate() NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::shared_ptr<Block>(new Block);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS::words_t& CLASS::add(words_t& to,
    const words_t& from) NOEXCEPT
{
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pto = to.data();
    auto pfrom = from.data();
    BC_POP_WARNING()

    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    pto[0] += pfrom[0];
    pto[1] += pfrom[1];
    pto[2] += pfrom[2];
    pto[3] += pfrom[3];
    pto[4] += pfrom[4];
    pto[5] += pfrom[5];
    pto[6] += pfrom[6];
    pto[7] += pfrom[7];
    pto[8] += pfrom[8];
    pto[9] += pfrom[9];
    pto[10] += pfrom[10];
    pto[11] += pfrom[11];
    pto[12] += pfrom[12];
    pto[13] += pfrom[13];
    pto[14] += pfrom[14];
    pto[15] += pfrom[15];
    BC_POP_WARNING()

    return to;
}

TEMPLATE
constexpr CLASS::block_t& CLASS::xor_(block_t& to,
    const block_t& from) NOEXCEPT
{
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pto = to.data();
    auto pfrom = from.data();
    BC_POP_WARNING()
        
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    pto[0] ^= pfrom[0];
    pto[1] ^= pfrom[1];
    pto[2] ^= pfrom[2];
    pto[3] ^= pfrom[3];
    pto[4] ^= pfrom[4];
    pto[5] ^= pfrom[5];
    pto[6] ^= pfrom[6];
    pto[7] ^= pfrom[7];
    pto[8] ^= pfrom[8];
    pto[9] ^= pfrom[9];
    pto[10] ^= pfrom[10];
    pto[11] ^= pfrom[11];
    pto[12] ^= pfrom[12];
    pto[13] ^= pfrom[13];
    pto[14] ^= pfrom[14];
    pto[15] ^= pfrom[15];
    pto[16] ^= pfrom[16];
    pto[17] ^= pfrom[17];
    pto[18] ^= pfrom[18];
    pto[19] ^= pfrom[19];
    pto[20] ^= pfrom[20];
    pto[21] ^= pfrom[21];
    pto[22] ^= pfrom[22];
    pto[23] ^= pfrom[23];
    pto[24] ^= pfrom[24];
    pto[25] ^= pfrom[25];
    pto[26] ^= pfrom[26];
    pto[27] ^= pfrom[27];
    pto[28] ^= pfrom[28];
    pto[29] ^= pfrom[29];
    pto[30] ^= pfrom[30];
    pto[31] ^= pfrom[31];
    pto[32] ^= pfrom[32];
    pto[33] ^= pfrom[33];
    pto[34] ^= pfrom[34];
    pto[35] ^= pfrom[35];
    pto[36] ^= pfrom[36];
    pto[37] ^= pfrom[37];
    pto[38] ^= pfrom[38];
    pto[39] ^= pfrom[39];
    pto[40] ^= pfrom[40];
    pto[41] ^= pfrom[41];
    pto[42] ^= pfrom[42];
    pto[43] ^= pfrom[43];
    pto[44] ^= pfrom[44];
    pto[45] ^= pfrom[45];
    pto[46] ^= pfrom[46];
    pto[47] ^= pfrom[47];
    pto[48] ^= pfrom[48];
    pto[49] ^= pfrom[49];
    pto[50] ^= pfrom[50];
    pto[51] ^= pfrom[51];
    pto[52] ^= pfrom[52];
    pto[53] ^= pfrom[53];
    pto[54] ^= pfrom[54];
    pto[55] ^= pfrom[55];
    pto[56] ^= pfrom[56];
    pto[57] ^= pfrom[57];
    pto[58] ^= pfrom[58];
    pto[59] ^= pfrom[59];
    pto[60] ^= pfrom[60];
    pto[61] ^= pfrom[61];
    pto[62] ^= pfrom[62];
    pto[63] ^= pfrom[63];
    BC_POP_WARNING()

    return to;
}

TEMPLATE
constexpr CLASS::rblock_t& CLASS::xor_(rblock_t& to,
    const rblock_t& from) NOEXCEPT
{
    for (size_t i = 0; i < (R << 1); ++i)
        xor_(to[i], from[i]);

    return to;
}

TEMPLATE
inline size_t CLASS::index(const rblock_t& rblock) NOEXCEPT
{
    // rfc7914
    // j = Integerify (X) mod N
    //    where Integerify (B[0] ... B[2 * r - 1]) is defined
    //    as the result of interpreting B[2 * r - 1] as a
    //    little-endian integer.
    return
        possible_narrow_cast<size_t>(
            native_from_little_end(
                unsafe_byte_cast<uint64_t>(
                    rblock.back().data())) % W);
}

TEMPLATE
template <size_t A, size_t B, size_t C, size_t D>
constexpr void CLASS::salsa_qr(words_t& words) NOEXCEPT
{
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto pwords = words.data();
    BC_POP_WARNING()

    // Salsa20/8 Quarter Round
    BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
    pwords[B] ^= std::rotl(pwords[A] + pwords[D], 7);
    pwords[C] ^= std::rotl(pwords[B] + pwords[A], 9);
    pwords[D] ^= std::rotl(pwords[C] + pwords[B], 13);
    pwords[A] ^= std::rotl(pwords[D] + pwords[C], 18);
    BC_POP_WARNING()
}

TEMPLATE
CLASS::block_t& CLASS::salsa_8(block_t& block) NOEXCEPT
{
    // Save a copy of the block and make a block of working space.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (2 * 64)] bytes stack allocated.
    // rfc7914: #define R(a,b) (((a) << (b)) | ((a) >> (32 - (b))))
    const auto save = from_little_endians(array_cast<word_t>(block));
    words_t words{ save };
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // rfc7914: Salsa20/8 Core Function (in two forms)

#ifdef USE_QUARTER_ROUNDS
    // round 1
    salsa_qr< 0,  4,  8, 12>(words);
    salsa_qr< 5,  9, 13,  1>(words);
    salsa_qr<10, 14,  2,  6>(words);
    salsa_qr<15,  3,  7, 11>(words);

    // round 2
    salsa_qr< 0,  1,  2,  3>(words);
    salsa_qr< 5,  6,  7,  4>(words);
    salsa_qr<10, 11,  8,  9>(words);
    salsa_qr<15, 12, 13, 14>(words);

    // round 3
    salsa_qr< 0,  4,  8, 12>(words);
    salsa_qr< 5,  9, 13,  1>(words);
    salsa_qr<10, 14,  2,  6>(words);
    salsa_qr<15,  3,  7, 11>(words);

    // round 4
    salsa_qr< 0,  1,  2,  3>(words);
    salsa_qr< 5,  6,  7,  4>(words);
    salsa_qr<10, 11,  8,  9>(words);
    salsa_qr<15, 12, 13, 14>(words);
    
    // round 5
    salsa_qr< 0,  4,  8, 12>(words);
    salsa_qr< 5,  9, 13,  1>(words);
    salsa_qr<10, 14,  2,  6>(words);
    salsa_qr<15,  3,  7, 11>(words);
    
    // round 6
    salsa_qr< 0,  1,  2,  3>(words);
    salsa_qr< 5,  6,  7,  4>(words);
    salsa_qr<10, 11,  8,  9>(words);
    salsa_qr<15, 12, 13, 14>(words);

    // round 7
    salsa_qr< 0,  4,  8, 12>(words);
    salsa_qr< 5,  9, 13,  1>(words);
    salsa_qr<10, 14,  2,  6>(words);
    salsa_qr<15,  3,  7, 11>(words);

    // round 8
    salsa_qr< 0,  1,  2,  3>(words);
    salsa_qr< 5,  6,  7,  4>(words);
    salsa_qr<10, 11,  8,  9>(words);
    salsa_qr<15, 12, 13, 14>(words);
#else
    BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
    auto x = words.data();
    BC_POP_WARNING()
    
    // salsa20/8 is salsa20 with 8 vs. 20 rounds.
    for (size_t i = 0; i < 4u; ++i)
    {
        BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
        // columns
        x[ 4] ^= std::rotl(x[ 0] + x[12],  7);
        x[ 8] ^= std::rotl(x[ 4] + x[ 0],  9);
        x[12] ^= std::rotl(x[ 8] + x[ 4], 13);
        x[ 0] ^= std::rotl(x[12] + x[ 8], 18);
    
        x[ 9] ^= std::rotl(x[ 5] + x[ 1],  7);
        x[13] ^= std::rotl(x[ 9] + x[ 5],  9);
        x[ 1] ^= std::rotl(x[13] + x[ 9], 13);
        x[ 5] ^= std::rotl(x[ 1] + x[13], 18);
    
        x[14] ^= std::rotl(x[10] + x[ 6],  7);
        x[ 2] ^= std::rotl(x[14] + x[10],  9);
        x[ 6] ^= std::rotl(x[ 2] + x[14], 13);
        x[10] ^= std::rotl(x[ 6] + x[ 2], 18);
    
        x[ 3] ^= std::rotl(x[15] + x[11],  7);
        x[ 7] ^= std::rotl(x[ 3] + x[15],  9);
        x[11] ^= std::rotl(x[ 7] + x[ 3], 13);
        x[15] ^= std::rotl(x[11] + x[ 7], 18);
    
        // rows
        x[ 1] ^= std::rotl(x[ 0] + x[ 3],  7);
        x[ 2] ^= std::rotl(x[ 1] + x[ 0],  9);
        x[ 3] ^= std::rotl(x[ 2] + x[ 1], 13);
        x[ 0] ^= std::rotl(x[ 3] + x[ 2], 18);
    
        x[ 6] ^= std::rotl(x[ 5] + x[ 4],  7);
        x[ 7] ^= std::rotl(x[ 6] + x[ 5],  9);
        x[ 4] ^= std::rotl(x[ 7] + x[ 6], 13);
        x[ 5] ^= std::rotl(x[ 4] + x[ 7], 18);
    
        x[11] ^= std::rotl(x[10] + x[ 9],  7);
        x[ 8] ^= std::rotl(x[11] + x[10],  9);
        x[ 9] ^= std::rotl(x[ 8] + x[11], 13);
        x[10] ^= std::rotl(x[ 9] + x[ 8], 18);
    
        x[12] ^= std::rotl(x[15] + x[14],  7);
        x[13] ^= std::rotl(x[12] + x[15],  9);
        x[14] ^= std::rotl(x[13] + x[12], 13);
        x[15] ^= std::rotl(x[14] + x[13], 18);
        BC_POP_WARNING()
    }
#endif

    // rfc7914: for (i = 0;i < 16;++i) out[i] = x[i] + in[i];
    // Add saved words to copy and emit in original form (little-endian).
    to_little_endians(array_cast<word_t>(block), add(words, save));
    return block;
}

TEMPLATE
bool CLASS::block_mix(rblock_t& rblock) NOEXCEPT
{
    // Make a working block initialized from rblock.back().
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (1 *  64)] bytes stack allocated.
    // rfc7914
    // 1. X = B[2 * r - 1]
    block_t xblock{ rblock.back() };
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if defined(BLOCK_MIX_NORMAL_FORM)

    // Make 2R working blocks (1 rblock).
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (R * 128)] bytes heap allocated.
    const auto ptr = allocate<rblock_t>();
    if (!ptr) return false;
    auto& yrblock = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // rfc7914
    // 2. for i = 0 to 2 * r - 1 do
    //    Y[i] = Salsa (X xor B[i])
    // end for
    for (size_t i = 0; i < (R << 1); ++i)
        yrblock[i] = salsa_8(xor_(xblock, rblock[i]));

    // rfc7914
    // 3a. B' = (Y[0], Y[2], ..., Y[2 * r - 2],...
    for (size_t i = 0, j = 0; i < (R << 1); i += 2)
        rblock[j++] = yrblock[i];

    // rfc7914
    // 3b. B' = ...Y[1], Y[3], ..., Y[2 * r - 1])
    for (size_t i = 1, j = R; i < (R << 1); i += 2)
        rblock[j++] = yrblock[i];

#elif defined(BLOCK_MIX_ANOTHER_FORM)

    // Make 2R working blocks (1 rblock).
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (R * 128)] bytes heap allocated.
    const auto ptr = allocate<rblock_t>();
    if (!ptr) return false;
    auto& yrblock = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for (size_t i = 0; i < (R << 1); ++i)
        yrblock[i] = salsa_8(xor_(xblock, rblock[i]));

    for (size_t i = 0; i < (R << 0); ++i)
        rblock[i] = yrblock[i << 1];

    for (size_t i = 0; i < (R << 0); ++i)
        rblock[i + R] = yrblock[add1(i << 1)];

#elif defined(BLOCK_MIX_OPTIMIZED_FORM)

    // Make R working blocks (half rblock).
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (R * 64)] bytes heap allocated.
    const auto ptr = allocate<std_array<block_t, R>>();
    if (!ptr) return false;
    auto& yblock = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    for (size_t i = 0, j = 0; i < R; ++i)
    {
        rblock[i] = salsa_8(xor_(xblock, rblock[j++]));
        yblock[i] = salsa_8(xor_(xblock, rblock[j++]));
    }

    for (size_t i = 0, j = R; i < R; ++i)
        rblock[j++] = yblock[i];

#else // BLOCK_MIX_OPTIMAL_FORM

    // Make R-1 working blocks.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (sub1(R) * 64)] bytes heap allocated.
    const auto ptr = allocate<std_array<block_t, sub1(R)>>();
    if (!ptr) return false;
    auto& yblock = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Compiler should elide both loops and (empty) array allocation when R = 1.

    // Direct copy even blocks, odds (except last) require temporary storage.
    for (size_t i = 0, j = 0; i < sub1(R); ++i)
    {
        rblock[i] = salsa_8(xor_(xblock, rblock[j++]));
        yblock[i] = salsa_8(xor_(xblock, rblock[j++]));
    }

    // Last even block here simplifies the first loop.
    rblock[sub1(R << 0)] = salsa_8(xor_(xblock, rblock[sub1(sub1(R << 1))]));

    // Last (odd) block does not require a temporary, since no block follows.
    salsa_8(xor_(rblock[sub1(R << 1)], xblock));

    // Relocate odd blocks (except last) to second half of the rblock.
    for (size_t i = 0, j = R; i < sub1(R); ++i)
        rblock[j++] = yblock[i];

#endif // BLOCK_MIX_OPTIMAL_FORM

    return true;
}

TEMPLATE
bool CLASS::romix(rblock_t& rblock) NOEXCEPT
{
    // Make a working set of W rblocks.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (W * (R * 128))] bytes heap allocated.
    const auto ptr = allocate<wrblock_t>();
    if (!ptr) return false;
    auto& wrblocks = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // rfc7914
    // 1. X = B
    // 2. for i = 0 to N - 1 do
    //    V[i] = X
    //    X = scryptBlockMix (X)
    // end for
    for (size_t i = 0; i < W; ++i)
    {
        wrblocks[i] = rblock;
        if (!block_mix(rblock))
            return false;
    }

    // rfc7914
    // 3. for i = 0 to N - 1 do
    //    j = Integerify (X) mod N
    //    X = scryptBlockMix (X xor V[j])
    // end for
    // 4. B' = X
    for (size_t i = 0; i < W; ++i)
        if (!block_mix(xor_(rblock, wrblocks[index(rblock)])))
            return false;

    return true;
}

// public
// ----------------------------------------------------------------------------

TEMPLATE
bool CLASS::hash(const data_slice& phrase, const data_slice& salt,
    uint8_t* buffer, size_t size) NOEXCEPT
{
    static_assert(size_of<prblock_t>() <= pbkd::sha256::maximum_size);
    if (is_null(buffer) || (size > pbkd::sha256::maximum_size))
        return false;

    // Make a working set of P rblocks.
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // [P * (R * 128)] bytes heap allocated.
    const auto ptr = allocate<prblock_t>();
    if (!ptr) return false;
    auto& prblocks = *ptr;
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // rfc7914
    // 1. Initialize an array B consisting of p blocks of 128 * r octets each:
    //    B[0] || B[1] || ... || B[p - 1] =
    //       PBKDF2-HMAC-SHA256 (P, S, 1, p * 128 * r)
    auto& b = array_cast<uint8_t>(prblocks);
    pbkd::sha256::hash(phrase.data(), phrase.size(), salt.data(), salt.size(),
        one, b.data(), b.size());

    // rfc7914
    // 2. for i = 0 to p - 1 do
    //    B[i] = scryptROMix (r, B[i], N)
    // end for
    std::atomic_bool success{ true };
    std::for_each(concurrency(), prblocks.begin(), prblocks.end(),
        [&](rblock_t& rblock) NOEXCEPT
        {
            success = success && romix(rblock);
        });

    // rfc7914
    // 3. DK = PBKDF2-HMAC-SHA256 (P, B[0] || B[1] || ... || B[p - 1], 1, dkLen)
    return success &&
        pbkd::sha256::hash(phrase.data(), phrase.size(), b.data(), b.size(),
            one, buffer, size);
}

BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

} // namespace system
} // namespace libbitcoin

#endif
