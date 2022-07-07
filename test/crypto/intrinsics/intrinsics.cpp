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
/// DELETECSTDINT

BOOST_AUTO_TEST_SUITE(intrinsics_tests)

using namespace bc::system::intrinsics;

// "try" tests not reliable across platforms.
// These have all been moved into cpp (no longer public).

////BOOST_AUTO_TEST_CASE(intrinsics__try_avx2__always__expected)
////{
////    uint64_t extended;
////    uint32_t eax, ebx, ecx, edx;
////    BOOST_REQUIRE(cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf));
////    BOOST_REQUIRE(get_right(ecx, cpu1_0::sse4_ecx_bit));
////    BOOST_REQUIRE(get_right(ecx, cpu1_0::xsave_ecx_bit));
////    BOOST_REQUIRE(get_right(ecx, cpu1_0::avx_ecx_bit));
////    BOOST_REQUIRE(xgetbv(extended, xcr0::feature));
////    BOOST_REQUIRE(get_right(extended, xcr0::sse_bit));
////    BOOST_REQUIRE(get_right(extended, xcr0::avx_bit));
////    BOOST_REQUIRE(cpuid_ex(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf));
////    BOOST_REQUIRE(get_right(ebx, cpu7_0::avx2_ebx_bit));
////}
////
////BOOST_AUTO_TEST_CASE(intrinsics__try_sse41__always__expected)
////{
////    uint32_t eax, ebx, ecx, edx;
////    BOOST_REQUIRE(cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf));
////    BOOST_REQUIRE(get_right(ecx, cpu1_0::sse4_ecx_bit));
////}
////
////BOOST_AUTO_TEST_CASE(intrinsics__try_shani__always__expected)
////{
////    uint32_t eax, ebx, ecx, edx;
////    BOOST_REQUIRE(cpuid_ex(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf));
////    BOOST_REQUIRE(get_right(ecx, cpu1_0::sse4_ecx_bit));
////    BOOST_REQUIRE(cpuid_ex(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf));
////    BOOST_REQUIRE(get_right(ebx, cpu7_0::shani_ebx_bit));
////}

// satoshi test case
BOOST_AUTO_TEST_CASE(intrinsics__sha256_paired_double__vs_sha256x2__same)
{
    for (int i = 0; i <= 32; ++i)
    {
        uint8_t in[64 * 32]{};
        uint8_t out1[32 * 32]{};
        uint8_t out2[32 * 32]{};
        const auto to = [&](int j) { return out1 + 32 * j; };
        const auto from = [&](int j) { return in + 64 * j; };

        // random fill
        for (int j = 0; j < 64 * i; ++j)
            in[j] = 42;

        // streaming double hash
        for (int j = 0; j < i; ++j)
        {
            hash::sha256x2::copy sink({ to(j), to(j) + 32 });
            sink.write_bytes({ from(j), from(j) + 64 });
            sink.flush();
        }

        // in place double hash
        intrinsics::sha256_paired_double(out2, in, i);
        BOOST_REQUIRE_EQUAL(memcmp(out1, out2, 32 * i), 0);
    }
}
BOOST_AUTO_TEST_SUITE_END()
