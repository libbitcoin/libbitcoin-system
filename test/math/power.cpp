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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(power_tests)

// use a non-const zero for log base tests to avoid compiler warning against
// possible (but unreachable) division by zero (caused by inlining
// "value / 0" when the base is const 0.
constexpr int32_t signed_zero = 0;
constexpr uint32_t unsigned_zero = 0;

// ceilinged_log

// power__ceilinged_log__negatives__undefined
static_assert(ceilinged_log(-2, 2u) == 0u);
static_assert(ceilinged_log(2u, -2) == 0u);
static_assert(ceilinged_log(-2, -2) == 0u);

// power__ceilinged_log__0_0__undefined
static_assert(ceilinged_log(signed_zero, 0) == 0u);
static_assert(ceilinged_log(signed_zero, 0u) == 0u);
static_assert(ceilinged_log(unsigned_zero, 0) == 0u);
static_assert(ceilinged_log(unsigned_zero, 0u) == 0u);

// power__ceilinged_log__1_0__undefined
static_assert(ceilinged_log(1, 0) == 0u);
static_assert(ceilinged_log(1, 0u) == 0u);
static_assert(ceilinged_log(1u, 0) == 0u);
static_assert(ceilinged_log(1u, 0u) == 0u);

// power__ceilinged_log__0_1__undefined
static_assert(ceilinged_log(signed_zero, 1) == 0u);
static_assert(ceilinged_log(signed_zero, 1u) == 0u);
static_assert(ceilinged_log(unsigned_zero, 1) == 0u);
static_assert(ceilinged_log(unsigned_zero, 1u) == 0u);

// power__ceilinged_log__1_1__undefined
static_assert(ceilinged_log(1, 1) == 0u);
static_assert(ceilinged_log(1, 1u) == 0u);
static_assert(ceilinged_log(1u, 1) == 0u);
static_assert(ceilinged_log(1u, 1u) == 0u);

// power__ceilinged_log__2_1__1
static_assert(ceilinged_log(2, 1) == 1u);
static_assert(ceilinged_log(2, 1u) == 1u);
static_assert(ceilinged_log(2u, 1) == 1u);
static_assert(ceilinged_log(2u, 1u) == 1u);

// power__ceilinged_log__2_2__2
static_assert(ceilinged_log(2, 2) == 2u);
static_assert(ceilinged_log(2, 2u) == 2u);
static_assert(ceilinged_log(2u, 2) == 2u);
static_assert(ceilinged_log(2u, 2u) == 2u);

// power__ceilinged_log__3_2__1
static_assert(ceilinged_log(3, 2) == 1u);
static_assert(ceilinged_log(3, 2u) == 1u);
static_assert(ceilinged_log(3u, 2) == 1u);
static_assert(ceilinged_log(3u, 2u) == 1u);

// power__ceilinged_log__2_3__2
static_assert(ceilinged_log(2, 3) == 2u);
static_assert(ceilinged_log(2, 3u) == 2u);
static_assert(ceilinged_log(2u, 3) == 2u);
static_assert(ceilinged_log(2u, 3u) == 2u);

// power__ceilinged_log__10s__expected
static_assert(ceilinged_log<int>(10, 1) == 1);
static_assert(ceilinged_log<int>(10, 10) == 2);
static_assert(ceilinged_log<int>(10, 100) == 3);
static_assert(ceilinged_log<int>(10, 1000) == 4);
static_assert(ceilinged_log<int>(10, 10000) == 5);
static_assert(ceilinged_log<int>(10, 100000) == 6);
static_assert(ceilinged_log<int>(10, 1000000) == 7);

// floored_log

// power__floored_log__negatives__undefined
static_assert(floored_log(-2, 2u) == 0u);
static_assert(floored_log(2u, -2) == 0u);
static_assert(floored_log(-2, -2) == 0u);

// power__floored_log__0_0__undefined
static_assert(floored_log(signed_zero, 0) == 0u);
static_assert(floored_log(signed_zero, 0u) == 0u);
static_assert(floored_log(unsigned_zero, 0) == 0u);
static_assert(floored_log(unsigned_zero, 0u) == 0u);

// power__floored_log__1_0__undefined
static_assert(floored_log(1, 0) == 0u);
static_assert(floored_log(1, 0u) == 0u);
static_assert(floored_log(1u, 0) == 0u);
static_assert(floored_log(1u, 0u) == 0u);

// power__floored_log__0_1__undefined
static_assert(floored_log(signed_zero, 1) == 0u);
static_assert(floored_log(signed_zero, 1u) == 0u);
static_assert(floored_log(unsigned_zero, 1) == 0u);
static_assert(floored_log(unsigned_zero, 1u) == 0u);

// power__floored_log__1_1__undefined
static_assert(floored_log(1, 1) == 0u);
static_assert(floored_log(1, 1u) == 0u);
static_assert(floored_log(1u, 1) == 0u);
static_assert(floored_log(1u, 1u) == 0u);

