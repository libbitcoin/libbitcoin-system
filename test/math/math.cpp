/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(math_tests)

#define MATH_BASICS 1
#define MATH_POWERS 1
#define MATH_MODULO 1
#define MATH_DIVIDE 1

#ifdef MATH_BASICS

// absolute

BOOST_AUTO_TEST_CASE(math__absolute__zero__0)
{
    BOOST_REQUIRE_EQUAL(system::absolute(0), 0);
    BOOST_REQUIRE_EQUAL(system::absolute(0u), 0u);
}

BOOST_AUTO_TEST_CASE(math__absolute__42__42)
{
    BOOST_REQUIRE_EQUAL(system::absolute(42), 42);
    BOOST_REQUIRE_EQUAL(system::absolute(42u), 42u);
}

BOOST_AUTO_TEST_CASE(math__absolute__negative_42__42)
{
    BOOST_REQUIRE_EQUAL(system::absolute(-42), 42);
}

// negative

BOOST_AUTO_TEST_CASE(math__negative__zero_zero__false)
{
    BOOST_REQUIRE(!system::negative(0, 0));
    BOOST_REQUIRE(!system::negative(0u, 0));
    BOOST_REQUIRE(!system::negative(0, 0u));
    BOOST_REQUIRE(!system::negative(0u, 0u));
}

