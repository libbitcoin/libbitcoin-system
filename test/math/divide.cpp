/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(divide_tests)

#define DIVIDE_MODULO 1
#define DIVIDE_DIVIDE 1
#define DIVIDE_ASSERT 0

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

#ifdef DIVIDE_MODULO

// ceilinged_modulo

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 % 1, 0);

    BOOST_REQUIRE_EQUAL(ceilinged_modulo(0, 1), 0);

    BOOST_REQUIRE(ceilinged_identity(0, 1));
    BOOST_REQUIRE(ceilinged_identity(0u, 1));
    BOOST_REQUIRE(ceilinged_identity(0, 1u));
    BOOST_REQUIRE(ceilinged_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__no_remainder_positives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(42 % 2, 0);

    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 2), 0);

    BOOST_REQUIRE(ceilinged_identity(42, 2));
    BOOST_REQUIRE(ceilinged_identity(42u, 2));
    BOOST_REQUIRE(ceilinged_identity(42, 2u));
    BOOST_REQUIRE(ceilinged_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 2), 0);

    BOOST_REQUIRE(ceilinged_identity(-42, 2));
    BOOST_REQUIRE(ceilinged_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -2), 0);

    BOOST_REQUIRE(ceilinged_identity(42, -2));
    BOOST_REQUIRE(ceilinged_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__no_remainder_negatives__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -2), 0);

    BOOST_REQUIRE(ceilinged_identity(-42, -2));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__positives__expected)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 % 8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, 8), -6);

    BOOST_REQUIRE(ceilinged_identity(42, 8));
    BOOST_REQUIRE(ceilinged_identity(42u, 8));
    BOOST_REQUIRE(ceilinged_identity(42, 8u));
    BOOST_REQUIRE(ceilinged_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, 8), -2);

    BOOST_REQUIRE(ceilinged_identity(-42, 8));
    BOOST_REQUIRE(ceilinged_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(42, -8), 2);

    BOOST_REQUIRE(ceilinged_identity(42, -8));
    BOOST_REQUIRE(ceilinged_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_modulo__negatives__expected)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);
    BOOST_REQUIRE_EQUAL(ceilinged_modulo(-42, -8), 6);

    BOOST_REQUIRE(ceilinged_identity(-42, -8));
}

// floored_modulo
// Same as Python (dividend % divisor).

BOOST_AUTO_TEST_CASE(divide__floored_modulo__0_1__cpp)
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

BOOST_AUTO_TEST_CASE(divide__floored_modulo__no_remainder_positives__cpp)
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

BOOST_AUTO_TEST_CASE(divide__floored_modulo__no_remainder_negative_dividend__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-42 % 2, 0);

    // The python result of -42%2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 2), 0);

    BOOST_REQUIRE(floored_identity(-42, 2));
    BOOST_REQUIRE(floored_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(divide__floored_modulo__no_remainder_negative_divisor__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 % -2, 0);

    // The python result of 42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -2), 0);

    BOOST_REQUIRE(floored_identity(42, -2));
    BOOST_REQUIRE(floored_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(divide__floored_modulo__no_remainder_negatives__cpp)
{
    // No remainder and positive quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-42 % -2, 0);

    // The python result of -42%-2.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -2), 0);

    BOOST_REQUIRE(floored_identity(-42, -2));
}

BOOST_AUTO_TEST_CASE(divide__floored_modulo__positives__cpp)
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

BOOST_AUTO_TEST_CASE(divide__floored_modulo__negative_dividend__python)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 % 8, -2);

    // The python result of -42%8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, 8), 6);

    BOOST_REQUIRE(floored_identity(-42, 8));
    BOOST_REQUIRE(floored_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__floored_modulo__negative_divisor__python)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 % -8, 2);

    // The python result of 42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(42, -8), -6);

    BOOST_REQUIRE(floored_identity(42, -8));
    BOOST_REQUIRE(floored_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(divide__floored_modulo__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 % -8, -2);

    // The python result of -42%-8.
    BOOST_REQUIRE_EQUAL(floored_modulo(-42, -8), -2);

    BOOST_REQUIRE(floored_identity(-42, -8));
}