// power__floored_log__2_1__0
static_assert(floored_log(2, 1) == 0u);
static_assert(floored_log(2, 1u) == 0u);
static_assert(floored_log(2u, 1) == 0u);
static_assert(floored_log(2u, 1u) == 0u);

// power__floored_log__2_2__1
static_assert(floored_log(2, 2) == 1u);
static_assert(floored_log(2, 2u) == 1u);
static_assert(floored_log(2u, 2) == 1u);
static_assert(floored_log(2u, 2u) == 1u);

// power__floored_log__3_2__0
static_assert(floored_log(3, 2) == 0u);
static_assert(floored_log(3, 2u) == 0u);
static_assert(floored_log(3u, 2) == 0u);
static_assert(floored_log(3u, 2u) == 0u);

// power__floored_log__2_3__1
static_assert(floored_log(2, 3) == 1u);
static_assert(floored_log(2, 3u) == 1u);
static_assert(floored_log(2u, 3) == 1u);
static_assert(floored_log(2u, 3u) == 1u);

// power__floored_log__10s__expected
static_assert(floored_log<int>(10, 1) == 0);
static_assert(floored_log<int>(10, 10) == 1);
static_assert(floored_log<int>(10, 100) == 2);
static_assert(floored_log<int>(10, 1000) == 3);
static_assert(floored_log<int>(10, 10000) == 4);
static_assert(floored_log<int>(10, 100000) == 5);
static_assert(floored_log<int>(10, 1000000) == 6);

// ceilinged_log2

// power__ceilinged_log2__zero__undefined
static_assert(ceilinged_log2(0) == 0u);
static_assert(ceilinged_log2(0u) == 0u);

// power__ceilinged_log2__negative__undefined
static_assert(ceilinged_log2(-42) == 0u);

// power__ceilinged_log2__maximums__sizeof_maximum
static_assert(ceilinged_log2(max_uint8) == to_bits(sizeof(uint8_t)));
static_assert(ceilinged_log2(max_uint16) == to_bits(sizeof(uint16_t)));
static_assert(ceilinged_log2(max_uint32) == to_bits(sizeof(uint32_t)));

// Also mixing in signed and unsigned types.

// power__ceilinged_log2__powers_of_2__expected
// Third case (2) is redundant with second (2).
static_assert(ceilinged_log2((1u << 0) + 0) == 1u);
static_assert(ceilinged_log2((1u << 0) + 1) == 2u);
static_assert(ceilinged_log2((1u << 1) + 0) == 2u);
static_assert(ceilinged_log2((1u << 1) + 1) == 2u);
static_assert(ceilinged_log2((1u << 2) + 0) == 3u);
static_assert(ceilinged_log2((1u << 2) + 1) == 3u);
static_assert(ceilinged_log2((1u << 3) + 0) == 4u);
static_assert(ceilinged_log2((1u << 3) + 1) == 4u);
static_assert(ceilinged_log2((1u << 4) + 0) == 5u);
static_assert(ceilinged_log2((1u << 4) + 1) == 5u);
static_assert(ceilinged_log2((1u << 5) + 0) == 6u);
static_assert(ceilinged_log2((1u << 5) + 1) == 6u);
static_assert(ceilinged_log2((1u << 6) + 0) == 7u);
static_assert(ceilinged_log2((1u << 6) + 1) == 7u);
static_assert(ceilinged_log2((1u << 7) + 0) == 8u);
static_assert(ceilinged_log2((1u << 7) + 1) == 8u);
static_assert(ceilinged_log2((1u << 8) + 0) == 9u);
static_assert(ceilinged_log2((1u << 8) + 1) == 9u);
static_assert(ceilinged_log2((1u << 9) + 0) == 10u);
static_assert(ceilinged_log2((1u << 9) + 1) == 10u);
static_assert(ceilinged_log2<int>((1 << 10) + 0) == 11);
static_assert(ceilinged_log2<int>((1 << 10) + 1) == 11);
static_assert(ceilinged_log2<int>((1 << 11) + 0) == 12);
static_assert(ceilinged_log2<int>((1 << 11) + 1) == 12);
static_assert(ceilinged_log2<int>((1 << 12) + 0) == 13);
static_assert(ceilinged_log2<int>((1 << 12) + 1) == 13);
static_assert(ceilinged_log2<int>((1 << 13) + 0) == 14);
static_assert(ceilinged_log2<int>((1 << 13) + 1) == 14);
static_assert(ceilinged_log2<int>((1 << 14) + 0) == 15);
static_assert(ceilinged_log2<int>((1 << 14) + 1) == 15);
static_assert(ceilinged_log2<int>((1 << 15) + 0) == 16);
static_assert(ceilinged_log2<int>((1 << 15) + 1) == 16);
static_assert(ceilinged_log2<int>((1 << 16) + 0) == 17);
static_assert(ceilinged_log2<int>((1 << 16) + 1) == 17);
static_assert(ceilinged_log2<int>((1 << 17) + 0) == 18);
static_assert(ceilinged_log2<int>((1 << 17) + 1) == 18);
static_assert(ceilinged_log2<int>((1 << 18) + 0) == 19);
static_assert(ceilinged_log2<int>((1 << 18) + 1) == 19);
static_assert(ceilinged_log2<int>((1 << 19) + 0) == 20);
static_assert(ceilinged_log2<int>((1 << 19) + 1) == 20);
static_assert(ceilinged_log2<int>((1 << 20) + 0) == 21);
static_assert(ceilinged_log2<int>((1 << 20) + 1) == 21);
static_assert(ceilinged_log2<int>((1 << 21) + 0) == 22);
static_assert(ceilinged_log2<int>((1 << 21) + 1) == 22);
static_assert(ceilinged_log2<int>((1 << 22) + 0) == 23);
static_assert(ceilinged_log2<int>((1 << 22) + 1) == 23);
static_assert(ceilinged_log2<int>((1 << 23) + 0) == 24);
static_assert(ceilinged_log2<int>((1 << 23) + 1) == 24);
static_assert(ceilinged_log2<int>((1 << 24) + 0) == 25);
static_assert(ceilinged_log2<int>((1 << 24) + 1) == 25);
static_assert(ceilinged_log2<int>((1 << 25) + 0) == 26);
static_assert(ceilinged_log2<int>((1 << 25) + 1) == 26);
static_assert(ceilinged_log2<int>((1 << 26) + 0) == 27);
static_assert(ceilinged_log2<int>((1 << 26) + 1) == 27);
static_assert(ceilinged_log2<int>((1 << 27) + 0) == 28);
static_assert(ceilinged_log2<int>((1 << 27) + 1) == 28);
static_assert(ceilinged_log2<int>((1 << 28) + 0) == 29);
static_assert(ceilinged_log2<int>((1 << 28) + 1) == 29);
static_assert(ceilinged_log2<int>((1 << 29) + 0) == 30);
static_assert(ceilinged_log2((1 << 29) + 1) == 30u);
static_assert(ceilinged_log2((1 << 30) + 0) == 31u);
static_assert(ceilinged_log2((1 << 30) + 1) == 31u);
static_assert(ceilinged_log2((1ll << 31) + 0) == 32u);
static_assert(ceilinged_log2((1ll << 31) + 1) == 32u);

