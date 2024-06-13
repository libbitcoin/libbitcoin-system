/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(intrinsics_haves_tests)

// Build symbols to constexpr.
// ----------------------------------------------------------------------------

#if defined(HAVE_SSE41)
    static_assert(with_sse41);
#else
    static_assert(!with_sse41);
#endif
#if defined(HAVE_AVX2)
    static_assert(with_avx2);
#else
    static_assert(!with_avx2);
#endif
#if defined(HAVE_AVX512)
    static_assert(with_avx512);
#else
    static_assert(!with_avx512);
#endif
#if defined(HAVE_SHANI)
    static_assert(with_shani);
#else
    static_assert(!with_shani);
#endif
#if defined(HAVE_NEON)
    static_assert(with_neon);
#else
    static_assert(!with_neon);
#endif

// try()
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics_haves__try_avx512__always__match)
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

BOOST_AUTO_TEST_CASE(intrinsics_haves__try_avx2__always__match)
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

BOOST_AUTO_TEST_CASE(intrinsics_haves__try_sse41__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit), try_sse41());
}

BOOST_AUTO_TEST_CASE(intrinsics_haves__try_shani__always__match)
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    BOOST_CHECK_EQUAL(
        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
        get_right(ecx, cpu1_0::sse41_ecx_bit) &&
        get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf) &&
        get_right(ebx, cpu7_0::shani_ebx_bit), try_shani());
}

// Unknown vector.
////BOOST_AUTO_TEST_CASE(intrinsics_haves__try_neon__always__match)
////{
////    uint32_t eax{}, ebx{}, ecx{}, edx{};
////    BOOST_CHECK_EQUAL(
////        get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf) &&
////        get_right(ecx, cpu1_0::sse41_ecx_bit), try_neon());
////}

// have_lanes
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(intrinsics__have_lanes__avx512__expected)
{
    auto have = false;

    have = have_lanes<uint64_t, 8>();
    BOOST_CHECK_EQUAL(have, with_avx512);
    have = have_lanes<uint32_t, 16>();
    BOOST_CHECK_EQUAL(have, with_avx512);
    have = have_lanes<uint16_t, 32>();
    BOOST_CHECK_EQUAL(have, with_avx512);
    have = have_lanes<uint8_t, 64>();
    BOOST_CHECK_EQUAL(have, with_avx512);

    have = have_lanes<uint64_t, 7>();
    BOOST_CHECK(!have);
    have = have_lanes<uint32_t, 15>();
    BOOST_CHECK(!have);
    have = have_lanes<uint16_t, 31>();
    BOOST_CHECK(!have);
    have = have_lanes<uint8_t, 63>();
    BOOST_CHECK(!have);
}

BOOST_AUTO_TEST_CASE(intrinsics__have_lanes__avx2__expected)
{
    auto have = false;

    have = have_lanes<uint64_t, 4>();
    BOOST_CHECK_EQUAL(have, with_avx2);
    have = have_lanes<uint32_t, 8>();
    BOOST_CHECK_EQUAL(have, with_avx2);
    have = have_lanes<uint16_t, 16>();
    BOOST_CHECK_EQUAL(have, with_avx2);
    have = have_lanes<uint8_t, 32>();
    BOOST_CHECK_EQUAL(have, with_avx2);

    have = have_lanes<uint64_t, 3>();
    BOOST_CHECK(!have);
    have = have_lanes<uint32_t, 7>();
    BOOST_CHECK(!have);
    have = have_lanes<uint16_t, 15>();
    BOOST_CHECK(!have);
    have = have_lanes<uint8_t, 31>();
    BOOST_CHECK(!have);
}

BOOST_AUTO_TEST_CASE(intrinsics__have_lanes__sse41__expected)
{
    auto have = false;

    have = have_lanes<uint64_t, 2>();
    BOOST_CHECK_EQUAL(have, with_sse41);
    have = have_lanes<uint32_t, 4>();
    BOOST_CHECK_EQUAL(have, with_sse41);
    have = have_lanes<uint16_t, 8>();
    BOOST_CHECK_EQUAL(have, with_sse41);
    have = have_lanes<uint8_t, 16>();
    BOOST_CHECK_EQUAL(have, with_sse41);

    have = have_lanes<uint64_t, 1>();
    BOOST_CHECK(!have);
    have = have_lanes<uint32_t, 3>();
    BOOST_CHECK(!have);
    have = have_lanes<uint16_t, 7>();
    BOOST_CHECK(!have);
    have = have_lanes<uint8_t, 15>();
    BOOST_CHECK(!have);
}

