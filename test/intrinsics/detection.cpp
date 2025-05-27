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

BOOST_AUTO_TEST_SUITE(intrinsics_detection_tests)

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit) &&
        get_right(ecx, cpu1_0::xsave_ecx_bit) &&
        get_right(ecx, cpu1_0::avx_ecx_bit) &&
        get_xcr(extended, xcr0::feature) &&
        get_right(extended, xcr0::sse_bit) &&
        get_right(extended, xcr0::avx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::avx512bw_ebx_bit), try_avx512());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx2__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit) &&
        get_right(ecx, cpu1_0::xsave_ecx_bit) &&
        get_right(ecx, cpu1_0::avx_ecx_bit) &&
        get_xcr(extended, xcr0::feature) &&
        get_right(extended, xcr0::sse_bit) &&
        get_right(extended, xcr0::avx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::avx2_ebx_bit), try_avx2());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_sse41__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit), try_sse41());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_shani__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::shani_ebx_bit), try_shani());
}

BOOST_AUTO_TEST_SUITE_END()