// power__ceilinged_log2__pow2__identity
static_assert(ceilinged_log2<int>(power2(15)) == 15 + 1);
static_assert(ceilinged_log2<unsigned>(power2(15u)) == 15u + 1u);

// power__ceilinged_log2__power_2__identity
static_assert(ceilinged_log2<int>(power(2, 15)) == 15 + 1);
static_assert(ceilinged_log2<unsigned>(power(2u, 15u)) == 15u + 1u);

// ceilinged_log2(uintx)

// power__ceilinged_log2_uintx__zero__undefined
static_assert(ceilinged_log2(uint256_t(0)) == 0u);
static_assert(ceilinged_log2(uint256_t(0u)) == 0u);

BOOST_AUTO_TEST_CASE(power__ceilinged_log2_uintx__maximums__sizeof_maximum)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint8)), to_bits(sizeof(uint8_t)));
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint16)), to_bits(sizeof(uint16_t)));
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx(max_uint32)), to_bits(sizeof(uint32_t)));
}

// Also mixing in signed and unsigned types.

// power__ceilinged_log2_uintx__powers_of_2__expected
// Third case (2) is redundant with second (2).
static_assert(ceilinged_log2(uint256_t((1u << 0) + 0)) == 1u);
static_assert(ceilinged_log2(uint256_t((1u << 0) + 1)) == 2u);
static_assert(ceilinged_log2(uint256_t((1u << 1) + 0)) == 2u);
static_assert(ceilinged_log2(uint256_t((1u << 1) + 1)) == 2u);
static_assert(ceilinged_log2(uint256_t((1u << 2) + 0)) == 3u);
static_assert(ceilinged_log2(uint256_t((1u << 2) + 1)) == 3u);
static_assert(ceilinged_log2(uint256_t((1u << 3) + 0)) == 4u);
static_assert(ceilinged_log2(uint256_t((1u << 3) + 1)) == 4u);
static_assert(ceilinged_log2(uint256_t((1u << 4) + 0)) == 5u);
static_assert(ceilinged_log2(uint256_t((1u << 4) + 1)) == 5u);
static_assert(ceilinged_log2(uint256_t((1u << 5) + 0)) == 6u);
static_assert(ceilinged_log2(uint256_t((1u << 5) + 1)) == 6u);
static_assert(ceilinged_log2(uint256_t((1u << 6) + 0)) == 7u);
static_assert(ceilinged_log2(uint256_t((1u << 6) + 1)) == 7u);
static_assert(ceilinged_log2(uint256_t((1u << 7) + 0)) == 8u);
static_assert(ceilinged_log2(uint256_t((1u << 7) + 1)) == 8u);
static_assert(ceilinged_log2(uint256_t((1u << 8) + 0)) == 9u);
static_assert(ceilinged_log2(uint256_t((1u << 8) + 1)) == 9u);
static_assert(ceilinged_log2(uint256_t((1u << 9) + 0)) == 10u);
static_assert(ceilinged_log2(uint256_t((1u << 9) + 1)) == 10u);
static_assert(ceilinged_log2<int>(uint256_t((1 << 10) + 0)) == 11);
static_assert(ceilinged_log2<int>(uint256_t((1 << 10) + 1)) == 11);
static_assert(ceilinged_log2<int>(uint256_t((1 << 11) + 0)) == 12);
static_assert(ceilinged_log2<int>(uint256_t((1 << 11) + 1)) == 12);
static_assert(ceilinged_log2<int>(uint256_t((1 << 12) + 0)) == 13);
static_assert(ceilinged_log2<int>(uint256_t((1 << 12) + 1)) == 13);
static_assert(ceilinged_log2<int>(uint256_t((1 << 13) + 0)) == 14);
static_assert(ceilinged_log2<int>(uint256_t((1 << 13) + 1)) == 14);
static_assert(ceilinged_log2<int>(uint256_t((1 << 14) + 0)) == 15);
static_assert(ceilinged_log2<int>(uint256_t((1 << 14) + 1)) == 15);
static_assert(ceilinged_log2<int>(uint256_t((1 << 15) + 0)) == 16);
static_assert(ceilinged_log2<int>(uint256_t((1 << 15) + 1)) == 16);
static_assert(ceilinged_log2<int>(uint256_t((1 << 16) + 0)) == 17);
static_assert(ceilinged_log2<int>(uint256_t((1 << 16) + 1)) == 17);
static_assert(ceilinged_log2<int>(uint256_t((1 << 17) + 0)) == 18);
static_assert(ceilinged_log2<int>(uint256_t((1 << 17) + 1)) == 18);
static_assert(ceilinged_log2<int>(uint256_t((1 << 18) + 0)) == 19);
static_assert(ceilinged_log2<int>(uint256_t((1 << 18) + 1)) == 19);
static_assert(ceilinged_log2<int>(uint256_t((1 << 19) + 0)) == 20);
static_assert(ceilinged_log2<int>(uint256_t((1 << 19) + 1)) == 20);
static_assert(ceilinged_log2<int>(uint256_t((1 << 20) + 0)) == 21);
static_assert(ceilinged_log2<int>(uint256_t((1 << 20) + 1)) == 21);
static_assert(ceilinged_log2<int>(uint256_t((1 << 21) + 0)) == 22);
static_assert(ceilinged_log2<int>(uint256_t((1 << 21) + 1)) == 22);
static_assert(ceilinged_log2<int>(uint256_t((1 << 22) + 0)) == 23);
static_assert(ceilinged_log2<int>(uint256_t((1 << 22) + 1)) == 23);
static_assert(ceilinged_log2<int>(uint256_t((1 << 23) + 0)) == 24);
static_assert(ceilinged_log2<int>(uint256_t((1 << 23) + 1)) == 24);
static_assert(ceilinged_log2<int>(uint256_t((1 << 24) + 0)) == 25);
static_assert(ceilinged_log2<int>(uint256_t((1 << 24) + 1)) == 25);
static_assert(ceilinged_log2<int>(uint256_t((1 << 25) + 0)) == 26);
static_assert(ceilinged_log2<int>(uint256_t((1 << 25) + 1)) == 26);
static_assert(ceilinged_log2<int>(uint256_t((1 << 26) + 0)) == 27);
static_assert(ceilinged_log2<int>(uint256_t((1 << 26) + 1)) == 27);
static_assert(ceilinged_log2<int>(uint256_t((1 << 27) + 0)) == 28);
static_assert(ceilinged_log2<int>(uint256_t((1 << 27) + 1)) == 28);
static_assert(ceilinged_log2<int>(uint256_t((1 << 28) + 0)) == 29);
static_assert(ceilinged_log2<int>(uint256_t((1 << 28) + 1)) == 29);
static_assert(ceilinged_log2<int>(uint256_t((1 << 29) + 0)) == 30);
static_assert(ceilinged_log2(uint256_t((1 << 29) + 1)) == 30u);
static_assert(ceilinged_log2(uint256_t((1 << 30) + 0)) == 31u);
static_assert(ceilinged_log2(uint256_t((1 << 30) + 1)) == 31u);
static_assert(ceilinged_log2(uint256_t((1ll << 31) + 0)) == 32u);
static_assert(ceilinged_log2(uint256_t((1ll << 31) + 1)) == 32u);
 