BOOST_AUTO_TEST_CASE(math__negative__zero_one__false)
{
    BOOST_REQUIRE(!system::negative(0, 1));
    BOOST_REQUIRE(!system::negative(0u, 1));
    BOOST_REQUIRE(!system::negative(0, 1u));
    BOOST_REQUIRE(!system::negative(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__negative__one_zero__false)
{
    BOOST_REQUIRE(!system::negative(1, 0));
    BOOST_REQUIRE(!system::negative(1u, 0));
    BOOST_REQUIRE(!system::negative(1, 0u));
    BOOST_REQUIRE(!system::negative(1u, 0u));
}

BOOST_AUTO_TEST_CASE(math__negative__one_one__false)
{
    BOOST_REQUIRE(!system::negative(1, 1));
    BOOST_REQUIRE(!system::negative(1u, 1));
    BOOST_REQUIRE(!system::negative(1, 1u));
    BOOST_REQUIRE(!system::negative(1u, 1u));
}

BOOST_AUTO_TEST_CASE(math__negative__negative_one_one__true)
{
    BOOST_REQUIRE(system::negative(-1, 1));
    BOOST_REQUIRE(system::negative(-1, 1u));
}

BOOST_AUTO_TEST_CASE(math__negative__one_negative_one__true)
{
    BOOST_REQUIRE(system::negative(1, -1));
    BOOST_REQUIRE(system::negative(1u, -1));
}

BOOST_AUTO_TEST_CASE(math__negative__max_unsigned_max_unsigned__false)
{
    BOOST_REQUIRE(!system::negative(max_uint32, max_uint32));
}

BOOST_AUTO_TEST_CASE(math__negative__max_unsigned_min_signed__true)
{
    BOOST_REQUIRE(system::negative(max_uint32, min_int32));
}

BOOST_AUTO_TEST_CASE(math__negative__min_signed_max_unsigned__true)
{
    BOOST_REQUIRE(system::negative(min_int32, max_uint32));
}

BOOST_AUTO_TEST_CASE(math__negative__min_signed_min_unsigned__false)
{
    BOOST_REQUIRE(!system::negative(min_int32, min_int32));
}

// remainder
// Same as c++ (dividend % divisor) != 0.

BOOST_AUTO_TEST_CASE(math__remainder__0_1__false)
{
    BOOST_REQUIRE(!system::remainder(0, 1));
    BOOST_REQUIRE(!system::remainder(0u, 1));
    BOOST_REQUIRE(!system::remainder(0, 1u));
    BOOST_REQUIRE(!system::remainder(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_positives__false)
{
    BOOST_REQUIRE(!system::remainder(42, 2));
    BOOST_REQUIRE(!system::remainder(42u, 2));
    BOOST_REQUIRE(!system::remainder(42, 2u));
    BOOST_REQUIRE(!system::remainder(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negative_dividend__false)
{
    BOOST_REQUIRE(!system::remainder(-42, 2));
    BOOST_REQUIRE(!system::remainder(-42, 2u));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negative_divisor__false)
{
    BOOST_REQUIRE_EQUAL(42 % -2, 0);
    BOOST_REQUIRE(!system::remainder(42, -2));

    ////static_assert(42u / -2 == 0, "truncated quotient is 0");
    ////static_assert(42u % -2 == 42, "truncated quotient is 0");
    ////static_assert(42u == (-2 * 0) + 42, "identity");
    BOOST_REQUIRE(system::remainder(42u, -2));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negatives__false)
{
    BOOST_REQUIRE(!system::remainder(-42, -2));
}

BOOST_AUTO_TEST_CASE(math__remainder__positives__true)
{
    BOOST_REQUIRE(system::remainder(42, 8));
    BOOST_REQUIRE(system::remainder(42u, 8));
    BOOST_REQUIRE(system::remainder(42, 8u));
    BOOST_REQUIRE(system::remainder(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__remainder__negative_dividend__true)
{
    BOOST_REQUIRE(system::remainder(-42, 8));
    BOOST_REQUIRE(system::remainder(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__remainder__negative_divisor__true)
{
    BOOST_REQUIRE(system::remainder(42, -8));
    BOOST_REQUIRE(system::remainder(42u, -8));
}

BOOST_AUTO_TEST_CASE(math__remainder__negatives__true)
{
    BOOST_REQUIRE(system::remainder(-42, -8));
}

#endif // MATH_BASICS

#ifdef MATH_POWERS

// ceilinged_log2

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__zero__undefined)
{
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(0), 0);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(0u), 0u);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__negative__undefined)
{
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(-42), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__maximums__sizeof_maximum)
{
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(max_uint8), sizeof(uint8_t) * byte_bits);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(max_uint16), sizeof(uint16_t) * byte_bits);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(max_uint32), sizeof(uint32_t) * byte_bits);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__powers_of_2__expected)
{
    // Third case (2) is redundant with second (2).
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 0) + 0), 1u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 0) + 1), 1u);
    ////BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 1) + 0), 1u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 1) + 1), 2u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 2) + 0), 2u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 2) + 1), 3u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 3) + 0), 3u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 3) + 1), 4u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 4) + 0), 4u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 4) + 1), 5u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 5) + 0), 5u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 5) + 1), 6u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 6) + 0), 6u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 6) + 1), 7u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 7) + 0), 7u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 7) + 1), 8u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 8) + 0), 8u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 8) + 1), 9u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 9) + 0), 9u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 9) + 1), 10u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 10) + 0), 10);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 10) + 1), 11);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 11) + 0), 11);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 11) + 1), 12);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 12) + 0), 12);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 12) + 1), 13);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 13) + 0), 13);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 13) + 1), 14);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 14) + 0), 14);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 14) + 1), 15);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 15) + 0), 15);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 15) + 1), 16);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 16) + 0), 16);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 16) + 1), 17);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 17) + 0), 17);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 17) + 1), 18);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 18) + 0), 18);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 18) + 1), 19);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 19) + 0), 19);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 19) + 1), 20);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 20) + 0), 20);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 20) + 1), 21);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 21) + 0), 21);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 21) + 1), 22);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 22) + 0), 22);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 22) + 1), 23);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 23) + 0), 23);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 23) + 1), 24);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 24) + 0), 24);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 24) + 1), 25);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 25) + 0), 25);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 25) + 1), 26);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 26) + 0), 26);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 26) + 1), 27);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 27) + 0), 27);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 27) + 1), 28);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 28) + 0), 28);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 28) + 1), 29);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1 << 29) + 0), 29);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 29) + 1), 30u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 30) + 0), 30u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 30) + 1), 31u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 31) + 0), 31u);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2((1u << 31) + 1), 32u);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(system::power2(15)), 15);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(system::power2(15u)), 15u);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_log2__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(system::power(2, 15)), 15);
    BOOST_REQUIRE_EQUAL(system::ceilinged_log2(system::power(2u, 15u)), 15u);
}

// floored_log2

BOOST_AUTO_TEST_CASE(math__floored_log2__zero__undefined)
{
    BOOST_REQUIRE_EQUAL(system::floored_log2(0), 0);
    BOOST_REQUIRE_EQUAL(system::floored_log2(0u), 0u);
}

