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

BOOST_AUTO_TEST_SUITE(intrinsics_xcpu_functional_tests)

#if defined(HAVE_SSE4)
BOOST_AUTO_TEST_CASE(functional__sse4_set_uint32__get_uint32_expected)
{
    if (have_sse41())
    {
        const auto xword = set<xint128_t>(0, 1, 2, 3);
        const auto word0 = get<uint32_t, 0>(xword);
        const auto word1 = get<uint32_t, 1>(xword);
        const auto word2 = get<uint32_t, 2>(xword);
        const auto word3 = get<uint32_t, 3>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u32);
        BOOST_CHECK_EQUAL(word1, 1_u32);
        BOOST_CHECK_EQUAL(word2, 2_u32);
        BOOST_CHECK_EQUAL(word3, 3_u32);
    }
}
#endif

#if defined(HAVE_AVX2)
BOOST_AUTO_TEST_CASE(functional__avx2__set_uint32__get_uint32_expected)
{
    if (have_avx2())
    {
        const auto xword = set<xint256_t>(0, 1, 2, 3, 4, 5, 6, 7);
        const auto word0 = get<uint32_t, 0>(xword);
        const auto word1 = get<uint32_t, 1>(xword);
        const auto word2 = get<uint32_t, 2>(xword);
        const auto word3 = get<uint32_t, 3>(xword);
        const auto word4 = get<uint32_t, 4>(xword);
        const auto word5 = get<uint32_t, 5>(xword);
        const auto word6 = get<uint32_t, 6>(xword);
        const auto word7 = get<uint32_t, 7>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u32);
        BOOST_CHECK_EQUAL(word1, 1_u32);
        BOOST_CHECK_EQUAL(word2, 2_u32);
        BOOST_CHECK_EQUAL(word3, 3_u32);
        BOOST_CHECK_EQUAL(word4, 4_u32);
        BOOST_CHECK_EQUAL(word5, 5_u32);
        BOOST_CHECK_EQUAL(word6, 6_u32);
        BOOST_CHECK_EQUAL(word7, 7_u32);
    }
}
#endif

#if defined(HAVE_AVX512)
BOOST_AUTO_TEST_CASE(functional__avx512__set_uint32__get_uint32_expected)
{
    if (have_avx512())
    {
        const auto xword = set<xint512_t>(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        const auto word0 = get<uint32_t, 0>(xword);
        const auto word1 = get<uint32_t, 1>(xword);
        const auto word2 = get<uint32_t, 2>(xword);
        const auto word3 = get<uint32_t, 3>(xword);
        const auto word4 = get<uint32_t, 4>(xword);
        const auto word5 = get<uint32_t, 5>(xword);
        const auto word6 = get<uint32_t, 6>(xword);
        const auto word7 = get<uint32_t, 7>(xword);
        const auto word8 = get<uint32_t, 8>(xword);
        const auto word9 = get<uint32_t, 9>(xword);
        const auto word10 = get<uint32_t, 10>(xword);
        const auto word11 = get<uint32_t, 11>(xword);
        const auto word12 = get<uint32_t, 12>(xword);
        const auto word13 = get<uint32_t, 13>(xword);
        const auto word14 = get<uint32_t, 14>(xword);
        const auto word15 = get<uint32_t, 15>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u32);
        BOOST_CHECK_EQUAL(word1, 1_u32);
        BOOST_CHECK_EQUAL(word2, 2_u32);
        BOOST_CHECK_EQUAL(word3, 3_u32);
        BOOST_CHECK_EQUAL(word4, 4_u32);
        BOOST_CHECK_EQUAL(word5, 5_u32);
        BOOST_CHECK_EQUAL(word6, 6_u32);
        BOOST_CHECK_EQUAL(word7, 7_u32);
        BOOST_CHECK_EQUAL(word8, 8_u32);
        BOOST_CHECK_EQUAL(word9, 9_u32);
        BOOST_CHECK_EQUAL(word10, 10_u32);
        BOOST_CHECK_EQUAL(word11, 11_u32);
        BOOST_CHECK_EQUAL(word12, 12_u32);
        BOOST_CHECK_EQUAL(word13, 13_u32);
        BOOST_CHECK_EQUAL(word14, 14_u32);
        BOOST_CHECK_EQUAL(word15, 15_u32);
    }
}
#endif


#if defined(HAVE_SSE4)
BOOST_AUTO_TEST_CASE(functional__sse4_set_uint64__get_uint64_expected)
{
    if (have_sse41())
    {
        const auto xword = set<xint128_t>(0, 1);
        const auto word0 = get<uint64_t, 0>(xword);
        const auto word1 = get<uint64_t, 1>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u64);
        BOOST_CHECK_EQUAL(word1, 1_u64);
    }
}
#endif

#if defined(HAVE_AVX2)
BOOST_AUTO_TEST_CASE(functional__avx2__set_uint64__get_uint64_expected)
{
    if (have_avx2())
    {
        const auto xword = set<xint256_t>(0, 1, 2, 3);
        const auto word0 = get<uint64_t, 0>(xword);
        const auto word1 = get<uint64_t, 1>(xword);
        const auto word2 = get<uint64_t, 2>(xword);
        const auto word3 = get<uint64_t, 3>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u64);
        BOOST_CHECK_EQUAL(word1, 1_u64);
        BOOST_CHECK_EQUAL(word2, 2_u64);
        BOOST_CHECK_EQUAL(word3, 3_u64);
    }
}
#endif

#if defined(HAVE_AVX512)
BOOST_AUTO_TEST_CASE(functional__avx512__set_uint64__get_uint64_expected)
{
    if (have_avx512())
    {
        const auto xword = set<xint512_t>(0, 1, 2, 3, 4, 5, 6, 7);
        const auto word0 = get<uint64_t, 0>(xword);
        const auto word1 = get<uint64_t, 1>(xword);
        const auto word2 = get<uint64_t, 2>(xword);
        const auto word3 = get<uint64_t, 3>(xword);
        const auto word4 = get<uint64_t, 4>(xword);
        const auto word5 = get<uint64_t, 5>(xword);
        const auto word6 = get<uint64_t, 6>(xword);
        const auto word7 = get<uint64_t, 7>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u64);
        BOOST_CHECK_EQUAL(word1, 1_u64);
        BOOST_CHECK_EQUAL(word2, 2_u64);
        BOOST_CHECK_EQUAL(word3, 3_u64);
        BOOST_CHECK_EQUAL(word4, 4_u64);
        BOOST_CHECK_EQUAL(word5, 5_u64);
        BOOST_CHECK_EQUAL(word6, 6_u64);
        BOOST_CHECK_EQUAL(word7, 7_u64);
    }
}
#endif
BOOST_AUTO_TEST_SUITE_END()