BOOST_AUTO_TEST_CASE(power__ceilinged_log2_uintx__powers_of_2__expected)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 32) + 0)), 33u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 32) + 1)), 33u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 63) + 0)), 64u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(uintx((1ull << 63) + 1)), 64u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<uintx>(base16_array("ffffffffffffffff"))), 64u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<32>(base16_array("010000000000000000"))), 65u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<32>(base16_array("ffffffffffffffffffffffffffffffff"))), 128u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<32>(base16_array("0100000000000000000000000000000000"))), 129u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<64>(base16_array("f000000000000000000000000000000000000000000000000000000000000000"))), 256u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<65>(base16_array("010000000000000000000000000000000000000000000000000000000000000000"))), 257u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<128>(base16_array("f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))), 512u);
    BOOST_REQUIRE_EQUAL(ceilinged_log2(from_big_endian<129>(base16_array("0100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))), 513u);
}

BOOST_AUTO_TEST_CASE(power__ceilinged_log2_uintx__pow2__identity)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(ceilinged_log2<int>(uintx(power2(15))), 15 + 1);
    BOOST_REQUIRE_EQUAL(ceilinged_log2<unsigned>(uintx(power2(15u))), 15u + 1u);
}

BOOST_AUTO_TEST_CASE(power__ceilinged_log2_uintx__power_2__identity)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(ceilinged_log2<int>(uintx(power(2, 15))), 15 + 1);
    BOOST_REQUIRE_EQUAL(ceilinged_log2<unsigned>(uintx(power(2u, 15u))), 15u + 1u);
}

// ceilinged_log256
static_assert(ceilinged_log256(0x0100000000000000ull) == 8);
static_assert(ceilinged_log256(0x01000000000000ull) == 7);
static_assert(ceilinged_log256(0x010000000000ull) == 6);
static_assert(ceilinged_log256(0x0100000000ull) == 5);
static_assert(ceilinged_log256(0x01000000ull) == 4);
static_assert(ceilinged_log256(0x010000ull) == 3);
static_assert(ceilinged_log256(0x0100ull) == 2);
static_assert(ceilinged_log256(0x01ull) == 1);
static_assert(ceilinged_log256(0x00ull) == 0);

// floored_log2

// power__floored_log2__zero__undefined
static_assert(floored_log2(0) == 0u);
static_assert(floored_log2(0u) == 0u);

// power__floored_log2__negative__undefined
static_assert(floored_log2(-42) == 0u);

// power__floored_log2__maximums__sizeof_maximum_minus_one
static_assert(floored_log2(max_uint8) == sub1(to_bits(sizeof(uint8_t))));
static_assert(floored_log2(max_uint16) == sub1(to_bits(sizeof(uint16_t))));
static_assert(floored_log2(max_uint32) == sub1(to_bits(sizeof(uint32_t))));

// Also mixing in signed and unsigned types.

// power__floored_log2__powers_of_2__expected
// Third case (2) is redundant with second (2).
static_assert(floored_log2((1u << 0) + 0) == 0u);
static_assert(floored_log2((1u << 0) + 1) == 1u);
///static_assert(floored_log2((1u << 1) + 0) == 1u);
static_assert(floored_log2((1u << 1) + 1) == 1u);
static_assert(floored_log2((1u << 2) + 0) == 2u);
static_assert(floored_log2((1u << 2) + 1) == 2u);
static_assert(floored_log2((1u << 3) + 0) == 3u);
static_assert(floored_log2((1u << 3) + 1) == 3u);
static_assert(floored_log2((1u << 4) + 0) == 4u);
static_assert(floored_log2((1u << 4) + 1) == 4u);
static_assert(floored_log2((1u << 5) + 0) == 5u);
static_assert(floored_log2((1u << 5) + 1) == 5u);
static_assert(floored_log2((1u << 6) + 0) == 6u);
static_assert(floored_log2((1u << 6) + 1) == 6u);
static_assert(floored_log2((1u << 7) + 0) == 7u);
static_assert(floored_log2((1u << 7) + 1) == 7u);
static_assert(floored_log2((1u << 8) + 0) == 8u);
static_assert(floored_log2((1u << 8) + 1) == 8u);
static_assert(floored_log2((1u << 9) + 0) == 9u);
static_assert(floored_log2((1u << 9) + 1) == 9u);
static_assert(floored_log2<int>((1 << 10) + 0) == 10);
static_assert(floored_log2<int>((1 << 10) + 1) == 10);
static_assert(floored_log2<int>((1 << 11) + 0) == 11);
static_assert(floored_log2<int>((1 << 11) + 1) == 11);
static_assert(floored_log2<int>((1 << 12) + 0) == 12);
static_assert(floored_log2<int>((1 << 12) + 1) == 12);
static_assert(floored_log2<int>((1 << 13) + 0) == 13);
static_assert(floored_log2<int>((1 << 13) + 1) == 13);
static_assert(floored_log2<int>((1 << 14) + 0) == 14);
static_assert(floored_log2<int>((1 << 14) + 1) == 14);
static_assert(floored_log2<int>((1 << 15) + 0) == 15);
static_assert(floored_log2<int>((1 << 15) + 1) == 15);
static_assert(floored_log2<int>((1 << 16) + 0) == 16);
static_assert(floored_log2<int>((1 << 16) + 1) == 16);
static_assert(floored_log2<int>((1 << 17) + 0) == 17);
static_assert(floored_log2<int>((1 << 17) + 1) == 17);
static_assert(floored_log2<int>((1 << 18) + 0) == 18);
static_assert(floored_log2<int>((1 << 18) + 1) == 18);
static_assert(floored_log2<int>((1 << 19) + 0) == 19);
static_assert(floored_log2<int>((1 << 19) + 1) == 19);
static_assert(floored_log2<int>((1 << 20) + 0) == 20);
static_assert(floored_log2<int>((1 << 20) + 1) == 20);
static_assert(floored_log2<int>((1 << 21) + 0) == 21);
static_assert(floored_log2<int>((1 << 21) + 1) == 21);
static_assert(floored_log2<int>((1 << 22) + 0) == 22);
static_assert(floored_log2<int>((1 << 22) + 1) == 22);
static_assert(floored_log2<int>((1 << 23) + 0) == 23);
static_assert(floored_log2<int>((1 << 23) + 1) == 23);
static_assert(floored_log2<int>((1 << 24) + 0) == 24);
static_assert(floored_log2<int>((1 << 24) + 1) == 24);
static_assert(floored_log2<int>((1 << 25) + 0) == 25);
static_assert(floored_log2<int>((1 << 25) + 1) == 25);
static_assert(floored_log2<int>((1 << 26) + 0) == 26);
static_assert(floored_log2<int>((1 << 26) + 1) == 26);
static_assert(floored_log2<int>((1 << 27) + 0) == 27);
static_assert(floored_log2<int>((1 << 27) + 1) == 27);
static_assert(floored_log2<int>((1 << 28) + 0) == 28);
static_assert(floored_log2<int>((1 << 28) + 1) == 28);
static_assert(floored_log2<int>((1 << 29) + 0) == 29);
static_assert(floored_log2<int>((1 << 29) + 1) == 29);
static_assert(floored_log2((1 << 30) + 0) == 30u);
static_assert(floored_log2((1 << 30) + 1) == 30u);
static_assert(floored_log2((1ll << 31) + 0) == 31u);
static_assert(floored_log2((1ll << 31) + 1) == 31u);

// power__floored_log2__pow2__identity
static_assert(floored_log2<int>(power2(15)) == 15);
static_assert(floored_log2<unsigned>(power2(15u)) == 15u);

// power__floored_log2__power_2__identity
static_assert(floored_log2<int>(power(2, 15)) == 15);
static_assert(floored_log2<unsigned>(power(2u, 15u)) == 15u);

// floored_log2(uintx)

// power__floored_log2_uintx__zero__undefined
static_assert(floored_log2(uint256_t(0)) == 0u);
static_assert(floored_log2(uint256_t(0)) == 0u);

BOOST_AUTO_TEST_CASE(power__floored_log2_uintx__maximums__sizeof_maximum_minus_one)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint8)), sub1(to_bits(sizeof(uint8_t))));
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint16)), sub1(to_bits(sizeof(uint16_t))));
    BOOST_REQUIRE_EQUAL(floored_log2(uintx(max_uint32)), sub1(to_bits(sizeof(uint32_t))));
}

