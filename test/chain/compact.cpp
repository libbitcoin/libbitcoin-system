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

BOOST_AUTO_TEST_SUITE(compact_number_tests)

using namespace system::chain;

constexpr uint32_t factory(int32_t logical_exponent, bool negative,
    uint32_t mantissa) noexcept
{
    // The exponent of a non-zero mantissa is valid from -3 to +29.
    BC_ASSERT(logical_exponent >= -3 && logical_exponent <= 252);

    // The mantissa may not intrude on the sign bit or the exponent.
    BC_ASSERT(is_zero(mantissa & 0xff800000));

    // The logical 0 exponent is represented as 3 (decimal point).
    const uint32_t exponent = logical_exponent + 3;

    // Construct the non-normalized compact value.
    return (exponent << 24) | ((negative ? 1 : 0) << 23) | mantissa;
}

// expand

BOOST_AUTO_TEST_CASE(compact__expand__negative3_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(-3, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(-3, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(-3, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(-3, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x7fffff))), 0x00000000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(-3, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(-3, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_compact(factory(-3, false, 0xffff))); // precision loss
    BOOST_CHECK(compact::is_compact(factory(-3, false, 0x7fffff))); // precision loss
}

BOOST_AUTO_TEST_CASE(compact__expand__negative2_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(-2, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(-2, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(-2, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(-2, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x7fffff))), 0x017f0000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(-2, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(-2, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_compact(factory(-2, false, 0xffff))); // precision loss
    BOOST_CHECK(compact::is_compact(factory(-2, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__negative1_exponent_normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(-1, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(-1, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(-1, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(-1, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0xffff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x7fffff))), 0x027fff00u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(-1, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(-1, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_compact(factory(-1, false, 0xffff)));
    BOOST_CHECK(compact::is_compact(factory(-1, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__zero_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(0, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(0, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(0, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(0, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0xff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0xffff))), 0x0300ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x7fffff))), 0x037fffffu);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(0, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(0, false, 0xff)));
    BOOST_CHECK(compact::is_compact(factory(0, false, 0xffff)));
    BOOST_CHECK(compact::is_compact(factory(0, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive1_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(1, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(1, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(1, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(1, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0xff))), 0x0300ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0xffff))), 0x0400ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x7fffff))), 0x047fffffu);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(1, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(1, false, 0xff)));
    BOOST_CHECK(compact::is_compact(factory(1, false, 0xffff)));
    BOOST_CHECK(compact::is_compact(factory(1, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive29_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(29, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(29, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(29, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(29, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0xff))), 0x1f00ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0xffff))), 0x2000ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x7fffff))), 0x207fffffu);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(29, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(29, false, 0xff)));
    BOOST_CHECK(compact::is_compact(factory(29, false, 0xffff)));
    BOOST_CHECK(compact::is_compact(factory(29, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive30_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(30, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(30, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(30, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(30, true, 0x7fffff)));

    // positive, overflow above 0xffff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0xff))), 0x2000ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0xffff))), 0x2100ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x7fffff))), 0x00000000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(30, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(30, false, 0xff)));
    BOOST_CHECK(compact::is_compact(factory(30, false, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(30, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive31_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(31, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(31, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(31, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(31, true, 0x7fffff)));

    // positive, overflow above 0xff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0xff))), 0x2100ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x7fffff))), 0x00000000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(31, false, 0)));
    BOOST_CHECK(compact::is_compact(factory(31, false, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(31, false, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(31, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive32_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(32, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(32, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(32, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(32, true, 0x7fffff)));

    // positive, always overflow
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x7fffff))), 0x00000000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(32, false, 0)));
    BOOST_CHECK(!compact::is_compact(factory(32, false, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(32, false, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(32, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive252_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x7fffff))), 0x00000000u);

    // !is_compact
    BOOST_CHECK(!compact::is_compact(factory(252, true, 0)));
    BOOST_CHECK(!compact::is_compact(factory(252, true, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(252, true, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(252, true, 0x7fffff)));

    // positive, always overflow.
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x7fffff))), 0x00000000u);

    // is_compact?
    BOOST_CHECK(!compact::is_compact(factory(252, false, 0)));
    BOOST_CHECK(!compact::is_compact(factory(252, false, 0xff)));
    BOOST_CHECK(!compact::is_compact(factory(252, false, 0xffff)));
    BOOST_CHECK(!compact::is_compact(factory(252, false, 0x7fffff)));
}

// expand/compress

BOOST_AUTO_TEST_CASE(compact__expand_compress__proof_of_work_limit__round_trips)
{
    const auto pow_limit = settings(chain::selection::mainnet).proof_of_work_limit;
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(pow_limit)), pow_limit);
}

BOOST_AUTO_TEST_CASE(compact__expand_compress__no_retarget_proof_of_work_limit__round_trips)
{
    const auto no_pow_limit = settings(chain::selection::regtest).proof_of_work_limit;
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(no_pow_limit)), no_pow_limit);
}

// compress/expand

BOOST_AUTO_TEST_CASE(compact__compress_expand__zero__round_trips)
{
    BOOST_CHECK_EQUAL(uint256_t(0), compact::expand(compact::compress(uint256_t(0))));
}

BOOST_AUTO_TEST_CASE(compact__expand_expand__42__round_trips)
{
    BOOST_CHECK_EQUAL(uint256_t(42), compact::expand(compact::compress(uint256_t(42))));
}

BOOST_AUTO_TEST_SUITE_END()

// Satoshi: for any exponent [0x00..0xff] and mantissa [0x00000000..0x007fffff].
//bool overflow = (mantissa != 0) &&
//(
//    (exponent > 34) ||
//    (exponent > 33 && mantissa > 0xff00) ||
//    (exponent > 32 && mantissa > 0xffff)
//);
// which can be normalized as:
//bool overflow =
//(
//    (exponent > 34 && mantissa > 0x0000) ||
//    (exponent > 33 && mantissa > 0xff00) ||
//    (exponent > 32 && mantissa > 0xffff)
//);
// and with shift-adjusted positive exponent [0:
//bool overflow =
//(
//    (exponent > 31 && mantissa > 0x0000) ||
//    (exponent > 32 && mantissa > 0xff00) ||
//    (exponent > 29 && mantissa > 0xffff)
//);

// > 0x0000 <= 0x00ff (overflow if exponent > 31 and ceilinged_log256(mantissa) > 1)
static_assert(ceilinged_log256(0x00000001ul) == 1);
static_assert(ceilinged_log256(0x000000fful) == 1);

// > 0x00ff <= 0xffff (eoverflow if exponent > 30 and ceilinged_log256(mantissa) > 2)
static_assert(ceilinged_log256(0x00000100ul) == 2);
static_assert(ceilinged_log256(0x0000fffful) == 2);

// > 0xffff (<= 0x007ffffful) (overflow if exponent > 29 and ceilinged_log256(mantissa) > 3)
static_assert(ceilinged_log256(0x00010000ul) == 3);
static_assert(ceilinged_log256(0x007ffffful) == 3);

////constexpr auto x0 = 32;
////constexpr auto x1 = 31;
////constexpr auto x2 = 30;
////constexpr auto x3 = 29;
////
////constexpr auto shift0 = x0 * 8;
////constexpr auto shift1 = x1 * 8;
////constexpr auto shift2 = x2 * 8;
////constexpr auto shift3 = x3 * 8;
////
////static_assert(shift0 == 256);
////static_assert(shift1 == 248);
////static_assert(shift2 == 240);
////static_assert(shift3 == 232);
////
////constexpr auto log0 = ceilinged_log2(0x00000000ul);
////constexpr auto log1 = ceilinged_log2(0x000000fful);
////constexpr auto log2 = ceilinged_log2(0x0000fffful);
////constexpr auto log3 = ceilinged_log2(0x007ffffful);
////
////static_assert(log0 == 0);
////static_assert(log1 == 8);
////static_assert(log2 == 16);
////static_assert(log3 == 23);
////
////constexpr auto width0 = log0 + shift0;
////constexpr auto width1 = log1 + shift1;
////constexpr auto width2 = log2 + shift2;
////constexpr auto width3 = log3 + shift3;
////
////static_assert(width0 == 256);
////static_assert(width1 == 256);
////static_assert(width2 == 256);
////static_assert(width3 == 255); // sign bit.
////
////constexpr auto max = 256;
////
////static_assert(width0 <= max);
////static_assert(width1 <= max);
////static_assert(width2 <= max);
////static_assert(width3 <= max);
////
////// zero y (excluded)
////static_assert(log0 + shift0 == max);    // x = 32 (y must be zero)
////static_assert(log0 + shift1 < max);     // x = 31
////static_assert(log0 + shift2 < max);     // x = 30
////static_assert(log0 + shift3 < max);     // x = 29
////
////// one byte y
////static_assert(log1 + shift0 > max);     // x = 32 (y must be zero)
////static_assert(log1 + shift1 == max);    // x = 31
////static_assert(log1 + shift2 < max);     // x = 30
////static_assert(log1 + shift3 < max);     // x = 29
////
////// two byte y
////static_assert(log2 + shift0 > max);     // x = 32 (y must be zero)
////static_assert(log2 + shift1 > max);     // x = 31 (fail)
////static_assert(log2 + shift2 == max);    // x = 30
////static_assert(log2 + shift3 < max);     // x = 29
////
////// three byte y (less sign bit)
////static_assert(log3 + shift0 > max);     // x = 32 (y must be zero)
////static_assert(log3 + shift1 > max);     // x = 31 (fail)
////static_assert(log3 + shift2 > max);     // x = 30 (fail)
////static_assert(log3 + shift3 +1 == max); // x = 29 (max - 1 because of sign bit)
////
////constexpr auto blog0 = ceilinged_log256(0x00000000ul);
////constexpr auto blog1 = ceilinged_log256(0x000000fful);
////constexpr auto blog2 = ceilinged_log256(0x0000fffful);
////constexpr auto blog3 = ceilinged_log256(0x007ffffful);
////////constexpr auto blog3 = ceilinged_log256(0x00fffffful); // without sign bit
////constexpr auto blog4 = ceilinged_log256(0xfffffffful);
////
////static_assert(blog0 == 0);
////static_assert(blog1 == 1);
////static_assert(blog2 == 2);
////static_assert(blog3 == 3);
////static_assert(blog4 == 4);
////
////constexpr auto bwidth0 = blog0 + x0;
////constexpr auto bwidth1 = blog1 + x1;
////constexpr auto bwidth2 = blog2 + x2;
////constexpr auto bwidth3 = blog3 + x3;
////constexpr auto bwidth4 = blog4 + 28;
////
////constexpr auto max_bytes = max / 8u;
////
////static_assert(bwidth0 == max_bytes); // x = 32
////static_assert(bwidth1 == max_bytes); // x = 31
////static_assert(bwidth2 == max_bytes); // x = 30
////static_assert(bwidth3 == max_bytes); // sign bit obscured by log256, but this value is excluded.
////static_assert(bwidth4 == max_bytes); // x = 29
////
////constexpr auto swidth0 = blog4 + 0;
////constexpr auto swidth1 = blog4 + 1;
////constexpr auto swidth2 = blog4 + 2;
////constexpr auto swidth3 = blog4 + 3;
////constexpr auto swidth4 = blog4 + 4;
////constexpr auto swidth5 = blog4 + 5;
////constexpr auto swidth6 = blog4 + 6;
////constexpr auto swidth7 = blog4 + 7;
////constexpr auto swidth8 = blog4 + 8;
////constexpr auto swidth9 = blog4 + 9;
////constexpr auto swidth10 = blog4 + 10;
////constexpr auto swidth11 = blog4 + 11;
////constexpr auto swidth12 = blog4 + 12;
////constexpr auto swidth13 = blog4 + 13;
////constexpr auto swidth14 = blog4 + 14;
////constexpr auto swidth15 = blog4 + 15;
////constexpr auto swidth16 = blog4 + 16;
////constexpr auto swidth17 = blog4 + 17;
////constexpr auto swidth18 = blog4 + 18;
////constexpr auto swidth19 = blog4 + 19;
////constexpr auto swidth20 = blog4 + 20;
////constexpr auto swidth21 = blog4 + 21;
////constexpr auto swidth22 = blog4 + 22;
////constexpr auto swidth23 = blog4 + 23;
////constexpr auto swidth24 = blog4 + 24;
////constexpr auto swidth25 = blog4 + 25;
////constexpr auto swidth26 = blog4 + 26;
////constexpr auto swidth27 = blog4 + 27;
////constexpr auto swidth28 = blog4 + 28;
////
////static_assert(swidth0 <= max_bytes);
////static_assert(swidth1 <= max_bytes);
////static_assert(swidth2 <= max_bytes);
////static_assert(swidth3 <= max_bytes);
////static_assert(swidth4 <= max_bytes);
////static_assert(swidth5 <= max_bytes);
////static_assert(swidth6 <= max_bytes);
////static_assert(swidth7 <= max_bytes);
////static_assert(swidth8 <= max_bytes);
////static_assert(swidth9 <= max_bytes);
////static_assert(swidth10 <= max_bytes);
////static_assert(swidth11 <= max_bytes);
////static_assert(swidth12 <= max_bytes);
////static_assert(swidth13 <= max_bytes);
////static_assert(swidth14 <= max_bytes);
////static_assert(swidth15 <= max_bytes);
////static_assert(swidth16 <= max_bytes);
////static_assert(swidth17 <= max_bytes);
////static_assert(swidth18 <= max_bytes);
////static_assert(swidth19 <= max_bytes);
////static_assert(swidth20 <= max_bytes);
////static_assert(swidth21 <= max_bytes);
////static_assert(swidth22 <= max_bytes);
////static_assert(swidth23 <= max_bytes);
////static_assert(swidth24 <= max_bytes);
////static_assert(swidth25 <= max_bytes);
////static_assert(swidth26 <= max_bytes);
////static_assert(swidth27 <= max_bytes);
////static_assert(swidth28 <= max_bytes);

// x > 31  &&  y > 0x0000 ||
// x > 30  &&  y > 0x00ff ||
// x > 29  &&  y > 0xffff

// x > 32 - 1  &&  log(y) > log(0x0000) ||
// x > 32 - 2  &&  log(y) > log(0x00ff) ||
// x > 32 - 3  &&  log(y) > log(0xffff)

// (x > (32 - 1) && log(y) == 1) ||
// (x > (32 - 1) && log(y) == 2)
// (x > (32 - 2) && log(y) == 3)

// 06.22.2022
// So, I found this subtle Satoshi bug:
//
////arith_uint256& arith_uint256::SetCompact(uint32_t nCompact, bool* pfNegative,
////    bool* pfOverflow)
////{
////    int nSize = nCompact >> 24;
////    uint32_t nWord = nCompact & 0x007fffff;
////    if (nSize <= 3) {
////        nWord >>= 8 * (3 - nSize);
////        *this = nWord;
////    } else {
////        *this = nWord;
////        *this <<= 8 * (nSize - 3);
////    }
////    if (pfNegative)
////        *pfNegative = nWord != 0 && (nCompact & 0x00800000) != 0;
////    if (pfOverflow)
////        *pfOverflow = nWord != 0 && ((nSize > 34) ||
////                                     (nWord > 0xff && nSize > 33) ||
////                                     (nWord > 0xffff && nSize > 32));
////    return *this;
////}
//
// The negative guard checks the value of nWord (the mantissa/y) against zero
// before checking for the sign bit in nCompact, from which the mantissa is
// obtained.
//
// Presumably as a performance optimization, the right shift when nSize is less
// than 3 is performed in nWord instead of in the big number (*this). But in the
// case of a left shift, nWord is unaffected by the shift (it's too small).
// Since the exponent and the mantissa are provided in block headers (as the
// bits field), they can be anything - this code validates them.
//
//The code that generates a compact number takes any value generated from the
// block hash and masks this sign bit, so it should never be set. Bit it is
// possible to set it and with a negative exponent, shift it away before it's
// tested.
//
//smaller values are higher work, but I haven't taken the time to figure out
// exactly where the inflection point comes in terms of block hashes, with the
// exponent going negative. It would be in the lowest order 3 bytes of a hash.
// I don't think we're there yet.
//
//000000000000000000058e6712450d2abc7d5aa413d21692e4fcb6ad5cb91273 (big-endian)

////inline uint256_t compact::expand(uint32_t small) noexcept
////{
////    const auto compact = compact::to_compact(small);
////
////    uint256_t big;
////
////    if (compact.exponent <= point)
////    {
////        // mantissa is altered here (as an optimization - it's also a bug):
////        compact.mantissa >>= ratio(point - compact.exponent);
////        big = compact.mantissa;
////    }
////    else
////    {
////        big = compact.mantissa;
////        big <<= ratio(compact.exponent - point);
////    }
////
////    // returned by reference.
////    // -0 is not discared here.
////    bool negative = !is_zero(compact.mantissa) && compact.negative;
////
////    // returned by reference.
////    // -0 is not discared here.
////    bool overflow = !is_zero(compact.mantissa) &&
////    (
////        (compact.exponent > 34 && compact.mantissa > 0x0000) ||
////        (compact.exponent > 33 && compact.mantissa > 0x00ff) ||
////        (compact.exponent > 32 && compact.mantissa > 0xffff)
////    );
////
////    // These are test hooks (parameterized return), zero is sufficient.
////    if (negative || overflow)
////        return 0;
////
////    return big;
////}
////
////inline uint32_t compact::compress(const uint256_t& big) noexcept
////{
////    int exponent = ceilinged_log256(big);
////
////    uint32_t compact = 0;
////
////    if (exponent <= 3)
////        compact = static_cast<mantissa_type>(big << ratio(point - exponent));
////    else
////        compact = static_cast<mantissa_type>(big >> ratio(exponent - point));
////
////    if (compact & 0x00800000)
////    {
////        compact >>= 8;
////        exponent++;
////    }
////    // This is a test hook (parameterized).
////    const auto negative = false;
////
////    // assemble compact (with parameterized negative).
////    compact |= exponent << mantissa_width;
////    compact |= (negative && (compact & 0x007fffff) ? 0x00800000 : 0);
////    return compact;
////}