BOOST_AUTO_TEST_CASE(math__floored_log2__negative__undefined)
{
    BOOST_REQUIRE_EQUAL(system::floored_log2(-42), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_log2__maximums__sizeof_maximum_minus_one)
{
    BOOST_REQUIRE_EQUAL(system::floored_log2(max_uint8), sizeof(uint8_t) * byte_bits - 1);
    BOOST_REQUIRE_EQUAL(system::floored_log2(max_uint16), sizeof(uint16_t) * byte_bits - 1);
    BOOST_REQUIRE_EQUAL(system::floored_log2(max_uint32), sizeof(uint32_t) * byte_bits - 1);
}

BOOST_AUTO_TEST_CASE(math__floored_log2__powers_of_2__expected)
{
    // Third case (2) is redundant with second (2).
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 0) + 0), 0u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 0) + 1), 1u);
    ///BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 1) + 0), 1u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 1) + 1), 1u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 2) + 0), 2u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 2) + 1), 2u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 3) + 0), 3u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 3) + 1), 3u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 4) + 0), 4u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 4) + 1), 4u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 5) + 0), 5u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 5) + 1), 5u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 6) + 0), 6u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 6) + 1), 6u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 7) + 0), 7u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 7) + 1), 7u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 8) + 0), 8u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 8) + 1), 8u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 9) + 0), 9u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 9) + 1), 9u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 10) + 0), 10);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 10) + 1), 10);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 11) + 0), 11);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 11) + 1), 11);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 12) + 0), 12);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 12) + 1), 12);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 13) + 0), 13);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 13) + 1), 13);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 14) + 0), 14);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 14) + 1), 14);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 15) + 0), 15);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 15) + 1), 15);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 16) + 0), 16);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 16) + 1), 16);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 17) + 0), 17);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 17) + 1), 17);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 18) + 0), 18);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 18) + 1), 18);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 19) + 0), 19);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 19) + 1), 19);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 20) + 0), 20);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 20) + 1), 20);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 21) + 0), 21);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 21) + 1), 21);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 22) + 0), 22);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 22) + 1), 22);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 23) + 0), 23);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 23) + 1), 23);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 24) + 0), 24);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 24) + 1), 24);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 25) + 0), 25);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 25) + 1), 25);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 26) + 0), 26);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 26) + 1), 26);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 27) + 0), 27);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 27) + 1), 27);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 28) + 0), 28);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 28) + 1), 28);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 29) + 0), 29);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1 << 29) + 1), 29);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 30) + 0), 30u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 30) + 1), 30u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 31) + 0), 31u);
    BOOST_REQUIRE_EQUAL(system::floored_log2((1u << 31) + 1), 31u);
}

BOOST_AUTO_TEST_CASE(math__floored_log2__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(system::floored_log2(system::power2(15)), 15);
    BOOST_REQUIRE_EQUAL(system::floored_log2(system::power2(15u)), 15u);
}

BOOST_AUTO_TEST_CASE(math__floored_log2__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(system::floored_log2(system::power(2, 15)), 15);
    BOOST_REQUIRE_EQUAL(system::floored_log2(system::power(2u, 15u)), 15u);
}

// pow2

BOOST_AUTO_TEST_CASE(math__pow2__0__1)
{
    BOOST_REQUIRE_EQUAL(system::power2(0), 1);
    BOOST_REQUIRE_EQUAL(system::power2(0u), 1u);
}

BOOST_AUTO_TEST_CASE(math__pow2__1__2)
{
    BOOST_REQUIRE_EQUAL(system::power2(1), 2);
    BOOST_REQUIRE_EQUAL(system::power2(1u), 2u);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_1__0)
{
    BOOST_REQUIRE_EQUAL(system::power2(-1), 1 / 2);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_2__0)
{
    BOOST_REQUIRE_EQUAL(system::power2(-2), 1 / 4);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_3__underflow)
{
    BOOST_REQUIRE_EQUAL(system::power2(-3), 0);
}

BOOST_AUTO_TEST_CASE(math__pow2__15__0x8000)
{
    BOOST_REQUIRE_EQUAL(system::power2<uint16_t>(15), 0x8000u);
}

BOOST_AUTO_TEST_CASE(math__pow2__overflow__0)
{
    BOOST_REQUIRE_EQUAL(system::power2<uint16_t>(16), 0u);
}

// power