// is_extended
// ----------------------------------------------------------------------------

// helper
template <typename>
constexpr bool is_defined = true;

// is_extended is true even with mock type.
static_assert(!is_extended<uint32_t>);
static_assert(is_extended<xint128_t>);
static_assert(is_extended<xint256_t>);
static_assert(is_extended<xint512_t>);
static_assert(is_defined<if_extended<xint128_t>>);
static_assert(is_defined<if_extended<xint256_t>>);
static_assert(is_defined<if_extended<xint512_t>>);

// capacity [for extended integers]
// ----------------------------------------------------------------------------

static_assert(capacity<xint128_t, uint8_t>    == 16); // required to fill
static_assert(capacity<xint128_t, uint8_t, 1> == 16); // full
static_assert(capacity<xint128_t, uint8_t, 2> ==  8); // full
static_assert(capacity<xint128_t, uint8_t, 3> ==  5);
static_assert(capacity<xint128_t, uint8_t, 4> ==  4); // full
static_assert(capacity<xint128_t, uint8_t, 5> ==  3);
static_assert(capacity<xint128_t, uint8_t, 6> ==  2);
static_assert(capacity<xint128_t, uint8_t, 7> ==  2);
static_assert(capacity<xint128_t, uint8_t, 8> ==  2); // full
static_assert(capacity<xint128_t, uint8_t, 9> ==  1);
static_assert(capacity<xint128_t, uint8_t, 10> == 1);
static_assert(capacity<xint128_t, uint8_t, 11> == 1);
static_assert(capacity<xint128_t, uint8_t, 12> == 1);
static_assert(capacity<xint128_t, uint8_t, 13> == 1);
static_assert(capacity<xint128_t, uint8_t, 14> == 1);
static_assert(capacity<xint128_t, uint8_t, 15> == 1);
static_assert(capacity<xint128_t, uint8_t, 16> == 1); // full
static_assert(capacity<xint128_t, uint8_t, 17> == 0);
////static_assert(capacity<xint128_t, uint8_t, 0> == 0);

static_assert(capacity<xint128_t, uint16_t>    == 8); // required to fill
static_assert(capacity<xint128_t, uint16_t, 1> == 8);
static_assert(capacity<xint128_t, uint16_t, 2> == 4); // full
static_assert(capacity<xint128_t, uint16_t, 3> == 2);
static_assert(capacity<xint128_t, uint16_t, 4> == 2); // full
static_assert(capacity<xint128_t, uint16_t, 5> == 1);
static_assert(capacity<xint128_t, uint16_t, 6> == 1);
static_assert(capacity<xint128_t, uint16_t, 7> == 1);
static_assert(capacity<xint128_t, uint16_t, 8> == 1); // full
static_assert(capacity<xint128_t, uint16_t, 9> == 0);
////static_assert(capacity<xint128_t, uint16_t, 0> == 0);

static_assert(capacity<xint128_t, uint32_t>    == 4); // required to fill (lanes)
static_assert(capacity<xint128_t, uint32_t, 1> == 4);
static_assert(capacity<xint128_t, uint32_t, 2> == 2); // full
static_assert(capacity<xint128_t, uint32_t, 3> == 1);
static_assert(capacity<xint128_t, uint32_t, 4> == 1); // full
static_assert(capacity<xint128_t, uint32_t, 5> == 0);
////static_assert(capacity<xint128_t, uint32_t, 0> == 0);

static_assert(capacity<xint128_t, uint64_t>    == 2); // required to fill (lanes)
static_assert(capacity<xint128_t, uint64_t, 1> == 2);
static_assert(capacity<xint128_t, uint64_t, 2> == 1); // full
static_assert(capacity<xint128_t, uint64_t, 3> == 0);
////static_assert(capacity<xint128_t, uint64_t, 0> == 0);

