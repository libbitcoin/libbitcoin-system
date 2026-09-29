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

// actual registers

struct registers
{
    bool valid{};
    uint32_t eax{};
    uint32_t ebx{};
    uint32_t ecx{};
    uint32_t edx{};
};

static registers get_registers(uint32_t leaf, uint32_t subleaf) NOEXCEPT
{
    registers out{};
    out.valid = get_cpu(out.eax, out.ebx, out.ecx, out.edx, leaf, subleaf);
    return out;
}

static bool get_avx(const registers& cpu1) NOEXCEPT
{
    return cpu1.valid
        && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit)
        && get_right(cpu1.ecx, cpu1_0::xsave_ecx_bit)
        && get_right(cpu1.ecx, cpu1_0::avx_ecx_bit);
}

static bool get_xcr_avx() NOEXCEPT
{
    uint64_t extended{};
    return get_xcr(extended, xcr0::feature)
        && get_right(extended, xcr0::sse_bit)
        && get_right(extended, xcr0::avx_bit);
}

static bool get_xcr_avx512() NOEXCEPT
{
    uint64_t extended{};
    return get_xcr_avx()
        && get_xcr(extended, xcr0::feature)
        && get_right(extended, xcr0::opmask_bit)
        && get_right(extended, xcr0::zmm_upper_bit)
        && get_right(extended, xcr0::zmm_high_bit);
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto avx512f = get_right(cpu7.ebx, cpu7_0::avx512f_ebx_bit);
    const auto avx512bw = get_right(cpu7.ebx, cpu7_0::avx512bw_ebx_bit);
    const auto avx512vl = get_right(cpu7.ebx, cpu7_0::avx512vl_ebx_bit);
    const auto expected = get_avx(cpu1) && get_xcr_avx512() && avx2;
    BOOST_CHECK_EQUAL(expected && avx512f && avx512bw && avx512vl && !is_avx512_low(), try_avx512());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__is_avx512_low__always__match)
{
    const auto cpu0 = get_registers(cpu0_0::leaf, cpu0_0::subleaf);
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto genu = cpu0.ebx == cpu0_0::intel_ebx;
    const auto inei = cpu0.edx == cpu0_0::intel_edx;
    const auto ntel = cpu0.ecx == cpu0_0::intel_ecx;
    const auto intel = cpu0.valid && genu && inei && ntel;
    BOOST_CHECK_EQUAL(intel && cpu1.valid && is_throttled(cpu1.eax), is_avx512_low());
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
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    BOOST_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2, try_avx2());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_sse41__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    BOOST_CHECK_EQUAL(cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit), try_sse41());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_shani__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto sse41 = cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit);
    const auto shani = cpu7.valid && get_right(cpu7.ebx, cpu7_0::shani_ebx_bit);
    BOOST_CHECK_EQUAL(sse41 && shani, try_shani());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512ifma__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto avx512f = get_right(cpu7.ebx, cpu7_0::avx512f_ebx_bit);
    const auto avx512vl = get_right(cpu7.ebx, cpu7_0::avx512vl_ebx_bit);
    const auto avx512ifma = get_right(cpu7.ebx, cpu7_0::avx512ifma_ebx_bit);
    const auto expected = get_avx(cpu1) && get_xcr_avx512() && avx2;
    BOOST_CHECK_EQUAL(expected && avx512f && avx512vl && avx512ifma, try_avx512ifma());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_sha512__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto cpu71 = get_registers(cpu7_1::leaf, cpu7_1::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto leaf71 = cpu7.eax >= cpu7_1::subleaf && cpu71.valid;
    const auto sha512 = get_right(cpu71.eax, cpu7_1::sha512_eax_bit);
    BOOST_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2 && leaf71 && sha512, try_sha512());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avxifma__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto cpu71 = get_registers(cpu7_1::leaf, cpu7_1::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto leaf71 = cpu7.eax >= cpu7_1::subleaf && cpu71.valid;
    const auto avxifma = get_right(cpu71.eax, cpu7_1::avxifma_eax_bit);
    BOOST_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2 && leaf71 && avxifma, try_avxifma());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_aesni__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto sse41 = cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit);
    const auto pclmulqdq = get_right(cpu1.ecx, cpu1_0::pclmulqdq_ecx_bit);
    const auto aes = get_right(cpu1.ecx, cpu1_0::aes_ecx_bit);
    BOOST_CHECK_EQUAL(sse41 && pclmulqdq && aes, try_aesni());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_vaes__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto pclmulqdq = get_right(cpu1.ecx, cpu1_0::pclmulqdq_ecx_bit);
    const auto aes = get_right(cpu1.ecx, cpu1_0::aes_ecx_bit);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto vaes = get_right(cpu7.ecx, cpu7_0::vaes_ecx_bit);
    const auto vpclmulqdq = get_right(cpu7.ecx, cpu7_0::vpclmulqdq_ecx_bit);
    const auto expected = get_avx(cpu1) && pclmulqdq && aes && get_xcr_avx() && avx2;
    BOOST_CHECK_EQUAL(expected && vaes && vpclmulqdq, try_vaes());
}

