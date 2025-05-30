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
#include "../test.hpp"

#if defined(HAVE_SSE4)

BOOST_AUTO_TEST_SUITE(intrinsics_intel_128_tests)

// set/get
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_128__set32__get_expected)
{
    if constexpr (have_128)
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

BOOST_AUTO_TEST_CASE(intrinsics__intel_128__set64__get_expected)
{
    if constexpr (have_128)
    {
        const auto xword = set<xint128_t>(0, 1);
        const auto word0 = get<uint64_t, 0>(xword);
        const auto word1 = get<uint64_t, 1>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u64);
        BOOST_CHECK_EQUAL(word1, 1_u64);
    }
}

// byteswap
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_128__byteswap32__expected)
{
    if constexpr (have_128)
    {
        const auto xword = byteswap<uint32_t>(set<xint128_t>(
            0x00000001, 0x00000002, 0x00000003, 0x00000004));
        const auto word0 = get<uint32_t, 0>(xword);
        const auto word1 = get<uint32_t, 1>(xword);
        const auto word2 = get<uint32_t, 2>(xword);
        const auto word3 = get<uint32_t, 3>(xword);
        BOOST_CHECK_EQUAL(word0, 0x01000000_u32);
        BOOST_CHECK_EQUAL(word1, 0x02000000_u32);
        BOOST_CHECK_EQUAL(word2, 0x03000000_u32);
        BOOST_CHECK_EQUAL(word3, 0x04000000_u32);
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_128__byteswap64__expected)
{
    if constexpr (have_128)
    {
        const auto xword = byteswap<uint64_t>(set<xint128_t>(
            0x0000000000000001, 0x0000000000000002));
        const auto word0 = get<uint64_t, 0>(xword);
        const auto word1 = get<uint64_t, 1>(xword);
        BOOST_CHECK_EQUAL(word0, 0x0100000000000000_u64);
        BOOST_CHECK_EQUAL(word1, 0x0200000000000000_u64);
    }
}

BOOST_AUTO_TEST_SUITE_END()

#endif // HAVE_SSE4