BOOST_AUTO_TEST_CASE(math__power__0_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(0, 0), 1);
    BOOST_REQUIRE_EQUAL(system::power(0u, 0u), 1u);
}

BOOST_AUTO_TEST_CASE(math__power__0_1__0)
{
    BOOST_REQUIRE_EQUAL(system::power(0, 1), 0);
    BOOST_REQUIRE_EQUAL(system::power(0u, 1u), 0u);
}

BOOST_AUTO_TEST_CASE(math__power__1_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(1, 0), 1);
    BOOST_REQUIRE_EQUAL(system::power(1u, 0u), 1u);
}

BOOST_AUTO_TEST_CASE(math__power__1_1__1)
{
    BOOST_REQUIRE_EQUAL(system::power(1, 1), 1);
    BOOST_REQUIRE_EQUAL(system::power(1u, 1u), 1u);
}

BOOST_AUTO_TEST_CASE(math__power__3_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(3, 0), 1);
    BOOST_REQUIRE_EQUAL(system::power(3u, 0u), 1u);
}

BOOST_AUTO_TEST_CASE(math__power__1_negative_1__1)
{
    BOOST_REQUIRE_EQUAL(system::power(1, -1), 1 / 1);
}

BOOST_AUTO_TEST_CASE(math__power__3_negative_1__0)
{
    BOOST_REQUIRE_EQUAL(system::power(3, -1), 1 / 3);
}

BOOST_AUTO_TEST_CASE(math__power__3_negative_2__0)
{
    BOOST_REQUIRE_EQUAL(system::power(3, -2), 1 / 9);
}

BOOST_AUTO_TEST_CASE(math__power__3_negative_3__0)
{
    BOOST_REQUIRE_EQUAL(system::power(3, -3), 1 / 81);
}

BOOST_AUTO_TEST_CASE(math__power__3_10__0xe6a9)
{
    BOOST_REQUIRE_EQUAL(system::power(3, 10), 0xe6a9);
    BOOST_REQUIRE_EQUAL(system::power(3u, 10u), 0xe6a9u);
}

BOOST_AUTO_TEST_CASE(math__power__overflow__expected)
{
    BOOST_REQUIRE_EQUAL(system::power<uint16_t>(3, 11), 0xb3fbu);
}

BOOST_AUTO_TEST_CASE(math__power__2_16__equals_pow2_16)
{
    BOOST_REQUIRE_EQUAL(system::power(2, 16), system::power2(16));
    BOOST_REQUIRE_EQUAL(system::power(2u, 16u), system::power2(16u));
}

BOOST_AUTO_TEST_CASE(math__power__negative_1_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(-1, 0), 1);
}

BOOST_AUTO_TEST_CASE(math__power__negative_1_1__negative_1)
{
    BOOST_REQUIRE_EQUAL(system::power(-1, 1), -1 / 1);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, 0), 1);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_1__negative_3)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, 1), -3);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_negative_1__0)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, -1), 1 / -3);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_negative_2__0)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, -2), 1 / 9);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_negative_3__0)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, -3), 1 / -81);
}

BOOST_AUTO_TEST_CASE(math__power__negative_3_10__0xe6a9)
{
    BOOST_REQUIRE_EQUAL(system::power(-3, 10), 0xe6a9);
}

BOOST_AUTO_TEST_CASE(math__power__negative_overflow__expected)
{
    BOOST_REQUIRE_EQUAL(system::power<uint16_t>(-3, 11), 0x4c05u);
}

#endif // POWERS

#ifdef MATH_MODULO

template <typename Dividend, typename Divisor>
bool ceilinged_identity(Dividend x, Divisor y)
{
    // Cast avoids conversion of left side to Divisor type.
    // This otherwise leads to comparison of unsigned types warnings.
    return ceilinged_divide(x, y) * static_cast<Dividend>(y) + ceilinged_modulo(x, y) == x;
}

template <typename Dividend, typename Divisor>
bool floored_identity(Dividend x, Divisor y)
{
    // Cast avoids conversion of left side to Divisor type.
    // This otherwise leads to comparison of unsigned types warnings.
    return floored_divide(x, y) * static_cast<Dividend>(y) + floored_modulo(x, y) == x;
}