// truncated_modulo
// Same as c++ (dividend % divisor).

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(0, 1), 0 % 1);

    BOOST_REQUIRE(truncated_identity(0, 1));
    BOOST_REQUIRE(truncated_identity(0u, 1));
    BOOST_REQUIRE(truncated_identity(0, 1u));
    BOOST_REQUIRE(truncated_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__no_remainder_positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 2), 42 % 2);

    BOOST_REQUIRE(truncated_identity(42, 2));
    BOOST_REQUIRE(truncated_identity(42u, 2));
    BOOST_REQUIRE(truncated_identity(42, 2u));
    BOOST_REQUIRE(truncated_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__no_remainder_negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 2), -42 % 2);

    BOOST_REQUIRE(truncated_identity(-42, 2));
    BOOST_REQUIRE(truncated_identity(-42, 2u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__no_remainder_negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -2), 42 % -2);

    BOOST_REQUIRE(truncated_identity(42, -2));
    BOOST_REQUIRE(truncated_identity(42u, -2));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__no_remainder_negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -2), -42 % -2);
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, 8), 42 % 8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42u, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
    BOOST_REQUIRE(truncated_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, 8), -42 % 8);

    BOOST_REQUIRE(truncated_identity(-42, 8));
    BOOST_REQUIRE(truncated_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(42, -8), 42 % -8);

    BOOST_REQUIRE(truncated_identity(42, -8));
    BOOST_REQUIRE(truncated_identity(42, -8));
}

BOOST_AUTO_TEST_CASE(divide__truncated_modulo__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_modulo(-42, -8), -42 % -8);

    BOOST_REQUIRE(truncated_identity(-42, -8));
}

#endif // DIVIDE_MODULO

#ifdef DIVIDE_DIVIDE

// ceilinged_divide
// Same as Python -(numerator//-denominator).

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(0, 1), 0);

    BOOST_REQUIRE(ceilinged_identity(0, 1));
    BOOST_REQUIRE(ceilinged_identity(0u, 1));
    BOOST_REQUIRE(ceilinged_identity(0, 1u));
    BOOST_REQUIRE(ceilinged_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__1_negative_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(1, -1), -1);

    BOOST_REQUIRE(ceilinged_identity(1, -1));
    BOOST_REQUIRE(ceilinged_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__negative_1_1__cpp)
{
    // No remainder and negative quotient are same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1 / 1);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(-1, 1), -1);

    BOOST_REQUIRE(ceilinged_identity(-1, 1));
    BOOST_REQUIRE(ceilinged_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__no_remainder__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(ceilinged_divide(42, 2), 21);

    BOOST_REQUIRE(ceilinged_identity(42, 2));
    BOOST_REQUIRE(ceilinged_identity(42u, 2));
    BOOST_REQUIRE(ceilinged_identity(42, 2u));
    BOOST_REQUIRE(ceilinged_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__positives__python)
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

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__negative_dividend__cpp)
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

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__negative_divisor__cpp)
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

BOOST_AUTO_TEST_CASE(divide__ceilinged_divide__negatives__python)
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

