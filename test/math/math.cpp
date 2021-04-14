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
#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc;
using namespace bc::system;

BOOST_AUTO_TEST_SUITE(math_tests)

// basics
// ----------------------------------------------------------------------------

// absolute

BOOST_AUTO_TEST_CASE(math__absolute__zero__0)
{
    BOOST_REQUIRE_EQUAL(system::absolute(0), 0);
}

BOOST_AUTO_TEST_CASE(math__absolute__42__42)
{
    BOOST_REQUIRE_EQUAL(system::absolute(42), 42);
}

BOOST_AUTO_TEST_CASE(math__absolute__negative_42__42)
{
    BOOST_REQUIRE_EQUAL(system::absolute(-42), 42);
}

// negative

BOOST_AUTO_TEST_CASE(math__negative__zero_zero__false)
{
    BOOST_REQUIRE(!system::negative(0, 0));
}

BOOST_AUTO_TEST_CASE(math__negative__zero_one__false)
{
    BOOST_REQUIRE(!system::negative(0, 0));
}

BOOST_AUTO_TEST_CASE(math__negative__one_zero__false)
{
    BOOST_REQUIRE(!system::negative(1, 0));
}

BOOST_AUTO_TEST_CASE(math__negative__one_one__false)
{
    BOOST_REQUIRE(!system::negative(1, 1));
}

BOOST_AUTO_TEST_CASE(math__negative__negative_one_one__true)
{
    BOOST_REQUIRE(system::negative(-1, 1));
}

BOOST_AUTO_TEST_CASE(math__negative__one_negative_one__true)
{
    BOOST_REQUIRE(system::negative(1, -1));
}

BOOST_AUTO_TEST_CASE(math__negative__max_unsigned_max_unsigned__false)
{
    BOOST_REQUIRE(!system::negative(max_uint32, max_uint32));
}

BOOST_AUTO_TEST_CASE(math__negative__max_unsigned_min_signed__true)
{
    BOOST_REQUIRE(system::negative(max_uint32, min_int32));
}

// remainder
// Same as c++ (dividend % divisor) != 0.

BOOST_AUTO_TEST_CASE(math__remainder__0_1__false)
{
    BOOST_REQUIRE(!system::remainder(0, 1));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_positives__false)
{
    BOOST_REQUIRE(!system::remainder(42, 2));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negative_dividend__false)
{
    BOOST_REQUIRE(!system::remainder(-42, 2));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negative_divisor__false)
{
    BOOST_REQUIRE(!system::remainder(42, -2));
}

BOOST_AUTO_TEST_CASE(math__remainder__no_remainder_negatives__false)
{
    BOOST_REQUIRE(!system::remainder(-42, -2));
}

BOOST_AUTO_TEST_CASE(math__remainder__positives__true)
{
    BOOST_REQUIRE(system::remainder(42, 8));
}

BOOST_AUTO_TEST_CASE(math__remainder__negative_dividend__true)
{
    BOOST_REQUIRE(system::remainder(-42, 8));
}

BOOST_AUTO_TEST_CASE(math__remainder__negative_divisor__true)
{
    BOOST_REQUIRE(system::remainder(42, -8));
}

BOOST_AUTO_TEST_CASE(math__remainder__negatives__true)
{
    BOOST_REQUIRE(system::remainder(-42, -8));
}

// exponents
// ----------------------------------------------------------------------------

// log2

BOOST_AUTO_TEST_CASE(math__log2__zero__undefined)
{
    BOOST_REQUIRE_EQUAL(system::log2(0), 0);
}

BOOST_AUTO_TEST_CASE(math__log2__negative__undefined)
{
    BOOST_REQUIRE_EQUAL(system::log2(-42), 0);
}