template <typename Dividend, typename Divisor>
bool truncated_identity(Dividend x, Divisor y)
{
    // Cast avoids conversion of left side to Divisor type.
    // This otherwise leads to comparison of unsigned types warnings.
    return  truncated_divide(x, y) * static_cast<Dividend>(y) + truncated_modulo(x, y) == x;
}

// Return values are typed by the dividend, consistent with native operators.

// ceilinged_modulo

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 % 1, 0);

    BOOST_REQUIRE_EQUAL(ceilinged_modulo(0, 1), 0);

    BOOST_REQUIRE(ceilinged_identity(0, 1));
    BOOST_REQUIRE(ceilinged_identity(0u, 1));
    BOOST_REQUIRE(ceilinged_identity(0, 1u));
    BOOST_REQUIRE(ceilinged_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_positives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(42 % 2, 0);

    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 2), 0);

    BOOST_REQUIRE(ceilinged_identity(42, 2));
    BOOST_REQUIRE(ceilinged_identity(42u, 2));
    BOOST_REQUIRE(ceilinged_identity(42, 2u));
    BOOST_REQUIRE(ceilinged_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 2), 0);

    BOOST_REQUIRE(ceilinged_identity(-42, 2));
    BOOST_REQUIRE(ceilinged_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -2), 0);

    BOOST_REQUIRE(ceilinged_identity(42, -2));
    BOOST_REQUIRE(ceilinged_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negatives__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -2), 0);

    BOOST_REQUIRE(ceilinged_identity(-42, -2));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__positives__expected)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 % 8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 8), -6);

    BOOST_REQUIRE(ceilinged_identity(42, 8));
    BOOST_REQUIRE(ceilinged_identity(42u, 8));
    BOOST_REQUIRE(ceilinged_identity(42, 8u));
    BOOST_REQUIRE(ceilinged_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 8), -2);

    BOOST_REQUIRE(ceilinged_identity(-42, 8));
    BOOST_REQUIRE(ceilinged_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -8), 2);

    BOOST_REQUIRE(ceilinged_identity(42, -8));
    BOOST_REQUIRE(ceilinged_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negatives__expected)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -8), 6);

    BOOST_REQUIRE(ceilinged_identity(-42, -8));
}

// floored_modulo
// Same as Python (dividend % divisor).

BOOST_AUTO_TEST_CASE(math__floored_modulo__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 % 1, 0);

    // The python result of 0%1.
    BOOST_REQUIRE_EQUAL(floored_modulo(0, 1), 0);

    BOOST_REQUIRE(floored_identity(0, 1));
    BOOST_REQUIRE(floored_identity(0u, 1));
    BOOST_REQUIRE(floored_identity(0, 1u));
    BOOST_REQUIRE(floored_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_positives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(42 % 2, 0);

    // The python result of 42%2.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, 2), 0);

    BOOST_REQUIRE(floored_identity(42, 2));
    BOOST_REQUIRE(floored_identity(42u, 2));
    BOOST_REQUIRE(floored_identity(42, 2u));
    BOOST_REQUIRE(floored_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);

    // The python result of -42%2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 2), 0);

    BOOST_REQUIRE(floored_identity(-42, 2));
    BOOST_REQUIRE(floored_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);

    // The python result of 42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -2), 0);

    BOOST_REQUIRE(floored_identity(42, -2));
    BOOST_REQUIRE(floored_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negatives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);

    // The python result of -42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -2), 0);

    BOOST_REQUIRE(floored_identity(-42, -2));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__positives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 % 8, 2);

    // The python result of 42%8.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, 8), 2);

    BOOST_REQUIRE(floored_identity(42, 8));
    BOOST_REQUIRE(floored_identity(42u, 8));
    BOOST_REQUIRE(floored_identity(42, 8u));
    BOOST_REQUIRE(floored_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negative_dividend__python)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);

    // The python result of -42%8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 8), 6);

    BOOST_REQUIRE(floored_identity(-42, 8));
    BOOST_REQUIRE(floored_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negative_divisor__python)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);

    // The python result of 42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -8), -6);

    BOOST_REQUIRE(floored_identity(42, -8));
    BOOST_REQUIRE(floored_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);

    // The python result of -42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -8), -2);

    BOOST_REQUIRE(floored_identity(-42, -8));
}

// truncated_modulo
// Same as c++ (dividend % divisor).