static_assert(capacity<xint256_t, uint8_t>        == 32); // required to fill
static_assert(capacity<xint256_t, uint8_t, 1 * 2> == 16); // full
static_assert(capacity<xint256_t, uint8_t, 2 * 2> ==  8); // full
static_assert(capacity<xint256_t, uint8_t, 3 * 2> ==  5);
static_assert(capacity<xint256_t, uint8_t, 4 * 2> ==  4); // full
static_assert(capacity<xint256_t, uint8_t, 5 * 2> ==  3);
static_assert(capacity<xint256_t, uint8_t, 6 * 2> ==  2);
static_assert(capacity<xint256_t, uint8_t, 7 * 2> ==  2);
static_assert(capacity<xint256_t, uint8_t, 8 * 2> ==  2); // full
static_assert(capacity<xint256_t, uint8_t, 9 * 2> ==  1);
static_assert(capacity<xint256_t, uint8_t, 10 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 11 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 12 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 13 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 14 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 15 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 16 * 2> == 1); // full
static_assert(capacity<xint256_t, uint8_t, 17 * 2> == 0);
////static_assert(capacity<xint256_t, uint8_t, 0> == 0);

static_assert(capacity<xint256_t, uint16_t>        == 16); // required to fill
static_assert(capacity<xint256_t, uint16_t, 1 * 2> == 8);
static_assert(capacity<xint256_t, uint16_t, 2 * 2> == 4); // full
static_assert(capacity<xint256_t, uint16_t, 3 * 2> == 2);
static_assert(capacity<xint256_t, uint16_t, 4 * 2> == 2); // full
static_assert(capacity<xint256_t, uint16_t, 5 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 6 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 7 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 8 * 2> == 1); // full
static_assert(capacity<xint256_t, uint16_t, 9 * 2> == 0);
////static_assert(capacity<xint256_t, uint16_t, 0> == 0);

static_assert(capacity<xint256_t, uint32_t>        == 8); // required to fill (lanes)
static_assert(capacity<xint256_t, uint32_t, 1 * 2> == 4);
static_assert(capacity<xint256_t, uint32_t, 2 * 2> == 2); // full
static_assert(capacity<xint256_t, uint32_t, 3 * 2> == 1);
static_assert(capacity<xint256_t, uint32_t, 4 * 2> == 1); // full
static_assert(capacity<xint256_t, uint32_t, 5 * 2> == 0);
////static_assert(capacity<xint256_t, uint32_t, 0> == 0);

static_assert(capacity<xint256_t, uint64_t>        == 4); // required to fill (lanes)
static_assert(capacity<xint256_t, uint64_t, 1 * 2> == 2);
static_assert(capacity<xint256_t, uint64_t, 2 * 2> == 1); // full
static_assert(capacity<xint256_t, uint64_t, 3 * 2> == 0);
////static_assert(capacity<xint256_t, uint64_t, 0> == 0);

static_assert(capacity<xint512_t, uint8_t>        == 64); // required to fill
static_assert(capacity<xint512_t, uint8_t, 1 * 4> == 16); // full
static_assert(capacity<xint512_t, uint8_t, 2 * 4> ==  8); // full
static_assert(capacity<xint512_t, uint8_t, 3 * 4> ==  5);
static_assert(capacity<xint512_t, uint8_t, 4 * 4> ==  4); // full
static_assert(capacity<xint512_t, uint8_t, 5 * 4> ==  3);
static_assert(capacity<xint512_t, uint8_t, 6 * 4> ==  2);
static_assert(capacity<xint512_t, uint8_t, 7 * 4> ==  2);
static_assert(capacity<xint512_t, uint8_t, 8 * 4> ==  2); // full
static_assert(capacity<xint512_t, uint8_t, 9 * 4> ==  1);
static_assert(capacity<xint512_t, uint8_t, 10 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 11 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 12 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 13 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 14 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 15 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 16 * 4> == 1); // full
static_assert(capacity<xint512_t, uint8_t, 17 * 4> == 0);
////static_assert(capacity<xint512_t, uint8_t, 0> == 0);

