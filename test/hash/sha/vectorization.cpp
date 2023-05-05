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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(vectorization_tests)

#if defined(HAVE_VECTOR_CONSTEXPR)

// Merkle root
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(vectorization__sha256__merkle_root__expected)
{
    using sha_256 = sha::algorithm<sha::h512<>, true, true, true>;

    // First round
    // AVX2
    constexpr auto expected1 = sha_256::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha_256::double_hash({ 2 }, { 3 });
    constexpr auto expected3 = sha_256::double_hash({ 4 }, { 5 });
    constexpr auto expected4 = sha_256::double_hash({ 6 }, { 7 });
    // AVX2
    constexpr auto expected5 = sha_256::double_hash({ 8 }, { 9 });
    constexpr auto expected6 = sha_256::double_hash({ 10 }, { 11 });
    constexpr auto expected7 = sha_256::double_hash({ 12 }, { 13 });
    constexpr auto expected8 = sha_256::double_hash({ 14 }, { 15 });
    // AVX2
    constexpr auto expected9 = sha_256::double_hash({ 16 }, { 17 });
    constexpr auto expected10 = sha_256::double_hash({ 18 }, { 19 });
    constexpr auto expected11 = sha_256::double_hash({ 20 }, { 21 });
    constexpr auto expected12 = sha_256::double_hash({ 22 }, { 23 });
    // SSE4
    constexpr auto expected13 = sha_256::double_hash({ 24 }, { 25 });
    constexpr auto expected14 = sha_256::double_hash({ 26 }, { 27 });

    // Second round
    // AVX2
    constexpr auto expected15 = sha_256::double_hash(expected1, expected2);
    constexpr auto expected16 = sha_256::double_hash(expected3, expected4);
    constexpr auto expected17 = sha_256::double_hash(expected5, expected6);
    constexpr auto expected18 = sha_256::double_hash(expected7, expected8);
    // Sequential
    constexpr auto expected19 = sha_256::double_hash(expected9, expected10);
    constexpr auto expected20 = sha_256::double_hash(expected11, expected12);
    constexpr auto expected21 = sha_256::double_hash(expected13, expected14);

    // Third round
    // AVX2
    constexpr auto expected22 = sha_256::double_hash(expected15, expected16);
    constexpr auto expected23 = sha_256::double_hash(expected17, expected18);
    constexpr auto expected24 = sha_256::double_hash(expected19, expected20);
    constexpr auto expected25 = sha_256::double_hash(expected21, expected21);

    // Fourth round
    constexpr auto expected26 = sha_256::double_hash(expected22, expected23);
    constexpr auto expected27 = sha_256::double_hash(expected24, expected25);

    // Fifth round
    constexpr auto expected = sha_256::double_hash(expected26, expected27);

    BOOST_CHECK_EQUAL(sha_256::merkle_root(
    {
        { 0 },  { 1 },  { 2 },  { 3 },  { 4 },  { 5 },  { 6 },  { 7 },
        { 8 },  { 9 },  { 10 }, { 11 }, { 12 }, { 13 }, { 14 }, { 15 },
        { 16 }, { 17 }, { 18 }, { 19 }, { 20 }, { 21 }, { 22 }, { 23 },
        { 24 }, { 25 }, { 26 }, { 27 }
    }), expected);
}