BOOST_AUTO_TEST_CASE(math__truncated_modulo__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(0, 1), 0 % 1);

    BOOST_REQUIRE(truncated_identity(0, 1));
    BOOST_REQUIRE(truncated_identity(0u, 1));
    BOOST_REQUIRE(truncated_identity(0, 1u));
    BOOST_REQUIRE(truncated_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 2), 42 % 2);

    BOOST_REQUIRE(truncated_identity(42, 2));
    BOOST_REQUIRE(truncated_identity(42u, 2));
    BOOST_REQUIRE(truncated_identity(42, 2u));
    BOOST_REQUIRE(truncated_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 2), -42 % 2);

    BOOST_REQUIRE(truncated_identity(-42, 2));
    BOOST_REQUIRE(truncated_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -2), 42 % -2);

    BOOST_REQUIRE(truncated_identity(42, -2));
    BOOST_REQUIRE(truncated_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -2), -42 % -2);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 8), 42 % 8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42u, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
    BOOST_REQUIRE(truncated_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 8), -42 % 8);

    BOOST_REQUIRE(truncated_identity(-42, 8));
    BOOST_REQUIRE(truncated_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -8), 42 % -8);

    BOOST_REQUIRE(truncated_identity(42, -8));
    BOOST_REQUIRE(truncated_identity(42, -8));
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -8), -42 % -8);

    BOOST_REQUIRE(truncated_identity(-42, -8));
}

#endif // MATH_MODULO

#ifdef MATH_DIVIDE

// ceilinged_divide
// Same as Python -(numerator//-denominator).

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(0, 1), 0);

    BOOST_REQUIRE(ceilinged_identity(0, 1));
    BOOST_REQUIRE(ceilinged_identity(0u, 1));
    BOOST_REQUIRE(ceilinged_identity(0, 1u));
    BOOST_REQUIRE(ceilinged_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__1_negative_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(1, -1), -1);

    BOOST_REQUIRE(ceilinged_identity(1, -1));
    BOOST_REQUIRE(ceilinged_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_1_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1 / 1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-1, 1), -1);

    BOOST_REQUIRE(ceilinged_identity(-1, 1));
    BOOST_REQUIRE(ceilinged_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__no_remainder__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 2), 21);

    BOOST_REQUIRE(ceilinged_identity(42, 2));
    BOOST_REQUIRE(ceilinged_identity(42u, 2));
    BOOST_REQUIRE(ceilinged_identity(42, 2u));
    BOOST_REQUIRE(ceilinged_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__positives__python)
{
    // A common round-up hack works for both positives in c++.
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 8), (42 + (8 - 1)) / 8);

    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 8, 5);

    // The python result of -(42//-8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 8), 6);

    BOOST_REQUIRE(ceilinged_identity(42, 8));
    BOOST_REQUIRE(ceilinged_identity(42u, 8));
    BOOST_REQUIRE(ceilinged_identity(42, 8u));
    BOOST_REQUIRE(ceilinged_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_dividend__cpp)
{
    // A common round-up hack DOES NOT work for negative dividend in c++.
    BOOST_REQUIRE_NE(ceilinged_divide(-42, 8), (-42 + (8 - 1)) / 8);

    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 / 8, -5);

    // The python result of -(-42//-8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, 8), -5);

    BOOST_REQUIRE(ceilinged_identity(-42, 8));
    BOOST_REQUIRE(ceilinged_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_divisor__cpp)
{
    // A common round-up hack DOES NOT work for negative divisor in c++.
    BOOST_REQUIRE_NE(ceilinged_divide(42, -8), (42 + (-8 - 1)) / -8);
    BOOST_REQUIRE_NE(ceilinged_divide(42u, -8), (42 + (-8 - 1)) / -8);

    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 / -8, -5);

    // The python result of -(42//8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, -8), -5);

    BOOST_REQUIRE(ceilinged_identity(42, -8));
    BOOST_REQUIRE(ceilinged_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negatives__python)
{
    // A common round-up hack works for both negatives in c++.
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, -8), (-42 + (-8 - 1)) / -8);

    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 / -8, 5);

    // The python result of -(-42//8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, -8), 6);

    BOOST_REQUIRE(ceilinged_identity(-42, -8));
}

// floored_divide
// Same as Python (dividend // divisor).

