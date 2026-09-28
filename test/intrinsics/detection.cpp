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

BOOST_AUTO_TEST_SUITE(intrinsics_detection_tests)

BOOST_AUTO_TEST_CASE(intrinsics_detection__get_cpu__highest_leaf__xcpu)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf), have_xcpu);
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__get_cpu__above_highest_leaf__false)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK(!get_cpu(eax, ebx, ecx, edx, 0x7fffffff, 0));
    BOOST_CHECK(!get_cpu(eax, ebx, ecx, edx, 0xffffffff, 0));
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_right(extended, xcr0::opmask_bit) && get_right(extended, xcr0::zmm_upper_bit) && get_right(extended, xcr0::zmm_high_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit) && get_right(ebx, cpu7_0::avx512f_ebx_bit) && get_right(ebx, cpu7_0::avx512bw_ebx_bit) && get_right(ebx, cpu7_0::avx512vl_ebx_bit) && !is_avx512_low(), try_avx512());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__is_avx512_low__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf) && ebx == cpu0_0::intel_ebx && edx == cpu0_0::intel_edx && ecx == cpu0_0::intel_ecx && get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && is_throttled(eax), is_avx512_low());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__is_throttled__signatures__expected)
{
    BOOST_CHECK(is_throttled(0x00050654));
    BOOST_CHECK(is_throttled(0x00050657));
    BOOST_CHECK(is_throttled(0x00050671));
    BOOST_CHECK(is_throttled(0x00080650));
    BOOST_CHECK(!is_throttled(0x000606a6));
    BOOST_CHECK(!is_throttled(0x00a60f12));
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx2__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit), try_avx2());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_sse41__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit), try_sse41());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_shani__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::shani_ebx_bit), try_shani());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512ifma__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    uint64_t extended{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_right(extended, xcr0::opmask_bit) && get_right(extended, xcr0::zmm_upper_bit) && get_right(extended, xcr0::zmm_high_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit) && get_right(ebx, cpu7_0::avx512f_ebx_bit) && get_right(ebx, cpu7_0::avx512vl_ebx_bit) && get_right(ebx, cpu7_0::avx512ifma_ebx_bit), try_avx512ifma());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_sha512__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit) && eax >= cpu7_1::subleaf && get_cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf) && get_right(eax, cpu7_1::sha512_eax_bit), try_sha512());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avxifma__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit) && eax >= cpu7_1::subleaf && get_cpu(eax, ebx, ecx, edx, cpu7_1::leaf, cpu7_1::subleaf) && get_right(eax, cpu7_1::avxifma_eax_bit), try_avxifma());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_aesni__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::pclmulqdq_ecx_bit) && get_right(ecx, cpu1_0::aes_ecx_bit), try_aesni());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_vaes__always__match)
{
    uint64_t extended{};
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) && get_right(ecx, cpu1_0::sse41_ecx_bit) && get_right(ecx, cpu1_0::pclmulqdq_ecx_bit) && get_right(ecx, cpu1_0::aes_ecx_bit) && get_right(ecx, cpu1_0::xsave_ecx_bit) && get_right(ecx, cpu1_0::avx_ecx_bit) && get_xcr(extended, xcr0::feature) && get_right(extended, xcr0::sse_bit) && get_right(extended, xcr0::avx_bit) && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) && get_right(ebx, cpu7_0::avx2_ebx_bit) && get_right(ecx, cpu7_0::vaes_ecx_bit) && get_right(ecx, cpu7_0::vpclmulqdq_ecx_bit), try_vaes());
}

// try_neon
// try_crypto
// try_sha3

BOOST_AUTO_TEST_SUITE_END()