// faked registers

static bool none_cpu(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t,
    uint32_t) NOEXCEPT
{
    return false;
}

static bool all_cpu(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t, uint32_t) NOEXCEPT
{
    a = b = c = d = max_uint32;
    return true;
}

// An Intel Skylake-SP, with all features.
static bool throttled_cpu(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t) NOEXCEPT
{
    a = b = c = d = max_uint32;
    if (leaf == cpu0_0::leaf)
    {
        b = cpu0_0::intel_ebx;
        d = cpu0_0::intel_edx;
        c = cpu0_0::intel_ecx;
    }
    else if (leaf == cpu1_0::leaf)
    {
        a = cpu1_0::skylake_server;
    }

    return true;
}

static bool all_xcr(uint64_t& value, uint32_t) NOEXCEPT
{
    value = max_uint64;
    return true;
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try__faked_none__false)
{
    BOOST_CHECK(!is_avx512_low<none_cpu>());
    BOOST_CHECK(!try_shani<none_cpu>());
    BOOST_CHECK(!try_sha512<none_cpu, all_xcr>());
    BOOST_CHECK(!try_avx512<none_cpu, all_xcr>());
    BOOST_CHECK(!try_avx512ifma<none_cpu, all_xcr>());
    BOOST_CHECK(!try_avxifma<none_cpu, all_xcr>());
    BOOST_CHECK(!try_avx2<none_cpu, all_xcr>());
    BOOST_CHECK(!try_sse41<none_cpu>());
    BOOST_CHECK(!try_aesni<none_cpu>());
    BOOST_CHECK(!try_vaes<none_cpu, all_xcr>());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try__faked_all__true)
{
    BOOST_CHECK(!is_avx512_low<all_cpu>());
    BOOST_CHECK(try_shani<all_cpu>());
    BOOST_CHECK(try_sha512<all_cpu, all_xcr>());
    BOOST_CHECK(try_avx512<all_cpu, all_xcr>());
    BOOST_CHECK(try_avx512ifma<all_cpu, all_xcr>());
    BOOST_CHECK(try_avxifma<all_cpu, all_xcr>());
    BOOST_CHECK(try_avx2<all_cpu, all_xcr>());
    BOOST_CHECK(try_sse41<all_cpu>());
    BOOST_CHECK(try_aesni<all_cpu>());
    BOOST_CHECK(try_vaes<all_cpu, all_xcr>());
}

BOOST_AUTO_TEST_CASE(intrinsics_detection__try_avx512__faked_throttled__false)
{
    BOOST_CHECK(is_avx512_low<throttled_cpu>());
    BOOST_CHECK(!try_avx512<throttled_cpu, all_xcr>());
    BOOST_CHECK(try_avx512ifma<throttled_cpu, all_xcr>());
}

// try_neon
// try_crypto
// try_sha3

BOOST_AUTO_TEST_SUITE_END()