BOOST_AUTO_TEST_CASE(math__log2__maximums__sizeof_maximum_minus_one)
{
    BOOST_REQUIRE_EQUAL(system::log2(max_uint8), sizeof(uint8_t) * byte_bits - 1);
    BOOST_REQUIRE_EQUAL(system::log2(max_uint16), sizeof(uint16_t) * byte_bits - 1);
    BOOST_REQUIRE_EQUAL(system::log2(max_uint32), sizeof(uint32_t) * byte_bits - 1);
}

BOOST_AUTO_TEST_CASE(math__log2__powers_of_2__expected)
{
    BOOST_REQUIRE_EQUAL(system::log2(1u << 0), 0);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 1), 1);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 2), 2);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 3), 3);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 4), 4);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 5), 5);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 6), 6);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 7), 7);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 8), 8);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 9), 9);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 10), 10);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 11), 11);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 12), 12);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 13), 13);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 14), 14);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 15), 15);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 16), 16);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 17), 17);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 18), 18);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 19), 19);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 20), 20);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 21), 21);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 22), 22);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 23), 23);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 24), 24);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 25), 25);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 26), 26);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 27), 27);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 28), 28);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 29), 29);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 30), 30);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 31), 31);
}

BOOST_AUTO_TEST_CASE(math__log2__pow2__identity)
{
    BOOST_REQUIRE_EQUAL(system::log2(system::pow2(15)), 15);
}

BOOST_AUTO_TEST_CASE(math__log2__power_2__identity)
{
    BOOST_REQUIRE_EQUAL(system::log2(system::power(2, 15)), 15);
}

// pow2

BOOST_AUTO_TEST_CASE(math__pow2__0__1)
{
    BOOST_REQUIRE_EQUAL(system::pow2(0), 1);
}

BOOST_AUTO_TEST_CASE(math__pow2__1__2)
{
    BOOST_REQUIRE_EQUAL(system::pow2(1), 2);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_1__0)
{
    BOOST_REQUIRE_EQUAL(system::pow2(-1), 1 / 2);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_2__0)
{
    BOOST_REQUIRE_EQUAL(system::pow2(-2), 1 / 4);
}

BOOST_AUTO_TEST_CASE(math__pow2__negative_3__underflow)
{
    BOOST_REQUIRE_EQUAL(system::pow2(-3), 0);
}

BOOST_AUTO_TEST_CASE(math__pow2__15__0x8000)
{
    BOOST_REQUIRE_EQUAL(system::pow2<uint16_t>(15), 0x8000);
}

BOOST_AUTO_TEST_CASE(math__pow2__overflow__0)
{
    BOOST_REQUIRE_EQUAL(system::pow2<uint16_t>(16), 0);
}

// power

BOOST_AUTO_TEST_CASE(math__power__0_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(0, 0), 1);
}

BOOST_AUTO_TEST_CASE(math__power__0_1__0)
{
    BOOST_REQUIRE_EQUAL(system::power(0, 1), 0);
}

BOOST_AUTO_TEST_CASE(math__power__1_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(1, 0), 1);
}

BOOST_AUTO_TEST_CASE(math__power__1_1__1)
{
    BOOST_REQUIRE_EQUAL(system::power(1, 1), 1);
}

BOOST_AUTO_TEST_CASE(math__power__3_0__1)
{
    BOOST_REQUIRE_EQUAL(system::power(3, 0), 1);
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
}

BOOST_AUTO_TEST_CASE(math__power__overflow__expected)
{
    BOOST_REQUIRE_EQUAL(system::power<uint16_t>(3, 11), 0xb3fb);
}

BOOST_AUTO_TEST_CASE(math__power__2_16__equals_pow2_16)
{
    BOOST_REQUIRE_EQUAL(system::power(2, 16), system::pow2(16));
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
    BOOST_REQUIRE_EQUAL(system::power<uint16_t>(-3, 11), 0x4c05);
}

// modulos
// ----------------------------------------------------------------------------

// ceilinged_modulo

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 % 1, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(0, 1), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_positives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(42 % 2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 2), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 2), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -2), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__no_remainder_negatives__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -2), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__positives__xxx)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 % 8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 8), 10);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 8), -2);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -8), 2);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_modulo__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -8), -10);
}