BOOST_AUTO_TEST_CASE(math__floored_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(floored_divide(0, 1), 0);

    BOOST_REQUIRE(floored_identity(0, 1));
    BOOST_REQUIRE(floored_identity(0u, 1));
    BOOST_REQUIRE(floored_identity(0, 1u));
    BOOST_REQUIRE(floored_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__1_negative_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(1, -1), -1);

    BOOST_REQUIRE(floored_identity(1, -1));
    BOOST_REQUIRE(floored_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_1_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(-1, 1u), -1);

    BOOST_REQUIRE(floored_identity(-1, 1));
    BOOST_REQUIRE(floored_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__no_remainder__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 2), 21);

    BOOST_REQUIRE(floored_identity(42, 2));
    BOOST_REQUIRE(floored_identity(42u, 2));
    BOOST_REQUIRE(floored_identity(42, 2u));
    BOOST_REQUIRE(floored_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__positives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 8), 5);

    BOOST_REQUIRE(floored_identity(42, 8));
    BOOST_REQUIRE(floored_identity(42u, 8));
    BOOST_REQUIRE(floored_identity(42, 8u));
    BOOST_REQUIRE(floored_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 / 8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, 8), -6);

    BOOST_REQUIRE(floored_identity(-42, 8));
    BOOST_REQUIRE(floored_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 / -8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, -8), -6);

    BOOST_REQUIRE(floored_identity(42, -8));
    BOOST_REQUIRE(floored_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 / -8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, -8), 5);

    BOOST_REQUIRE(floored_identity(-42, -8));
}

// truncated_divide
// Same as c++ (dividend / divisor).

BOOST_AUTO_TEST_CASE(math__truncated_divide__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(0, 1), 0 / 1);

    BOOST_REQUIRE(truncated_identity(0, 1));
    BOOST_REQUIRE(truncated_identity(0u, 1));
    BOOST_REQUIRE(truncated_identity(0, 1u));
    BOOST_REQUIRE(truncated_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__1_negative_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(1, -1), 1 / -1);

    BOOST_REQUIRE(truncated_identity(1, -1));
    BOOST_REQUIRE(truncated_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_1_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-1, 1), -1 / 1);

    BOOST_REQUIRE(truncated_identity(-1, 1));
    BOOST_REQUIRE(truncated_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__no_remainder__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 2), 42 / 2);

    BOOST_REQUIRE(truncated_identity(42, 2));
    BOOST_REQUIRE(truncated_identity(42u, 2));
    BOOST_REQUIRE(truncated_identity(42, 2u));
    BOOST_REQUIRE(truncated_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 8), 42 / 8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42u, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
    BOOST_REQUIRE(truncated_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, 8), -42 / 8);

    BOOST_REQUIRE(truncated_identity(-42, 8));
    BOOST_REQUIRE(truncated_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, -8), 42 / -8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, -8), -42 / -8);

    BOOST_REQUIRE(truncated_identity(-42, -8));
}

#endif // MATH_DIVIDE

BOOST_AUTO_TEST_SUITE_END()

// Full matrix of conversions/signs (unsigned types avoided due to warnings).
// ----------------------------------------------------------------------------

// C++11: if the quotient x/y is representable in the type of the result:
// Identity: (x / y) * y + (x % y) = x

// Divide increment magnitude.
constexpr auto i = 1;

// Operand magnitudes.
constexpr auto x = 4;
constexpr auto y = 3;

// Zero to zero.
static_assert(0 / -y == 0, "0");
static_assert(0 / +y == 0, "0");
static_assert(0 % -y == 0, "0");
static_assert(0 % +y == 0, "0");

// ----------------------------------------------------------------------------

// Truncated divide:
static_assert(+x / +y == +1, "+");
static_assert(-x / -y == +1, "+");
static_assert(-x / +y == -1, "-");
static_assert(+x / -y == -1, "-");

// Truncated modulo:
static_assert(+x % +y == +1, "+");
static_assert(-x % -y == -1, "+");
static_assert(-x % +y == -1, "-");
static_assert(+x % -y == +1, "-");

// Truncated identity:
static_assert((+x / +y) * (+y) + (+x % +y) == +x, "+");
static_assert((-x / -y) * (-y) + (-x % -y) == -x, "+");
static_assert((-x / +y) * (+y) + (-x % +y) == -x, "-");
static_assert((+x / -y) * (-y) + (+x % -y) == +x, "-");

// ----------------------------------------------------------------------------