BOOST_AUTO_TEST_CASE(vectorization__sha512__merkle_root__expected)
{
    using sha_512 = sha::algorithm<sha::h512<>, true, true, true>;

    // First round
    // AVX2
    constexpr auto expected1 = sha_512::double_hash({ 0 }, { 1 });
    constexpr auto expected2 = sha_512::double_hash({ 2 }, { 3 });
    constexpr auto expected3 = sha_512::double_hash({ 4 }, { 5 });
    constexpr auto expected4 = sha_512::double_hash({ 6 }, { 7 });
    // AVX2
    constexpr auto expected5 = sha_512::double_hash({ 8 }, { 9 });
    constexpr auto expected6 = sha_512::double_hash({ 10 }, { 11 });
    constexpr auto expected7 = sha_512::double_hash({ 12 }, { 13 });
    constexpr auto expected8 = sha_512::double_hash({ 14 }, { 15 });
    // AVX2
    constexpr auto expected9 = sha_512::double_hash({ 16 }, { 17 });
    constexpr auto expected10 = sha_512::double_hash({ 18 }, { 19 });
    constexpr auto expected11 = sha_512::double_hash({ 20 }, { 21 });
    constexpr auto expected12 = sha_512::double_hash({ 22 }, { 23 });
    // SSE4
    constexpr auto expected13 = sha_512::double_hash({ 24 }, { 25 });
    constexpr auto expected14 = sha_512::double_hash({ 26 }, { 27 });

    // Second round
    // AVX2
    constexpr auto expected15 = sha_512::double_hash(expected1, expected2);
    constexpr auto expected16 = sha_512::double_hash(expected3, expected4);
    constexpr auto expected17 = sha_512::double_hash(expected5, expected6);
    constexpr auto expected18 = sha_512::double_hash(expected7, expected8);
    // Sequential
    constexpr auto expected19 = sha_512::double_hash(expected9, expected10);
    constexpr auto expected20 = sha_512::double_hash(expected11, expected12);
    constexpr auto expected21 = sha_512::double_hash(expected13, expected14);

    // Third round
    // AVX2
    constexpr auto expected22 = sha_512::double_hash(expected15, expected16);
    constexpr auto expected23 = sha_512::double_hash(expected17, expected18);
    constexpr auto expected24 = sha_512::double_hash(expected19, expected20);
    constexpr auto expected25 = sha_512::double_hash(expected21, expected21);

    // Fourth round
    constexpr auto expected26 = sha_512::double_hash(expected22, expected23);
    constexpr auto expected27 = sha_512::double_hash(expected24, expected25);

    // Fifth round
    constexpr auto expected = sha_512::double_hash(expected26, expected27);

    BOOST_CHECK_EQUAL(sha_512::merkle_root(
    {
        { 0 },  { 1 },  { 2 },  { 3 },  { 4 },  { 5 },  { 6 },  { 7 },
        { 8 },  { 9 },  { 10 }, { 11 }, { 12 }, { 13 }, { 14 }, { 15 },
        { 16 }, { 17 }, { 18 }, { 19 }, { 20 }, { 21 }, { 22 }, { 23 },
        { 24 }, { 25 }, { 26 }, { 27 }
    }), expected);
}

// Message scheduling
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(vectorization__sha160__scheduling__expected)
{
    // AVX512, AVX2, SSE4, sequential
    constexpr size_t coverall = 16_size + 8 + 4 + 2 + 1;
    using sha160n = sha::algorithm<sha::h160, true, false, true>;
    using sha160v = sha::algorithm<sha::h160, true, true, true>;
    constexpr auto blocks = std_array<sha160::block_t, coverall>{ 1 };
    constexpr auto hashn = sha160n::hash(blocks);
    constexpr auto hashv = sha160v::hash(blocks);
    static_assert(hashn == hashv);
    BOOST_CHECK_EQUAL(hashn, sha160v::hash(blocks));
}

BOOST_AUTO_TEST_CASE(vectorization__sha256__scheduling__expected)
{
    // AVX512, AVX2, SSE4, sequential
    constexpr size_t coverall = 16_size + 8 + 4 + 2 + 1;
    using sha256n = sha::algorithm<sha::h256<>, true, false, true>;
    using sha256v = sha::algorithm<sha::h256<>, true, true, true>;
    constexpr auto blocks = std_array<sha256::block_t, coverall>{ 1 };
    constexpr auto hashn = sha256n::hash(blocks);
    constexpr auto hashv = sha256v::hash(blocks);
    static_assert(hashn == hashv);
    BOOST_CHECK_EQUAL(hashn, sha256v::hash(blocks));
}

// sha512 vectorization is disabled in x32 builds.
BOOST_AUTO_TEST_CASE(vectorization__sha512__scheduling__expected)
{
    // AVX2, SSE4, sequential
    constexpr size_t coverall = 16_size + 8 + 4 + 2 + 1;
    using sha512n = sha::algorithm<sha::h512<>, true, false, true>;
    using sha512v = sha::algorithm<sha::h512<>, true, true, true>;
    constexpr auto blocks = std_array<sha512::block_t, coverall>{ 1 };
    constexpr auto hashn = sha512n::hash(blocks);
    constexpr auto hashv = sha512v::hash(blocks);
    static_assert(hashn == hashv);
    BOOST_CHECK_EQUAL(hashn, sha512v::hash(blocks));
}

#endif

BOOST_AUTO_TEST_SUITE_END()
