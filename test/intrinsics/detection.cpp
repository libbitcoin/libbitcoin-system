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

BC_AUTO_TEST_SUITE(intrinsics_detection_tests)

BC_AUTO_TEST_CASE(intrinsics_detection__get_cpu__highest_leaf__xcpu)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BC_CHECK_EQUAL(get_cpu(eax, ebx, ecx, edx, cpu0_0::leaf, cpu0_0::subleaf), have_xcpu);
}

BC_AUTO_TEST_CASE(intrinsics_detection__get_cpu__above_highest_leaf__false)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BC_CHECK(!get_cpu(eax, ebx, ecx, edx, 0x7fffffff, 0));
    BC_CHECK(!get_cpu(eax, ebx, ecx, edx, 0xffffffff, 0));
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

BC_AUTO_TEST_CASE(intrinsics_detection__try_avx512__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto avx512f = get_right(cpu7.ebx, cpu7_0::avx512f_ebx_bit);
    const auto avx512bw = get_right(cpu7.ebx, cpu7_0::avx512bw_ebx_bit);
    const auto avx512vl = get_right(cpu7.ebx, cpu7_0::avx512vl_ebx_bit);
    const auto expected = get_avx(cpu1) && get_xcr_avx512() && avx2;
    BC_CHECK_EQUAL(expected && avx512f && avx512bw && avx512vl && !is_avx512_low(), try_avx512());
}

BC_AUTO_TEST_CASE(intrinsics_detection__is_avx512_low__always__match)
{
    const auto cpu0 = get_registers(cpu0_0::leaf, cpu0_0::subleaf);
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto genu = cpu0.ebx == cpu0_0::intel_ebx;
    const auto inei = cpu0.edx == cpu0_0::intel_edx;
    const auto ntel = cpu0.ecx == cpu0_0::intel_ecx;
    const auto intel = cpu0.valid && genu && inei && ntel;
    BC_CHECK_EQUAL(intel && cpu1.valid && is_throttled(cpu1.eax), is_avx512_low());
}

BC_AUTO_TEST_CASE(intrinsics_detection__is_throttled__signatures__expected)
{
    BC_CHECK(is_throttled(0x00050654));
    BC_CHECK(is_throttled(0x00050657));
    BC_CHECK(is_throttled(0x00050671));
    BC_CHECK(is_throttled(0x00080650));
    BC_CHECK(!is_throttled(0x000606a6));
    BC_CHECK(!is_throttled(0x00a60f12));
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_avx2__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    BC_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2, try_avx2());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_sse41__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    BC_CHECK_EQUAL(cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit), try_sse41());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_shani__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto sse41 = cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit);
    const auto shani = cpu7.valid && get_right(cpu7.ebx, cpu7_0::shani_ebx_bit);
    BC_CHECK_EQUAL(sse41 && shani, try_shani());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_avx512ifma__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto avx512f = get_right(cpu7.ebx, cpu7_0::avx512f_ebx_bit);
    const auto avx512vl = get_right(cpu7.ebx, cpu7_0::avx512vl_ebx_bit);
    const auto avx512ifma = get_right(cpu7.ebx, cpu7_0::avx512ifma_ebx_bit);
    const auto expected = get_avx(cpu1) && get_xcr_avx512() && avx2;
    BC_CHECK_EQUAL(expected && avx512f && avx512vl && avx512ifma, try_avx512ifma());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_sha512__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto cpu71 = get_registers(cpu7_1::leaf, cpu7_1::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto leaf71 = cpu7.eax >= cpu7_1::subleaf && cpu71.valid;
    const auto sha512 = get_right(cpu71.eax, cpu7_1::sha512_eax_bit);
    BC_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2 && leaf71 && sha512, try_sha512());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_avxifma__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto cpu71 = get_registers(cpu7_1::leaf, cpu7_1::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto leaf71 = cpu7.eax >= cpu7_1::subleaf && cpu71.valid;
    const auto avxifma = get_right(cpu71.eax, cpu7_1::avxifma_eax_bit);
    BC_CHECK_EQUAL(get_avx(cpu1) && get_xcr_avx() && avx2 && leaf71 && avxifma, try_avxifma());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_aesni__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto sse41 = cpu1.valid && get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit);
    const auto pclmulqdq = get_right(cpu1.ecx, cpu1_0::pclmulqdq_ecx_bit);
    const auto aes = get_right(cpu1.ecx, cpu1_0::aes_ecx_bit);
    BC_CHECK_EQUAL(sse41 && pclmulqdq && aes, try_aesni());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_vaes__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto pclmulqdq = get_right(cpu1.ecx, cpu1_0::pclmulqdq_ecx_bit);
    const auto aes = get_right(cpu1.ecx, cpu1_0::aes_ecx_bit);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto vaes = get_right(cpu7.ecx, cpu7_0::vaes_ecx_bit);
    const auto vpclmulqdq = get_right(cpu7.ecx, cpu7_0::vpclmulqdq_ecx_bit);
    const auto expected = get_avx(cpu1) && pclmulqdq && aes && get_xcr_avx() && avx2;
    BC_CHECK_EQUAL(expected && vaes && vpclmulqdq, try_vaes());
}

