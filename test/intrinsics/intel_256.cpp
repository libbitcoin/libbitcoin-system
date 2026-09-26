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
#include "../test.hpp"

#if defined(HAVE_AVX2)

BOOST_AUTO_TEST_SUITE(intrinsics_intrinsics__inteltests)

// set/get
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__set32__get_expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::set<xint256_t>(0, 1, 2, 3, 4, 5, 6, 7);
        const auto word0 = f::get<uint32_t, 0>(xword);
        const auto word1 = f::get<uint32_t, 1>(xword);
        const auto word2 = f::get<uint32_t, 2>(xword);
        const auto word3 = f::get<uint32_t, 3>(xword);
        const auto word4 = f::get<uint32_t, 4>(xword);
        const auto word5 = f::get<uint32_t, 5>(xword);
        const auto word6 = f::get<uint32_t, 6>(xword);
        const auto word7 = f::get<uint32_t, 7>(xword);
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

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__set64__get_expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::set<xint256_t>(0, 1, 2, 3);
        const auto word0 = f::get<uint64_t, 0>(xword);
        const auto word1 = f::get<uint64_t, 1>(xword);
        const auto word2 = f::get<uint64_t, 2>(xword);
        const auto word3 = f::get<uint64_t, 3>(xword);
        BOOST_CHECK_EQUAL(word0, 0_u64);
        BOOST_CHECK_EQUAL(word1, 1_u64);
        BOOST_CHECK_EQUAL(word2, 2_u64);
        BOOST_CHECK_EQUAL(word3, 3_u64);
    }
}

// byteswap
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__byteswap32__expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::byteswap<uint32_t>(f::set<xint256_t>(
            0x00000001, 0x00000002, 0x00000003, 0x00000004,
            0x00000005, 0x00000006, 0x00000007, 0x00000008));
        const auto word0 = f::get<uint32_t, 0>(xword);
        const auto word1 = f::get<uint32_t, 1>(xword);
        const auto word2 = f::get<uint32_t, 2>(xword);
        const auto word3 = f::get<uint32_t, 3>(xword);
        const auto word4 = f::get<uint32_t, 4>(xword);
        const auto word5 = f::get<uint32_t, 5>(xword);
        const auto word6 = f::get<uint32_t, 6>(xword);
        const auto word7 = f::get<uint32_t, 7>(xword);
        BOOST_CHECK_EQUAL(word0, 0x01000000_u32);
        BOOST_CHECK_EQUAL(word1, 0x02000000_u32);
        BOOST_CHECK_EQUAL(word2, 0x03000000_u32);
        BOOST_CHECK_EQUAL(word3, 0x04000000_u32);
        BOOST_CHECK_EQUAL(word4, 0x05000000_u32);
        BOOST_CHECK_EQUAL(word5, 0x06000000_u32);
        BOOST_CHECK_EQUAL(word6, 0x07000000_u32);
        BOOST_CHECK_EQUAL(word7, 0x08000000_u32);
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__byteswap64__expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::byteswap<uint64_t>(f::set<xint256_t>(
            0x0000000000000001, 0x0000000000000002,
            0x0000000000000003, 0x0000000000000004));
        const auto word0 = f::get<uint64_t, 0>(xword);
        const auto word1 = f::get<uint64_t, 1>(xword);
        const auto word2 = f::get<uint64_t, 2>(xword);
        const auto word3 = f::get<uint64_t, 3>(xword);
        BOOST_CHECK_EQUAL(word0, 0x0100000000000000_u64);
        BOOST_CHECK_EQUAL(word1, 0x0200000000000000_u64);
        BOOST_CHECK_EQUAL(word2, 0x0300000000000000_u64);
        BOOST_CHECK_EQUAL(word3, 0x0400000000000000_u64);
    }
}

// compare/select
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__andnot__expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::andnot(f::set<xint256_t>(0xf0f0_u64, 0xffff_u64, 0_u64, max_uint64), f::set<xint256_t>(0xffff_u64, 0x0f0f_u64, 42_u64, 42_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), 0x0f0f_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 0x0000_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), 42_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 0_u64);
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__eq64__expected)
{
    if constexpr (have_256)
    {
        const auto xword = f::eq<64>(f::set<xint256_t>(1_u64, 2_u64, 3_u64, 4_u64), f::set<xint256_t>(1_u64, 3_u64, 3_u64, 0_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), max_uint64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 0_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), max_uint64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 0_u64);
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__select__expected)
{
    if constexpr (have_256)
    {
        const auto mask = f::set<xint256_t>(max_uint64, 0_u64, 0_u64, max_uint64);
        const auto xword = f::select(mask, f::set<xint256_t>(10_u64, 20_u64, 30_u64, 40_u64), f::set<xint256_t>(50_u64, 60_u64, 70_u64, 80_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), 10_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 60_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), 70_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 40_u64);
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__any__expected)
{
    if constexpr (have_256)
    {
        BOOST_CHECK(!f::any(f::set<xint256_t>(0_u64, 0_u64, 0_u64, 0_u64)));
        BOOST_CHECK(f::any(f::set<xint256_t>(0_u64, 0_u64, 0_u64, 1_u64)));
        BOOST_CHECK(f::any(f::set<xint256_t>(0x8000000000000000_u64, 0_u64, 0_u64, 0_u64)));
    }
}

// gather
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__gather__expected)
{
    if constexpr (have_256)
    {
        constexpr std_array<uint64_t, 8> table{ 100, 101, 102, 103, 104, 105, 106, 107 };
        const auto xword = f::gather(table.data(), f::set<xint256_t>(3_u64, 0_u64, 7_u64, 5_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), 103_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 100_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), 107_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 105_u64);
    }
}

// madd52
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__madd52lo__expected)
{
    if constexpr (have_256)
    {
        constexpr auto max52 = 0x000fffffffffffff_u64;
        constexpr auto high = 0x8000000000000003_u64;
        const auto xword = f::madd52lo<64>(f::set<xint256_t>(7_u64, 1_u64, 0_u64, max_uint64), f::set<xint256_t>(max52, high, 1_u64, 1_u64), f::set<xint256_t>(max52, 5_u64, 0_u64, 2_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), 8_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 16_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), 0_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 1_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), f::madd52lo(7_u64, max52, max52));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), f::madd52lo(1_u64, high, 5_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), f::madd52lo(max_uint64, 1_u64, 2_u64));
    }
}

BOOST_AUTO_TEST_CASE(intrinsics__intel_256__madd52hi__expected)
{
    if constexpr (have_256)
    {
        constexpr auto max52 = 0x000fffffffffffff_u64;
        constexpr auto high = 0x8000000000000003_u64;
        const auto xword = f::madd52hi<64>(f::set<xint256_t>(7_u64, 1_u64, 0_u64, 3_u64), f::set<xint256_t>(max52, high, 1_u64, max52), f::set<xint256_t>(max52, 5_u64, 0_u64, 2_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), 0x000ffffffffffffe_u64 + 7_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), 1_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 2>(xword)), 0_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), 4_u64);
        BOOST_CHECK_EQUAL((f::get<uint64_t, 0>(xword)), f::madd52hi(7_u64, max52, max52));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 1>(xword)), f::madd52hi(1_u64, high, 5_u64));
        BOOST_CHECK_EQUAL((f::get<uint64_t, 3>(xword)), f::madd52hi(3_u64, max52, 2_u64));
    }
}

BOOST_AUTO_TEST_SUITE_END()

#endif // HAVE_AVX2