// power__floored_log2_uintx__powers_of_2__expected
// Third case (2) is redundant with second (2).
static_assert(floored_log2(uint256_t((1u << 0) + 0)) == 0u);
static_assert(floored_log2(uint256_t((1u << 0) + 1)) == 1u);
///static_assert(floored_log2(uint256_t((1u << 1) + 0)) == 1u);
static_assert(floored_log2(uint256_t((1u << 1) + 1)) == 1u);
static_assert(floored_log2(uint256_t((1u << 2) + 0)) == 2u);
static_assert(floored_log2(uint256_t((1u << 2) + 1)) == 2u);
static_assert(floored_log2(uint256_t((1u << 3) + 0)) == 3u);
static_assert(floored_log2(uint256_t((1u << 3) + 1)) == 3u);
static_assert(floored_log2(uint256_t((1u << 4) + 0)) == 4u);
static_assert(floored_log2(uint256_t((1u << 4) + 1)) == 4u);
static_assert(floored_log2(uint256_t((1u << 5) + 0)) == 5u);
static_assert(floored_log2(uint256_t((1u << 5) + 1)) == 5u);
static_assert(floored_log2(uint256_t((1u << 6) + 0)) == 6u);
static_assert(floored_log2(uint256_t((1u << 6) + 1)) == 6u);
static_assert(floored_log2(uint256_t((1u << 7) + 0)) == 7u);
static_assert(floored_log2(uint256_t((1u << 7) + 1)) == 7u);
static_assert(floored_log2(uint256_t((1u << 8) + 0)) == 8u);
static_assert(floored_log2(uint256_t((1u << 8) + 1)) == 8u);
static_assert(floored_log2(uint256_t((1u << 9) + 0)) == 9u);
static_assert(floored_log2(uint256_t((1u << 9) + 1)) == 9u);
static_assert(floored_log2<int>(uint256_t((1 << 10) + 0)) == 10);
static_assert(floored_log2<int>(uint256_t((1 << 10) + 1)) == 10);
static_assert(floored_log2<int>(uint256_t((1 << 11) + 0)) == 11);
static_assert(floored_log2<int>(uint256_t((1 << 11) + 1)) == 11);
static_assert(floored_log2<int>(uint256_t((1 << 12) + 0)) == 12);
static_assert(floored_log2<int>(uint256_t((1 << 12) + 1)) == 12);
static_assert(floored_log2<int>(uint256_t((1 << 13) + 0)) == 13);
static_assert(floored_log2<int>(uint256_t((1 << 13) + 1)) == 13);
static_assert(floored_log2<int>(uint256_t((1 << 14) + 0)) == 14);
static_assert(floored_log2<int>(uint256_t((1 << 14) + 1)) == 14);
static_assert(floored_log2<int>(uint256_t((1 << 15) + 0)) == 15);
static_assert(floored_log2<int>(uint256_t((1 << 15) + 1)) == 15);
static_assert(floored_log2<int>(uint256_t((1 << 16) + 0)) == 16);
static_assert(floored_log2<int>(uint256_t((1 << 16) + 1)) == 16);
static_assert(floored_log2<int>(uint256_t((1 << 17) + 0)) == 17);
static_assert(floored_log2<int>(uint256_t((1 << 17) + 1)) == 17);
static_assert(floored_log2<int>(uint256_t((1 << 18) + 0)) == 18);
static_assert(floored_log2<int>(uint256_t((1 << 18) + 1)) == 18);
static_assert(floored_log2<int>(uint256_t((1 << 19) + 0)) == 19);
static_assert(floored_log2<int>(uint256_t((1 << 19) + 1)) == 19);
static_assert(floored_log2<int>(uint256_t((1 << 20) + 0)) == 20);
static_assert(floored_log2<int>(uint256_t((1 << 20) + 1)) == 20);
static_assert(floored_log2<int>(uint256_t((1 << 21) + 0)) == 21);
static_assert(floored_log2<int>(uint256_t((1 << 21) + 1)) == 21);
static_assert(floored_log2<int>(uint256_t((1 << 22) + 0)) == 22);
static_assert(floored_log2<int>(uint256_t((1 << 22) + 1)) == 22);
static_assert(floored_log2<int>(uint256_t((1 << 23) + 0)) == 23);
static_assert(floored_log2<int>(uint256_t((1 << 23) + 1)) == 23);
static_assert(floored_log2<int>(uint256_t((1 << 24) + 0)) == 24);
static_assert(floored_log2<int>(uint256_t((1 << 24) + 1)) == 24);
static_assert(floored_log2<int>(uint256_t((1 << 25) + 0)) == 25);
static_assert(floored_log2<int>(uint256_t((1 << 25) + 1)) == 25);
static_assert(floored_log2<int>(uint256_t((1 << 26) + 0)) == 26);
static_assert(floored_log2<int>(uint256_t((1 << 26) + 1)) == 26);
static_assert(floored_log2<int>(uint256_t((1 << 27) + 0)) == 27);
static_assert(floored_log2<int>(uint256_t((1 << 27) + 1)) == 27);
static_assert(floored_log2<int>(uint256_t((1 << 28) + 0)) == 28);
static_assert(floored_log2<int>(uint256_t((1 << 28) + 1)) == 28);
static_assert(floored_log2<int>(uint256_t((1 << 29) + 0)) == 29);
static_assert(floored_log2<int>(uint256_t((1 << 29) + 1)) == 29);