BC_AUTO_TEST_CASE(intrinsics_detection__is_avx512__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto avx2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::avx2_ebx_bit);
    const auto avx512f = get_right(cpu7.ebx, cpu7_0::avx512f_ebx_bit);
    const auto avx512bw = get_right(cpu7.ebx, cpu7_0::avx512bw_ebx_bit);
    const auto avx512vl = get_right(cpu7.ebx, cpu7_0::avx512vl_ebx_bit);
    const auto expected = get_avx(cpu1) && get_xcr_avx512() && avx2;
    BC_CHECK_EQUAL(expected && avx512f && avx512bw && avx512vl, is_avx512());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_x86_64_v2__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto ext1 = get_registers(cpu80000001_0::leaf, cpu80000001_0::subleaf);
    const auto sse3 = get_right(cpu1.ecx, cpu1_0::sse3_ecx_bit);
    const auto ssse3 = get_right(cpu1.ecx, cpu1_0::ssse3_ecx_bit);
    const auto cx16 = get_right(cpu1.ecx, cpu1_0::cx16_ecx_bit);
    const auto sse41 = get_right(cpu1.ecx, cpu1_0::sse41_ecx_bit);
    const auto sse42 = get_right(cpu1.ecx, cpu1_0::sse42_ecx_bit);
    const auto popcnt = get_right(cpu1.ecx, cpu1_0::popcnt_ecx_bit);
    const auto lahf = ext1.valid && get_right(ext1.ecx, cpu80000001_0::lahf_ecx_bit);
    BC_CHECK_EQUAL(cpu1.valid && sse3 && ssse3 && cx16 && sse41 && sse42 && popcnt && lahf, try_x86_64_v2());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_x86_64_v3__always__match)
{
    const auto cpu1 = get_registers(cpu1_0::leaf, cpu1_0::subleaf);
    const auto cpu7 = get_registers(cpu7_0::leaf, cpu7_0::subleaf);
    const auto ext1 = get_registers(cpu80000001_0::leaf, cpu80000001_0::subleaf);
    const auto fma = get_right(cpu1.ecx, cpu1_0::fma_ecx_bit);
    const auto movbe = get_right(cpu1.ecx, cpu1_0::movbe_ecx_bit);
    const auto f16c = get_right(cpu1.ecx, cpu1_0::f16c_ecx_bit);
    const auto bmi1 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::bmi1_ebx_bit);
    const auto bmi2 = cpu7.valid && get_right(cpu7.ebx, cpu7_0::bmi2_ebx_bit);
    const auto lzcnt = ext1.valid && get_right(ext1.ecx, cpu80000001_0::lzcnt_ecx_bit);
    const auto expected = try_x86_64_v2() && try_avx2() && cpu1.valid;
    BC_CHECK_EQUAL(expected && fma && movbe && f16c && bmi1 && bmi2 && lzcnt, try_x86_64_v3());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_compiled__always__true)
{
    BC_CHECK(try_compiled());
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

BC_AUTO_TEST_CASE(intrinsics_detection__try__faked_none__false)
{
    BC_CHECK(!is_avx512_low<none_cpu>());
    BC_CHECK(!try_shani<none_cpu>());
    BC_CHECK((!try_sha512<none_cpu, all_xcr>()));
    BC_CHECK((!is_avx512<none_cpu, all_xcr>()));
    BC_CHECK((!try_avx512<none_cpu, all_xcr>()));
    BC_CHECK((!try_avx512ifma<none_cpu, all_xcr>()));
    BC_CHECK((!try_avxifma<none_cpu, all_xcr>()));
    BC_CHECK((!try_avx2<none_cpu, all_xcr>()));
    BC_CHECK(!try_sse41<none_cpu>());
    BC_CHECK(!try_aesni<none_cpu>());
    BC_CHECK((!try_vaes<none_cpu, all_xcr>()));
    BC_CHECK(!try_x86_64_v2<none_cpu>());
    BC_CHECK((!try_x86_64_v3<none_cpu, all_xcr>()));
    BC_CHECK((!try_x86_64_v4<none_cpu, all_xcr>()));
}

BC_AUTO_TEST_CASE(intrinsics_detection__try__faked_all__true)
{
    BC_CHECK(!is_avx512_low<all_cpu>());
    BC_CHECK(try_shani<all_cpu>());
    BC_CHECK((try_sha512<all_cpu, all_xcr>()));
    BC_CHECK((is_avx512<all_cpu, all_xcr>()));
    BC_CHECK((try_avx512<all_cpu, all_xcr>()));
    BC_CHECK((try_avx512ifma<all_cpu, all_xcr>()));
    BC_CHECK((try_avxifma<all_cpu, all_xcr>()));
    BC_CHECK((try_avx2<all_cpu, all_xcr>()));
    BC_CHECK(try_sse41<all_cpu>());
    BC_CHECK(try_aesni<all_cpu>());
    BC_CHECK((try_vaes<all_cpu, all_xcr>()));
    BC_CHECK(try_x86_64_v2<all_cpu>());
    BC_CHECK((try_x86_64_v3<all_cpu, all_xcr>()));
    BC_CHECK((try_x86_64_v4<all_cpu, all_xcr>()));
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_avx512__faked_throttled__false)
{
    BC_CHECK(is_avx512_low<throttled_cpu>());
    BC_CHECK((is_avx512<throttled_cpu, all_xcr>()));
    BC_CHECK((!try_avx512<throttled_cpu, all_xcr>()));
    BC_CHECK((try_avx512ifma<throttled_cpu, all_xcr>()));
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon__always__match)
{
    BC_CHECK_EQUAL(get_arm(arm_feature::neon), try_neon());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon_sha2__always__match)
{
    BC_CHECK_EQUAL(get_arm(arm_feature::sha1) && get_arm(arm_feature::sha256), try_neon_sha2());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon_aes__always__match)
{
    BC_CHECK_EQUAL(get_arm(arm_feature::aes) && get_arm(arm_feature::pmull), try_neon_aes());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon_sha3__always__match)
{
    BC_CHECK_EQUAL(get_arm(arm_feature::sha3) && get_arm(arm_feature::sha512), try_neon_sha3());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_lse__always__match)
{
    BC_CHECK_EQUAL(get_arm(arm_feature::atomics), try_lse());
}

static bool none_arm(arm_feature) NOEXCEPT
{
    return false;
}

static bool all_arm(arm_feature) NOEXCEPT
{
    return true;
}

// Each pair lacks its second feature.
static bool half_arm(arm_feature feature) NOEXCEPT
{
    return feature == arm_feature::sha1
        || feature == arm_feature::aes
        || feature == arm_feature::sha3;
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon__faked_none__false)
{
    BC_CHECK(!try_neon<none_arm>());
    BC_CHECK(!try_neon_sha2<none_arm>());
    BC_CHECK(!try_neon_aes<none_arm>());
    BC_CHECK(!try_neon_sha3<none_arm>());
    BC_CHECK(!try_lse<none_arm>());
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon__faked_all__true)
{
    BC_CHECK(try_neon<all_arm>());
    BC_CHECK(try_neon_sha2<all_arm>());
    BC_CHECK(try_neon_aes<all_arm>());
    BC_CHECK(try_neon_sha3<all_arm>());
    BC_CHECK(try_lse<all_arm>());
    BC_CHECK((try_compiled<all_cpu, all_xcr, all_arm>()));
}

BC_AUTO_TEST_CASE(intrinsics_detection__try_neon__faked_half__false)
{
    BC_CHECK(!try_neon<half_arm>());
    BC_CHECK(!try_neon_sha2<half_arm>());
    BC_CHECK(!try_neon_aes<half_arm>());
    BC_CHECK(!try_neon_sha3<half_arm>());
}

// build tags

static std::string suffix(const build_tags& tags) NOEXCEPT
{
    return join(build_suffix(tags), "-");
}

BC_AUTO_TEST_CASE(intrinsics_detection__detected_tags__faked_none__none)
{
    const auto tags = detected_tags<none_cpu, all_xcr, none_arm>();
    BC_CHECK(!tags.sse41);
    BC_CHECK(!tags.avx2);
    BC_CHECK(!tags.avx512);
    BC_CHECK(!tags.avxifma);
    BC_CHECK(!tags.avx512ifma);
    BC_CHECK(!tags.shani);
    BC_CHECK(!tags.sha2);
    BC_CHECK(!tags.sha3);
}

BC_AUTO_TEST_CASE(intrinsics_detection__detected_tags__faked_all__all)
{
    const auto tags = detected_tags<all_cpu, all_xcr, all_arm>();
    BC_CHECK(tags.sse41);
    BC_CHECK(tags.avx2);
    BC_CHECK(tags.avx512);
    BC_CHECK(tags.avxifma);
    BC_CHECK(tags.avx512ifma);
    BC_CHECK(tags.shani);
    BC_CHECK(tags.sha2);
    BC_CHECK(tags.sha3);
}

BC_AUTO_TEST_CASE(intrinsics_detection__detected_tags__faked_throttled__no_avx512)
{
    const auto tags = detected_tags<throttled_cpu, all_xcr, none_arm>();
    BC_CHECK(tags.sse41);
    BC_CHECK(tags.avx2);
    BC_CHECK(!tags.avx512);
    BC_CHECK(tags.avxifma);
    BC_CHECK(!tags.avx512ifma);
    BC_CHECK(tags.shani);
}

BC_AUTO_TEST_CASE(intrinsics_detection__detected_tags__faked_half_arm__none)
{
    const auto tags = detected_tags<none_cpu, all_xcr, half_arm>();
    BC_CHECK(!tags.sha2);
    BC_CHECK(!tags.sha3);
}

BC_AUTO_TEST_CASE(intrinsics_detection__build_suffix__x64__canonical)
{
    BC_CHECK_EQUAL(suffix({}), "");
    BC_CHECK_EQUAL(suffix({ .sse41 = true }), "sse41");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .shani = true }), "shani");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true }), "avx2");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .shani = true }), "avx2-shani");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avxifma = true, .shani = true }), "avxifma");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avx512 = true }), "avx512");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avx512 = true, .shani = true }), "avx512-shani");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avx512 = true, .avxifma = true, .shani = true }), "avx512-avxifma");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avx512 = true, .avx512ifma = true, .shani = true }), "avx512ifma");
    BC_CHECK_EQUAL(suffix({ .sse41 = true, .avx2 = true, .avx512 = true, .avxifma = true, .avx512ifma = true, .shani = true }), "avx512ifma");
}

BC_AUTO_TEST_CASE(intrinsics_detection__build_suffix__arm64__canonical)
{
    BC_CHECK_EQUAL(suffix({ .sha2 = true }), "sha2");
    BC_CHECK_EQUAL(suffix({ .sha2 = true, .sha3 = true }), "sha3");
}

BC_AUTO_TEST_SUITE_END()