BOOST_AUTO_TEST_CASE(divide__floored_divide__0_1__cpp)
{
    // Zero is same as c++.
    BOOST_REQUIRE_EQUAL(0 / 1, 0);
    BOOST_REQUIRE_EQUAL(floored_divide(0, 1), 0);

    BOOST_REQUIRE(floored_identity(0, 1));
    BOOST_REQUIRE(floored_identity(0u, 1));
    BOOST_REQUIRE(floored_identity(0, 1u));
    BOOST_REQUIRE(floored_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__1_negative_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(1 / -1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(1, -1), -1);

    BOOST_REQUIRE(floored_identity(1, -1));
    BOOST_REQUIRE(floored_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__negative_1_1__cpp)
{
    // No remainder is same as c++.
    BOOST_REQUIRE_EQUAL(-1 / 1, -1);
    BOOST_REQUIRE_EQUAL(floored_divide(-1, 1u), -1);

    BOOST_REQUIRE(floored_identity(-1, 1));
    BOOST_REQUIRE(floored_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__no_remainder__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 2, 21);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 2), 21);

    BOOST_REQUIRE(floored_identity(42, 2));
    BOOST_REQUIRE(floored_identity(42u, 2));
    BOOST_REQUIRE(floored_identity(42, 2u));
    BOOST_REQUIRE(floored_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__positives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(42 / 8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, 8), 5);

    BOOST_REQUIRE(floored_identity(42, 8));
    BOOST_REQUIRE(floored_identity(42u, 8));
    BOOST_REQUIRE(floored_identity(42, 8u));
    BOOST_REQUIRE(floored_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__negative_dividend__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(-42 / 8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, 8), -6);

    BOOST_REQUIRE(floored_identity(-42, 8));
    BOOST_REQUIRE(floored_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__negative_divisor__expected)
{
    // Negative quotient is ceilinged in c++.
    BOOST_REQUIRE_EQUAL(42 / -8, -5);
    BOOST_REQUIRE_EQUAL(floored_divide(42, -8), -6);

    BOOST_REQUIRE(floored_identity(42, -8));
    BOOST_REQUIRE(floored_identity(42u, -8));
}

BOOST_AUTO_TEST_CASE(divide__floored_divide__negatives__cpp)
{
    // Positive quotient is floored in c++.
    BOOST_REQUIRE_EQUAL(-42 / -8, 5);
    BOOST_REQUIRE_EQUAL(floored_divide(-42, -8), 5);

    BOOST_REQUIRE(floored_identity(-42, -8));
}

// truncated_divide
// Same as c++ (dividend / divisor).

BOOST_AUTO_TEST_CASE(divide__truncated_divide__0_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(0, 1), 0 / 1);

    BOOST_REQUIRE(truncated_identity(0, 1));
    BOOST_REQUIRE(truncated_identity(0u, 1));
    BOOST_REQUIRE(truncated_identity(0, 1u));
    BOOST_REQUIRE(truncated_identity(0u, 1u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__1_negative_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(1, -1), 1 / -1);

    BOOST_REQUIRE(truncated_identity(1, -1));
    BOOST_REQUIRE(truncated_identity(1u, -1));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__negative_1_1__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-1, 1), -1 / 1);

    BOOST_REQUIRE(truncated_identity(-1, 1));
    BOOST_REQUIRE(truncated_identity(-1, 1u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__no_remainder__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 2), 42 / 2);

    BOOST_REQUIRE(truncated_identity(42, 2));
    BOOST_REQUIRE(truncated_identity(42u, 2));
    BOOST_REQUIRE(truncated_identity(42, 2u));
    BOOST_REQUIRE(truncated_identity(42u, 2u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__positives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, 8), 42 / 8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42u, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
    BOOST_REQUIRE(truncated_identity(42u, 8u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__negative_dividend__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, 8), -42 / 8);

    BOOST_REQUIRE(truncated_identity(-42, 8));
    BOOST_REQUIRE(truncated_identity(-42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__negative_divisor__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(42, -8), 42 / -8);

    BOOST_REQUIRE(truncated_identity(42, 8));
    BOOST_REQUIRE(truncated_identity(42, 8u));
}

BOOST_AUTO_TEST_CASE(divide__truncated_divide__negatives__cpp)
{
    BOOST_REQUIRE_EQUAL(truncated_divide(-42, -8), -42 / -8);

    BOOST_REQUIRE(truncated_identity(-42, -8));
}

#endif // DIVIDE_DIVIDE

#ifdef DIVIDE_ASSERT

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

// ----------------------------------------------------------------------------

// Adjustments (+/-1 always corresponds to -/+divisor).
static_assert(((+x / +y) + 1) * +y + ((+x % +y) - +y) == +x, "+1++");
static_assert(((+x / +y) - 1) * +y + ((+x % +y) + +y) == +x, "-1++");
static_assert(((-x / -y) + 1) * -y + ((-x % -y) - -y) == -x, "+1--");
static_assert(((-x / -y) - 1) * -y + ((-x % -y) + -y) == -x, "-1--");
static_assert(((-x / +y) + 1) * +y + ((-x % +y) - +y) == -x, "+1-+");
static_assert(((-x / +y) - 1) * +y + ((-x % +y) + +y) == -x, "-1-+");
static_assert(((+x / -y) + 1) * -y + ((+x % -y) - -y) == +x, "+1+-");
static_assert(((+x / -y) - 1) * -y + ((+x % -y) + -y) == +x, "-1+-");

#endif // DIVIDE_ASSERT

BOOST_AUTO_TEST_SUITE_END()
