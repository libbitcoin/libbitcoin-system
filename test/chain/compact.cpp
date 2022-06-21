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

BOOST_AUTO_TEST_SUITE(compact_tests)

using namespace system::chain;

constexpr uint32_t factory(int32_t logical_exponent, bool negative, uint32_t mantissa) noexcept
{
    // The exponent of a non-zero mantissa is valid from -3 to +29.
    BC_ASSERT(logical_exponent >= -3 && logical_exponent <= 252);

    // The mantissa may not intrude on the sign bit or the exponent.
    BC_ASSERT(is_zero(mantissa & 0xff800000));

    // The logical 0 exponent is represented as 3, so consider that the decimal point.
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

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(-3, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(-3, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(-3, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(-3, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x7fffff))), 0x00000000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(-3, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(-3, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_valid(factory(-3, false, 0xffff))); // precision loss
    BOOST_CHECK(compact::is_valid(factory(-3, false, 0x7fffff))); // precision loss
}

BOOST_AUTO_TEST_CASE(compact__expand__negative2_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(-2, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(-2, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(-2, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(-2, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x7fffff))), 0x017f0000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(-2, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(-2, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_valid(factory(-2, false, 0xffff))); // precision loss
    BOOST_CHECK(compact::is_valid(factory(-2, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__negative1_exponent_normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(-1, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(-1, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(-1, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(-1, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0xffff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x7fffff))), 0x027fff00u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(-1, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(-1, false, 0xff))); // precision loss
    BOOST_CHECK(compact::is_valid(factory(-1, false, 0xffff)));
    BOOST_CHECK(compact::is_valid(factory(-1, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__zero_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(0, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(0, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(0, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(0, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0xff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0xffff))), 0x0300ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x7fffff))), 0x037fffffu);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(0, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(0, false, 0xff)));
    BOOST_CHECK(compact::is_valid(factory(0, false, 0xffff)));
    BOOST_CHECK(compact::is_valid(factory(0, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive1_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(1, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(1, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(1, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(1, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0xff))), 0x0300ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0xffff))), 0x0400ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x7fffff))), 0x047fffffu);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(1, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(1, false, 0xff)));
    BOOST_CHECK(compact::is_valid(factory(1, false, 0xffff)));
    BOOST_CHECK(compact::is_valid(factory(1, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive29_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(29, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(29, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(29, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(29, true, 0x7fffff)));

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0xff))), 0x1f00ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0xffff))), 0x2000ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x7fffff))), 0x207fffffu);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(29, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(29, false, 0xff)));
    BOOST_CHECK(compact::is_valid(factory(29, false, 0xffff)));
    BOOST_CHECK(compact::is_valid(factory(29, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive30_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(30, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(30, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(30, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(30, true, 0x7fffff)));

    // positive, overflow above 0xffff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0xff))), 0x2000ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0xffff))), 0x2100ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x7fffff))), 0x00000000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(30, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(30, false, 0xff)));
    BOOST_CHECK(compact::is_valid(factory(30, false, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(30, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive31_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(31, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(31, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(31, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(31, true, 0x7fffff)));

    // positive, overflow above 0xff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0xff))), 0x2100ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x7fffff))), 0x00000000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(31, false, 0)));
    BOOST_CHECK(compact::is_valid(factory(31, false, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(31, false, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(31, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive32_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(32, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(32, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(32, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(32, true, 0x7fffff)));

    // positive, always overflow
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x7fffff))), 0x00000000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(32, false, 0)));
    BOOST_CHECK(!compact::is_valid(factory(32, false, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(32, false, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(32, false, 0x7fffff)));
}

BOOST_AUTO_TEST_CASE(compact__expand__positive252_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x7fffff))), 0x00000000u);

    // !is_valid
    BOOST_CHECK(!compact::is_valid(factory(252, true, 0)));
    BOOST_CHECK(!compact::is_valid(factory(252, true, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(252, true, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(252, true, 0x7fffff)));

    // positive, always overflow.
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0xff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0xffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x7fffff))), 0x00000000u);

    // is_valid?
    BOOST_CHECK(!compact::is_valid(factory(252, false, 0)));
    BOOST_CHECK(!compact::is_valid(factory(252, false, 0xff)));
    BOOST_CHECK(!compact::is_valid(factory(252, false, 0xffff)));
    BOOST_CHECK(!compact::is_valid(factory(252, false, 0x7fffff)));
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