BOOST_AUTO_TEST_CASE(power__floored_log2_uintx__powers_of_2__expected)
{
    // uintx{} not constexpr.
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 30) + 0)), 30u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 30) + 1)), 30u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 31) + 0)), 31u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1u << 31) + 1)), 31u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 32) + 0)), 32u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 32) + 1)), 32u);
    BOOST_REQUIRE_EQUAL(floored_log2(uintx((1ull << 63) + 1)), 63u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<uintx>(base16_array("ffffffffffffffff"))), 63u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<32>(base16_array("010000000000000000"))), 64u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<32>(base16_array("ffffffffffffffffffffffffffffffff"))), 127u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<32>(base16_array("0100000000000000000000000000000000"))), 128u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<64>(base16_array("f000000000000000000000000000000000000000000000000000000000000000"))), 255u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<65>(base16_array("010000000000000000000000000000000000000000000000000000000000000000"))), 256u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<128>(base16_array("f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))), 511u);
    BOOST_REQUIRE_EQUAL(floored_log2(from_big_endian<129>(base16_array("0100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"))), 512u);
}

BOOST_AUTO_TEST_CASE(power__floored_log2_uintx__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(floored_log2<int>(uintx(power2(15))), 15);
    BOOST_REQUIRE_EQUAL(floored_log2<unsigned>(uintx(power2(15u))), 15u);
}

BOOST_AUTO_TEST_CASE(power__floored_log2_uintx__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(floored_log2<int>(uintx(power(2, 15))), 15);
    BOOST_REQUIRE_EQUAL(floored_log2<unsigned>(uintx(power(2u, 15u))), 15u);
}

// floored_log256
static_assert(floored_log256(0xffffffffffffffffull) == 7);
static_assert(floored_log256(0xffffffffffffffull) == 6);
static_assert(floored_log256(0xffffffffffffull) == 5);
static_assert(floored_log256(0xffffffffffull) == 4);
static_assert(floored_log256(0xffffffffull) == 3);
static_assert(floored_log256(0xffffffull) == 2);
static_assert(floored_log256(0xffffull) == 1);
static_assert(floored_log256(0xffull) == 0);
static_assert(floored_log256(0x00ull) == 0);

// TODO: test power return types (default to size_t).

// power2

// power__pow2__0__1
static_assert(power2(0) == 1u);
static_assert(power2(0u) == 1u);

// power__pow2__1__2
static_assert(power2(1) == 2u);
static_assert(power2(1u) == 2u);

// power__pow2__negative_1__0
static_assert(power2(-1) == 1u / 2u);

// power__pow2__negative_2__0
static_assert(power2(-2) == 1u / 4u);

// power__pow2__negative_3__underflow
static_assert(power2(-3) == 0u);

// power__pow2__15__0x8000
static_assert(power2<uint16_t>(15) == 0x8000u);

// power__pow2__overflow__0
static_assert(power2<uint16_t>(16) == 0u);

// power

// power__power__0_0__undefined
static_assert(power(0, 0) == 0u);
static_assert(power(0u, 0u) == 0u);

// power__power__0_1__0
static_assert(power(0, 1) == 0u);
static_assert(power(0u, 1u) == 0u);

// power__power__1_0__1
static_assert(power(1, 0) == 1u);
static_assert(power(1u, 0u) == 1u);

// power__power__1_1__1
static_assert(power(1, 1) == 1u);
static_assert(power(1u, 1u) == 1u);

// power__power__3_0__1
static_assert(power(3, 0) == 1u);
static_assert(power(3u, 0u) == 1u);

// power__power__1_negative_1__1
static_assert(power(1, -1) == 1u / 1u);

// power__power__3_negative_1__0
static_assert(power(3, -1) == 1u / 3u);

// power__power__3_negative_2__0
static_assert(power(3, -2) == 1u / 9u);

// power__power__3_negative_3__0
static_assert(power(3, -3) == 1u / 81u);

// power__power__3_10__0xe6a9
static_assert(power(3, 10) == 0xe6a9u);
static_assert(power(3u, 10u) == 0xe6a9u);

// power__power__overflow__expected
static_assert(power<uint16_t>(3, 11) == 0xb3fbu);

// power__power__2_16__equals_pow2_16
static_assert(power(2, 16) == power2(16));
static_assert(power(2u, 16u) == power2(16u));

// power__power__negative_1_0__1
static_assert(power(-1, 0) == 1u);

// power__power__negative_1_1__negative_1
static_assert(power(-1, 1) == static_cast<size_t>(-1));

// power__power__negative_3_0__1
static_assert(power(-3, 0) == 1u);

// power__power__negative_3_1__negative_3
static_assert(power(-3, 1) == static_cast<size_t>(-3));

// power__power__negative_3_negative_1__0
static_assert(power(-3, -1) == 1u / static_cast<size_t>(-3));

// power__power__negative_3_negative_2__0
static_assert(power(-3, -2) ==  1u / 9u);

// power__power__negative_3_negative_3__0
static_assert(power(-3, -3) == 1u / static_cast<size_t>(-81));

// power__power__negative_3_10__0xe6a9
static_assert(power(-3, 10) == 0xe6a9u);

// power__power__negative_overflow__expected
static_assert(power<uint16_t>(-3, 11) == 0x4c05u);

BOOST_AUTO_TEST_SUITE_END()
