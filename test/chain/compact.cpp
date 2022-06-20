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

#define PRIMES "020305070b0d1113171d1f25292b2f353b3d4347494f53596165676b6d717f83"
static const auto primes = base16_hash(PRIMES);

static uint32_t factory(int32_t logical_exponent, bool negative, uint32_t mantissa)
{
    // The exponent of a non-zero mantissa is valid from -3 to +29.
    BC_ASSERT(logical_exponent >= -3 && logical_exponent <= 252);

    // The mantissa may not intrude on the sign bit or the exponent.
    BC_ASSERT((mantissa & 0xff800000) == 0);

    // The logical 0 exponent is represented as 3, so consider that the decimal point.
    const uint32_t exponent = logical_exponent + 3;

    // Construct the non-normalized compact value.
    return exponent << 24 | (negative ? 1 : 0) << 23 | mantissa;
}

BOOST_AUTO_TEST_CASE(compact__constructor1__proof_of_work_limit__normalizes_unchanged)
{
    const auto pow_limit = settings(chain::selection::mainnet).proof_of_work_limit;
    BOOST_REQUIRE_EQUAL(compact(pow_limit).to_uint32(), pow_limit);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__no_retarget_proof_of_work_limit__normalizes_unchanged)
{
    const auto no_pow_limit = settings(chain::selection::regtest).proof_of_work_limit;
    BOOST_REQUIRE_EQUAL(compact(no_pow_limit).to_uint32(), no_pow_limit);
}

// constructor1/normal

BOOST_AUTO_TEST_CASE(compact__constructor1__negative3_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(-3, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(-3, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, false, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, false, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-3, false, 0x7fffff)).to_uint32(), 0x00000000u);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__negative2_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(-2, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(-2, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, false, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, false, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-2, false, 0x7fffff)).to_uint32(), 0x017f0000u);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__negative1_exponent_normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(-1, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-1, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-1, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-1, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(-1, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-1, false, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(-1, false, 0xffff)).to_uint32(), 0x0200ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(-1, false, 0x7fffff)).to_uint32(), 0x027fff00u);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__zero_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(0, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(0, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(0, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(0, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(0, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(0, false, 0xff)).to_uint32(), 0x0200ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(0, false, 0xffff)).to_uint32(), 0x0300ffffu); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(0, false, 0x7fffff)).to_uint32(), 0x037fffffu);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive1_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(1, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(1, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(1, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(1, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(1, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(1, false, 0xff)).to_uint32(), 0x0300ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(1, false, 0xffff)).to_uint32(), 0x0400ffffu); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(1, false, 0x7fffff)).to_uint32(), 0x047fffffu);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive29_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(29, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(29, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(29, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(29, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive
    BOOST_REQUIRE_EQUAL(compact(factory(29, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(29, false, 0xff)).to_uint32(), 0x1f00ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(29, false, 0xffff)).to_uint32(), 0x2000ffffu); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(29, false, 0x7fffff)).to_uint32(), 0x207fffffu);
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive30_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(30, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(30, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(30, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(30, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive, overflow above 0xffff
    BOOST_REQUIRE_EQUAL(compact(factory(30, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(30, false, 0xff)).to_uint32(), 0x2000ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(30, false, 0xffff)).to_uint32(), 0x2100ffffu); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(30, false, 0x7fffff)).to_uint32(), 0x00000000u); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive31_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(31, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(31, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(31, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(31, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive, overflow above 0xff
    BOOST_REQUIRE_EQUAL(compact(factory(31, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(31, false, 0xff)).to_uint32(), 0x2100ff00u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(31, false, 0xffff)).to_uint32(), 0x00000000u); // is_negated
    BOOST_REQUIRE_EQUAL(compact(factory(31, false, 0x7fffff)).to_uint32(), 0x00000000u); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive32_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(32, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(32, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(32, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(32, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive, always overflow
    BOOST_REQUIRE_EQUAL(compact(factory(32, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(32, false, 0xff)).to_uint32(), 0x00000000u); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact(factory(32, false, 0xffff)).to_uint32(), 0x00000000u); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact(factory(32, false, 0x7fffff)).to_uint32(), 0x00000000u); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact__constructor1__positive252_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact(factory(252, true, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(252, true, 0xff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(252, true, 0xffff)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(252, true, 0x7fffff)).to_uint32(), 0x00000000u);

    // positive, always overflow
    BOOST_REQUIRE_EQUAL(compact(factory(252, false, 0)).to_uint32(), 0x00000000u);
    BOOST_REQUIRE_EQUAL(compact(factory(252, false, 0xff)).to_uint32(), 0x00000000u); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact(factory(252, false, 0xffff)).to_uint32(), 0x00000000u); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact(factory(252, false, 0x7fffff)).to_uint32(), 0x00000000u); // is_overflowed
}

// constructor2/uint256_t

BOOST_AUTO_TEST_CASE(compact__constructor2__zero__round_trips)
{
    BOOST_REQUIRE(uint256_t(0) == compact(uint256_t(0)).to_uint256());
}

BOOST_AUTO_TEST_CASE(compact__constructor2__big_value__round_trips)
{
    BOOST_REQUIRE(uint256_t(42) == compact(uint256_t(42)).to_uint256());
}

BOOST_AUTO_TEST_CASE(compact__constructor2__hash__round_trips)
{
    BOOST_REQUIRE(to_uint256(primes) == compact(to_uint256(primes)).to_uint256());
}

BOOST_AUTO_TEST_SUITE_END()