// floored_modulo
// Same as Python (dividend % divisor).

BOOST_AUTO_TEST_CASE(math__floored_modulo__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 % 1, 0);

    // The python result of 0%1.
    BOOST_REQUIRE_EQUAL(floored_modulo(0, 1), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_positives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(42 % 2, 0);

    // The python result of 42%2.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, 2), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);

    // The python result of -42%2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 2), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);

    // The python result of 42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -2), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__no_remainder_negatives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);

    // The python result of -42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -2), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__positives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 % 8, 2);

    // The python result of 42%8.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, 8), 2);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);

    // The python result of -42%8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 8), 6);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);

    // The python result of 42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -8), -6);
}

BOOST_AUTO_TEST_CASE(math__floored_modulo__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);

    // The python result of -42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -8), -2);
}

// truncated_modulo
// Same as c++ (dividend % divisor).

BOOST_AUTO_TEST_CASE(math__truncated_modulo__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(0, 1), 0 % 1);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 2), 42 % 2);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 2), -42 % 2);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -2), 42 % -2);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__no_remainder_negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -2), -42 % -2);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 8), 42 % 8);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 8), -42 % 8);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -8), 42 % -8);
}

BOOST_AUTO_TEST_CASE(math__truncated_modulo__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -8), -42 % -8);
}

// divides
// ----------------------------------------------------------------------------

// ceilinged_divide
// Same as Python -(numerator//-denominator).

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(0, 1), 0);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__1_negative_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(1, -1), -1);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_1_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1 / 1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-1, 1), -1);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__no_remainder__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 2), 21);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__positives__expected)
{
    // A common round-up hack works for both positives in c++.
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 8), (42 + (8 - 1)) / 8);

    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 8, 5);

    // The python result of -(42//-8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 8), 6);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_dividend__cpp)
{
    // A common round-up hack DOES NOT work for negative dividend in c++.
    BOOST_REQUIRE_NE(ceilinged_divide(-42, 8), (-42 + (8 - 1)) / 8);

    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 / 8, -5);

    // The python result of -(-42//-8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, 8), -5);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negative_divisor__cpp)
{
    // A common round-up hack DOES NOT work for negative divisor in c++.
    BOOST_REQUIRE_NE(ceilinged_divide(42, -8), (42 + (-8 - 1)) / -8);

    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 / -8, -5);

    // The python result of -(42//8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, -8), -5);
}

BOOST_AUTO_TEST_CASE(math__ceilinged_divide__negatives__expected)
{
    // A common round-up hack works for both negatives in c++.
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, -8), (-42 + (-8 - 1)) / -8);

    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 / -8, 5);

    // The python result of -(-42//8).
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-42, -8), 6);
}

// floored_divide
// Same as Python (dividend // divisor).

BOOST_AUTO_TEST_CASE(math__floored_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(floored_divide(0, 1), 0);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__1_negative_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(1, -1), -1);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_1_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(-1, 1), -1);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__no_remainder__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 2), 21);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__positives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 8), 5);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_dividend__negative_6)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 / 8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, 8), -6);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negative_divisor__negative_6)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 / -8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, -8), -6);
}

BOOST_AUTO_TEST_CASE(math__floored_divide__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 / -8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, -8), 5);
}

// truncated_divide
// Same as c++ (dividend / divisor).

BOOST_AUTO_TEST_CASE(math__truncated_divide__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(0, 1), 0 / 1);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__1_negative_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(1, -1), 1 / -1);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_1_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-1, 1), -1 / 1);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__no_remainder__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 2), 42 / 2);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 8), 42 / 8);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, 8), -42 / 8);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, -8), 42 / -8);
}

BOOST_AUTO_TEST_CASE(math__truncated_divide__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, -8), -42 / -8);
}

BOOST_AUTO_TEST_SUITE_END()
