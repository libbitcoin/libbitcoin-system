/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>

#include <cstdint>
#include <bitcoin/bitcoin.hpp>

BOOST_AUTO_TEST_SUITE(compact_number_tests)

using namespace bc;
using namespace bc::chain;

static const hash_digest primes
{
    {
        0x02, 0x03, 0x05, 0x07, 0x0b, 0x0d, 0x11, 0x13,
        0x17, 0x1d, 0x1f, 0x25, 0x29, 0x2b, 0x2f, 0x35,
        0x3b, 0x3d, 0x43, 0x47, 0x49, 0x4f, 0x53, 0x59,
        0x61, 0x65, 0x67, 0x6b, 0x6d, 0x71, 0x7f, 0x83
    }
};

static uint32_t compact(int32_t logical_exponent, bool negative, uint32_t mantissa)
{
    // The exponent of a non-zero mantissa is valid from -3 to +29.
    BITCOIN_ASSERT(logical_exponent >= -3 && logical_exponent <= 252);
    
    // The mantissa may not intrude on the sign bit or the exponent.
    BITCOIN_ASSERT((mantissa & 0xff800000) == 0);

    // The logical 0 exponent is represented as 3, so consider that the decimal point.
    const uint32_t exponent = logical_exponent + 3;

    // Construct the non-normalized compact value.
    return exponent << 24 | (negative ? 1 : 0) << 23 | mantissa;
}

// constructor1/normal

BOOST_AUTO_TEST_CASE(compact_number__constructor1__negative3_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, false, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, false, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-3, false, 0x7fffff)).normal(), 0x00000000);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__negative2_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, false, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, false, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-2, false, 0x7fffff)).normal(), 0x017f0000);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__negative1_exponent_normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, false, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, false, 0xffff)).normal(), 0x0200ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(-1, false, 0x7fffff)).normal(), 0x027fff00);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__zero_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, false, 0xff)).normal(), 0x0200ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, false, 0xffff)).normal(), 0x0300ffff); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(0, false, 0x7fffff)).normal(), 0x037fffff);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive1_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, false, 0xff)).normal(), 0x0300ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, false, 0xffff)).normal(), 0x0400ffff); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(1, false, 0x7fffff)).normal(), 0x047fffff);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive29_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, true, 0x7fffff)).normal(), 0x00000000);

    // positive
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, false, 0xff)).normal(), 0x1f00ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, false, 0xffff)).normal(), 0x2000ffff); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(29, false, 0x7fffff)).normal(), 0x207fffff);
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive30_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, true, 0x7fffff)).normal(), 0x00000000);

    // positive, overflow above 0xffff
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, false, 0xff)).normal(), 0x2000ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, false, 0xffff)).normal(), 0x2100ffff); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(30, false, 0x7fffff)).normal(), 0x00000000); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive31_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, true, 0x7fffff)).normal(), 0x00000000);

    // positive, overflow above 0xff
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, false, 0xff)).normal(), 0x2100ff00); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, false, 0xffff)).normal(), 0x00000000); // is_negated
    BOOST_REQUIRE_EQUAL(compact_number(compact(31, false, 0x7fffff)).normal(), 0x00000000); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive32_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, true, 0x7fffff)).normal(), 0x00000000);

    // positive, always overflow
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, false, 0xff)).normal(), 0x00000000); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, false, 0xffff)).normal(), 0x00000000); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact_number(compact(32, false, 0x7fffff)).normal(), 0x00000000); // is_overflowed
}

BOOST_AUTO_TEST_CASE(compact_number__constructor1__positive252_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, true, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, true, 0xff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, true, 0xffff)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, true, 0x7fffff)).normal(), 0x00000000);

    // positive, always overflow
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, false, 0)).normal(), 0x00000000);
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, false, 0xff)).normal(), 0x00000000); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, false, 0xffff)).normal(), 0x00000000); // is_overflowed
    BOOST_REQUIRE_EQUAL(compact_number(compact(252, false, 0x7fffff)).normal(), 0x00000000); // is_overflowed
}

// constructor2/uint256_t

BOOST_AUTO_TEST_CASE(compact_number__constructor2__big_value__round_trips)
{
    BOOST_REQUIRE(uint256_t(42) == compact_number(uint256_t(42)));
}

BOOST_AUTO_TEST_CASE(compact_number__constructor2__hash_big_zero__round_trips)
{
    BOOST_REQUIRE(uint256_t(null_hash) == compact_number({ null_hash }));
}

BOOST_AUTO_TEST_CASE(compact_number__constructor2__hash__round_trips)
{
    BOOST_REQUIRE(uint256_t(primes) == compact_number({ primes }));
}

BOOST_AUTO_TEST_SUITE_END()