static_assert(capacity<xint512_t, uint16_t>        == 32); // required to fill
static_assert(capacity<xint512_t, uint16_t, 1 * 4> == 8);
static_assert(capacity<xint512_t, uint16_t, 2 * 4> == 4); // full
static_assert(capacity<xint512_t, uint16_t, 3 * 4> == 2);
static_assert(capacity<xint512_t, uint16_t, 4 * 4> == 2); // full
static_assert(capacity<xint512_t, uint16_t, 5 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 6 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 7 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 8 * 4> == 1); // full
static_assert(capacity<xint512_t, uint16_t, 9 * 4> == 0);
////static_assert(capacity<xint512_t, uint16_t, 0> == 0);

static_assert(capacity<xint512_t, uint32_t>        == 16); // required to fill (lanes)
static_assert(capacity<xint512_t, uint32_t, 1 * 4> == 4);
static_assert(capacity<xint512_t, uint32_t, 2 * 4> == 2); // full
static_assert(capacity<xint512_t, uint32_t, 3 * 4> == 1);
static_assert(capacity<xint512_t, uint32_t, 4 * 4> == 1); // full
static_assert(capacity<xint512_t, uint32_t, 5 * 4> == 0);
////static_assert(capacity<xint512_t, uint32_t, 0> == 0);

static_assert(capacity<xint512_t, uint64_t>        == 8); // required to fill (lanes)
static_assert(capacity<xint512_t, uint64_t, 1 * 4> == 2);
static_assert(capacity<xint512_t, uint64_t, 2 * 4> == 1); // full
static_assert(capacity<xint512_t, uint64_t, 3 * 4> == 0);
////static_assert(capacity<xint512_t, uint64_t, 0> == 0);

// to_extended
// ----------------------------------------------------------------------------

////static_assert(is_same_type<to_extended<uint16_t, 65>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 64>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 63>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 33>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 32>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 31>, xint256_t>);
static_assert(is_same_type<to_extended<uint8_t, 17>, xint256_t>);
static_assert(is_same_type<to_extended<uint8_t, 16>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 15>, xint128_t>);
static_assert(is_same_type<to_extended<uint8_t,  9>, xint128_t>);
static_assert(is_same_type<to_extended<uint8_t,  8>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  7>, uint64_t>);
static_assert(is_same_type<to_extended<uint8_t,  5>, uint64_t>);
static_assert(is_same_type<to_extended<uint8_t,  4>, uint32_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  3>, uint32_t>);
static_assert(is_same_type<to_extended<uint8_t,  2>, uint16_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  1>, uint8_t>);   // full
////static_assert(is_same_type<to_extended<uint8_t,  0>, uint16_t>);

////static_assert(is_same_type<to_extended<uint16_t, 33>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 32>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint16_t, 31>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 17>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 16>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint16_t, 15>, xint256_t>);
static_assert(is_same_type<to_extended<uint16_t,  9>, xint256_t>);
static_assert(is_same_type<to_extended<uint16_t,  8>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint16_t,  7>, xint128_t>);
static_assert(is_same_type<to_extended<uint16_t,  5>, xint128_t>);
static_assert(is_same_type<to_extended<uint16_t,  4>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint16_t,  3>, uint64_t>);
static_assert(is_same_type<to_extended<uint16_t,  2>, uint32_t>);  // full
static_assert(is_same_type<to_extended<uint16_t,  1>, uint16_t>);  // full
////static_assert(is_same_type<to_extended<uint16_t,  0>, uint16_t>);

////static_assert(is_same_type<to_extended<uint32_t, 17>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t, 16>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint32_t, 15>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t,  9>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t,  8>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint32_t,  7>, xint256_t>);
static_assert(is_same_type<to_extended<uint32_t,  5>, xint256_t>);
static_assert(is_same_type<to_extended<uint32_t,  4>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint32_t,  3>, xint128_t>);
static_assert(is_same_type<to_extended<uint32_t,  2>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint32_t,  1>, uint32_t>);  // full
////static_assert(is_same_type<to_extended<uint32_t,  0>, uint32_t>);

////static_assert(is_same_type<to_extended<uint64_t, 9>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 8>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 7>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 5>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 4>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 3>, xint256_t>);
static_assert(is_same_type<to_extended<uint64_t, 2>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 1>, uint64_t>);  // full
////static_assert(is_same_type<to_extended<uint64_t, 0>, uint64_t>);

BOOST_AUTO_TEST_SUITE_END()
