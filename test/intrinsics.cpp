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
#include "test.hpp"

BOOST_AUTO_TEST_SUITE(intrinsics_tests)

BOOST_AUTO_TEST_CASE(intrinsics__try_shani__always__false)
{
    const auto tryit = try_shani();

#if defined(HAVE_X64)
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse4_ecx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::shani_ebx_bit), tryit);
#else
    BOOST_CHECK(!tryit);
#endif
}

BOOST_AUTO_TEST_CASE(intrinsics__try_avx2__always__true)
{
    const auto tryit = try_avx2();

#if defined(HAVE_X64)
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse4_ecx_bit) &&
        get_right(ecx, cpu1_0::xsave_ecx_bit) &&
        get_right(ecx, cpu1_0::avx_ecx_bit) &&
        get_xcr(extended, xcr0::feature) &&
        get_right(extended, xcr0::sse_bit) &&
        get_right(extended, xcr0::avx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::avx2_ebx_bit), tryit);
#else
    BOOST_CHECK(!tryit);
#endif
}

// Intrinsics fail on X32 when try_sse41 true.
BOOST_AUTO_TEST_CASE(intrinsics__try_sse41__always__expected)
{
    const auto tryit = try_sse41();

#if defined(HAVE_X64)
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse4_ecx_bit), tryit);
#else
    BOOST_CHECK(!tryit);
#endif
}

BOOST_AUTO_TEST_CASE(intrinsics__try_sse4__always__expected)
{
    const auto tryit = try_sse41a();

#if defined(HAVE_X64) && defined(HAVE_XASSEMBLY)
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse4_ecx_bit), tryit);
#else
    BOOST_CHECK(!tryit);
#endif
}

BOOST_AUTO_TEST_CASE(intrinsics__try_neon__always__expected)
{
    constexpr auto tryit = try_neon();

#if defined(HAVE_NEON)
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse4_ecx_bit), tryit);
#else
    BOOST_CHECK(!tryit);
#endif
}

// This currently assumes always have sse41/avx2 on X64 (and never on X32),
// never have shani/neon, and only have sse41a on MSX/X64. Update with CI.
BOOST_AUTO_TEST_CASE(intrinsics__have_all__always__expected)
{
#if defined(HAVE_X64)
    BOOST_REQUIRE(have_avx2());
    BOOST_REQUIRE(have_sse41());
    #if defined (HAVE_XASSEMBLY)
        BOOST_REQUIRE(have_sse41a());
    #endif
#else
    BOOST_REQUIRE(!have_avx2());
    BOOST_REQUIRE(!have_sse41());
    BOOST_REQUIRE(!have_sse41a());
#endif
    BOOST_REQUIRE(!have_shani());
    BOOST_REQUIRE(!have_neon());
}

BOOST_AUTO_TEST_SUITE_END()