// Ceilinged divide: (increment truncated quotient if positive and remainder)
static_assert((+x / +y) + (((+x < 0) == (+y < 0)) ? ((+x % +y) != 0 ? +i : 0) : 0) == +1 + i, "+");
static_assert((-x / -y) + (((-x < 0) == (-y < 0)) ? ((-x % -y) != 0 ? +i : 0) : 0) == +1 + i, "+");
static_assert((-x / +y) + (((-x < 0) == (+y < 0)) ? ((-x % +y) != 0 ? +i : 0) : 0) == -1 + 0, "-");
static_assert((+x / -y) + (((+x < 0) == (-y < 0)) ? ((+x % -y) != 0 ? +i : 0) : 0) == -1 + 0, "-");

// Ceilinged modulo: (decrease truncated modulo by divisor if positive and remainder)
static_assert((+x % +y) - (((+x < 0) == (+y < 0)) ? +y : 0) == +1 - +y, "+");
static_assert((-x % -y) - (((-x < 0) == (-y < 0)) ? -y : 0) == -1 - -y, "+");
static_assert((-x % +y) - (((-x < 0) == (+y < 0)) ? +y : 0) == -1 - +0, "-");
static_assert((+x % -y) - (((+x < 0) == (-y < 0)) ? -y : 0) == +1 - -0, "-");

// Ceilinged identity
static_assert((+1 + i) * (+y) + (+1 - +y) == +x, "+");
static_assert((+1 + i) * (-y) + (-1 - -y) == -x, "+");
static_assert((-1 + 0) * (+y) + (-1 - +0) == -x, "-");
static_assert((-1 + 0) * (-y) + (+1 - -0) == +x, "-");

// ----------------------------------------------------------------------------

// Floored divide: (decrement truncated quotient if negative and remainder)
static_assert((+x / +y) - (((+x < 0) != (+y < 0)) ? ((+x % +y) != 0 ? +i : 0) : 0) == +1 - 0, "+");
static_assert((-x / -y) - (((-x < 0) != (-y < 0)) ? ((-x % -y) != 0 ? +i : 0) : 0) == +1 - 0, "+");
static_assert((-x / +y) - (((-x < 0) != (+y < 0)) ? ((-x % +y) != 0 ? +i : 0) : 0) == -1 - i, "-");
static_assert((+x / -y) - (((+x < 0) != (-y < 0)) ? ((+x % -y) != 0 ? +i : 0) : 0) == -1 - i, "-");

// Floored modulo: (increase truncated modulo by divisor if negative and remainder)
static_assert((+x % +y) + (((+x < 0) != (+y < 0)) ? +y : 0) == +1 + +0, "+");
static_assert((-x % -y) + (((-x < 0) != (-y < 0)) ? -y : 0) == -1 + +0, "+");
static_assert((-x % +y) + (((-x < 0) != (+y < 0)) ? +y : 0) == -1 + +y, "-");
static_assert((+x % -y) + (((+x < 0) != (-y < 0)) ? -y : 0) == +1 + -y, "-");

// Floored identity:
static_assert((+1 - 0) * (+y) + (+1 + +0) == +x, "+");
static_assert((+1 - 0) * (-y) + (-1 + +0) == -x, "+");
static_assert((-1 - i) * (+y) + (-1 + +y) == -x, "-");
static_assert((-1 - i) * (-y) + (+1 + -y) == +x, "-");

// Verify modulo adjustment.
static_assert(((+x / +y) + 1) * +y + ((+x % +y) - +y) == +x, "+1++");
static_assert(((+x / +y) - 1) * +y + ((+x % +y) + +y) == +x, "-1++");
static_assert(((-x / -y) + 1) * -y + ((-x % -y) - -y) == -x, "+1--");
static_assert(((-x / -y) - 1) * -y + ((-x % -y) + -y) == -x, "-1--");
static_assert(((-x / +y) + 1) * +y + ((-x % +y) - +y) == -x, "+1-+");
static_assert(((-x / +y) - 1) * +y + ((-x % +y) + +y) == -x, "-1-+");
static_assert(((+x / -y) + 1) * -y + ((+x % -y) - -y) == +x, "+1+-");
static_assert(((+x / -y) - 1) * -y + ((+x % -y) + -y) == +x, "-1+-");

// ----------------------------------------------------------------------------