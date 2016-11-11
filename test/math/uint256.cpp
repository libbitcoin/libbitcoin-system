/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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

#include <sstream>
#include <string>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(uint256_tests)

#define MAX_HASH \
"ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
static const auto max_hash = hash_literal(MAX_HASH);

#define NEGATIVE1_HASH \
"8000000000000000000000000000000000000000000000000000000000000000"
static const auto negative_zero_hash = hash_literal(NEGATIVE1_HASH);

#define MOST_HASH \
"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
static const auto most_hash = hash_literal(MOST_HASH);

#define ODD_HASH \
"8437390223499ab234bf128e8cd092343485898923aaaaabbcbcc4874353fff4"
static const auto odd_hash = hash_literal(ODD_HASH);

#define HALF_HASH \
"00000000000000000000000000000000ffffffffffffffffffffffffffffffff"
static const auto half_hash = hash_literal(HALF_HASH);

#define QUARTER_HASH \
"000000000000000000000000000000000000000000000000ffffffffffffffff"
static const auto quarter_hash = hash_literal(QUARTER_HASH);

#define UNIT_HASH \
"0000000000000000000000000000000000000000000000000000000000000001"
static const auto unit_hash = hash_literal(UNIT_HASH);

#define ONES_HASH \
"0000000100000001000000010000000100000001000000010000000100000001"
static const auto ones_hash = hash_literal(ONES_HASH);

// constructors
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__constructor_default__always__equates_to_0)
{
    uint256_t minimum;
    BOOST_REQUIRE_EQUAL(minimum > 0, false);
    BOOST_REQUIRE_EQUAL(minimum < 0, false);
    BOOST_REQUIRE_EQUAL(minimum >= 0, true);
    BOOST_REQUIRE_EQUAL(minimum <= 0, true);
    BOOST_REQUIRE_EQUAL(minimum == 0, true);
    BOOST_REQUIRE_EQUAL(minimum != 0, false);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_move__42__equals_42)
{
    static const auto expected = 42u;
    static const uint256_t value(uint256_t{ expected });
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_copy__odd_hash__equals_odd_hash)
{
    static const uint256_t expected(odd_hash);
    static const uint256_t value(expected);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_uint32__minimum__equals_0)
{
    static const auto expected = 0u;
    static const uint256_t value(expected);
    BOOST_REQUIRE(value == expected);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_uint32__42__equals_42)
{
    static const auto expected = 42u;
    static const uint256_t value(expected);
    BOOST_REQUIRE(value == expected);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_uint32__maximum__equals_maximum)
{
    static const auto expected = max_uint32;
    static const uint256_t value(expected);
    BOOST_REQUIRE(value == expected);
}

BOOST_AUTO_TEST_CASE(uint256__constructor_hash__odd_hash__round_trips_odd_hash)
{
    static const auto& expected = odd_hash;
    static const uint256_t value(uint256_t{ expected });
    BOOST_REQUIRE(value.hash() == expected);
}

// bit_length
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__bit_length__null_hash__returns_0)
{
    static const uint256_t value{ null_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 0u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__unit_hash__returns_1)
{
    static const uint256_t value{ unit_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 1u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__quarter_hash__returns_64)
{
    static const uint256_t value{ quarter_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 64u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__half_hash__returns_128)
{
    static const uint256_t value{ half_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 128u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__most_hash__returns_255)
{
    static const uint256_t value{ most_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 255u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__negative_zero_hash__returns_256)
{
    static const uint256_t value{ negative_zero_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 256u);
}

BOOST_AUTO_TEST_CASE(uint256__bit_length__max_hash__returns_256)
{
    static const uint256_t value{ max_hash };
    BOOST_REQUIRE_EQUAL(value.bit_length(), 256u);
}

// byte_length
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__byte_length__null_hash__returns_0)
{
    static const uint256_t value{ null_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 0u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__unit_hash__returns_1)
{
    static const uint256_t value{ unit_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 1u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__quarter_hash__returns_8)
{
    static const uint256_t value{ quarter_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 8u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__half_hash__returns_16)
{
    static const uint256_t value{ half_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 16u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__most_hash__returns_32)
{
    static const uint256_t value{ most_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 32u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__negative_zero_hash__returns_32)
{
    static const uint256_t value{ negative_zero_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 32u);
}

BOOST_AUTO_TEST_CASE(uint256__byte_length__max_hash__returns_32)
{
    static const uint256_t value{ max_hash };
    BOOST_REQUIRE_EQUAL(value.byte_length(), 32u);
}

// hash
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__hash__default__returns_null_hash)
{
    static const uint256_t value;
    BOOST_REQUIRE(value.hash() == null_hash);
}

BOOST_AUTO_TEST_CASE(uint256__hash__1__returns_unit_hash)
{
    static const uint256_t value(1);
    BOOST_REQUIRE(value.hash() == unit_hash);
}

BOOST_AUTO_TEST_CASE(uint256__hash__negative_1__returns_negative_zero_hash)
{
    static const uint256_t value(1);
    BOOST_REQUIRE(value.hash() == unit_hash);
}

// array operator
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__array__default__expected)
{
    static const uint256_t value;
    BOOST_REQUIRE_EQUAL(value[0], 0u);
    BOOST_REQUIRE_EQUAL(value[1], 0u);
    BOOST_REQUIRE_EQUAL(value[2], 0u);
    BOOST_REQUIRE_EQUAL(value[3], 0u);
}

BOOST_AUTO_TEST_CASE(uint256__array__42__expected)
{
    static const uint256_t value(42);
    BOOST_REQUIRE_EQUAL(value[0], 42u);
    BOOST_REQUIRE_EQUAL(value[1], 0u);
    BOOST_REQUIRE_EQUAL(value[2], 0u);
    BOOST_REQUIRE_EQUAL(value[3], 0u);
}

BOOST_AUTO_TEST_CASE(uint256__array__0x87654321__expected)
{
    static const uint256_t value(0x87654321);
    BOOST_REQUIRE_EQUAL(value[0], 0x0000000087654321);
    BOOST_REQUIRE_EQUAL(value[1], 0u);
    BOOST_REQUIRE_EQUAL(value[2], 0u);
    BOOST_REQUIRE_EQUAL(value[3], 0u);
}

BOOST_AUTO_TEST_CASE(uint256__array__negative_1__expected)
{
    static const uint256_t value(negative_zero_hash);
    BOOST_REQUIRE_EQUAL(value[0], 0u);
    BOOST_REQUIRE_EQUAL(value[1], 0u);
    BOOST_REQUIRE_EQUAL(value[2], 0u);
    BOOST_REQUIRE_EQUAL(value[3], 0x8000000000000000);
}

BOOST_AUTO_TEST_CASE(uint256__array__odd_hash__expected)
{
    static const uint256_t value(odd_hash);
    BOOST_REQUIRE_EQUAL(value[0], 0xbcbcc4874353fff4);
    BOOST_REQUIRE_EQUAL(value[1], 0x3485898923aaaaab);
    BOOST_REQUIRE_EQUAL(value[2], 0x34bf128e8cd09234);
    BOOST_REQUIRE_EQUAL(value[3], 0x8437390223499ab2);
}

// comparison operators
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__comparison_operators__null_hash__expected)
{
    static const uint256_t value(null_hash);

    BOOST_REQUIRE_EQUAL(value > 0, false);
    BOOST_REQUIRE_EQUAL(value < 0, false);
    BOOST_REQUIRE_EQUAL(value >= 0, true);
    BOOST_REQUIRE_EQUAL(value <= 0, true);
    BOOST_REQUIRE_EQUAL(value == 0, true);
    BOOST_REQUIRE_EQUAL(value != 0, false);

    BOOST_REQUIRE_EQUAL(value > 1, false);
    BOOST_REQUIRE_EQUAL(value < 1, true);
    BOOST_REQUIRE_EQUAL(value >= 1, false);
    BOOST_REQUIRE_EQUAL(value <= 1, true);
    BOOST_REQUIRE_EQUAL(value == 1, false);
    BOOST_REQUIRE_EQUAL(value != 1, true);
}

BOOST_AUTO_TEST_CASE(uint256__comparison_operators__unit_hash__expected)
{
    static const uint256_t value(unit_hash);

    BOOST_REQUIRE_EQUAL(value > 1, false);
    BOOST_REQUIRE_EQUAL(value < 1, false);
    BOOST_REQUIRE_EQUAL(value >= 1, true);
    BOOST_REQUIRE_EQUAL(value <= 1, true);
    BOOST_REQUIRE_EQUAL(value == 1, true);
    BOOST_REQUIRE_EQUAL(value != 1, false);

    BOOST_REQUIRE_EQUAL(value > 0, true);
    BOOST_REQUIRE_EQUAL(value < 0, false);
    BOOST_REQUIRE_EQUAL(value >= 0, true);
    BOOST_REQUIRE_EQUAL(value <= 0, false);
    BOOST_REQUIRE_EQUAL(value == 0, false);
    BOOST_REQUIRE_EQUAL(value != 0, true);
}

BOOST_AUTO_TEST_CASE(uint256__comparison_operators__negative_zero_hash__expected)
{
    static const uint256_t value(negative_zero_hash);
    static const uint256_t most(most_hash);
    static const uint256_t maximum(max_hash);

    BOOST_REQUIRE_EQUAL(value > 1, true);
    BOOST_REQUIRE_EQUAL(value < 1, false);
    BOOST_REQUIRE_EQUAL(value >= 1, true);
    BOOST_REQUIRE_EQUAL(value <= 1, false);
    BOOST_REQUIRE_EQUAL(value == 1, false);
    BOOST_REQUIRE_EQUAL(value != 1, true);

    BOOST_REQUIRE_GT(value, most);
    BOOST_REQUIRE_LT(value, maximum);

    BOOST_REQUIRE_GE(value, most);
    BOOST_REQUIRE_LE(value, maximum);

    BOOST_REQUIRE_EQUAL(value, value);
    BOOST_REQUIRE_NE(value, most);
    BOOST_REQUIRE_NE(value, maximum);
}

// not
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__not__minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(~uint256_t(), uint256_t(max_hash));
}

BOOST_AUTO_TEST_CASE(uint256__not__maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(~uint256_t(max_hash), uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__not__most_hash__negative_zero_hash)
{
    BOOST_REQUIRE_EQUAL(~uint256_t(most_hash), uint256_t(negative_zero_hash));
}

BOOST_AUTO_TEST_CASE(uint256__not__not_odd_hash__odd_hash)
{
    BOOST_REQUIRE_EQUAL(~~uint256_t(odd_hash), uint256_t(odd_hash));
}

BOOST_AUTO_TEST_CASE(uint256__not__odd_hash__expected)
{
    static const uint256_t value(odd_hash);
    static const auto not_value = ~value;
    BOOST_REQUIRE_EQUAL(not_value[0], ~0xbcbcc4874353fff4);
    BOOST_REQUIRE_EQUAL(not_value[1], ~0x3485898923aaaaab);
    BOOST_REQUIRE_EQUAL(not_value[2], ~0x34bf128e8cd09234);
    BOOST_REQUIRE_EQUAL(not_value[3], ~0x8437390223499ab2);
}

// two's compliment (negate)
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__twos_compliment__null_hash__null_hash)
{
    BOOST_REQUIRE_EQUAL(-uint256_t(), uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__twos_compliment__unit_hash__max_hash)
{
    BOOST_REQUIRE_EQUAL(-uint256_t(unit_hash), uint256_t(max_hash));
}

BOOST_AUTO_TEST_CASE(uint256__twos_compliment__odd_hash__expected)
{
    static const uint256_t value(odd_hash);
    static const auto compliment = -value;
    BOOST_REQUIRE_EQUAL(compliment[0], ~0xbcbcc4874353fff4 + 1);
    BOOST_REQUIRE_EQUAL(compliment[1], ~0x3485898923aaaaab);
    BOOST_REQUIRE_EQUAL(compliment[2], ~0x34bf128e8cd09234);
    BOOST_REQUIRE_EQUAL(compliment[3], ~0x8437390223499ab2);
}

// shift right
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__shift_right__null_hash__null_hash)
{
    BOOST_REQUIRE_EQUAL(uint256_t() >> 0, uint256_t());
    BOOST_REQUIRE_EQUAL(uint256_t() >> 1, uint256_t());
    BOOST_REQUIRE_EQUAL(uint256_t() >> max_uint32, uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__shift_right__unit_hash_0__unit_hash)
{
    static const uint256_t value(unit_hash);
    BOOST_REQUIRE_EQUAL(value >> 0, value);
}

BOOST_AUTO_TEST_CASE(uint256__shift_right__unit_hash_positive__null_hash)
{
    static const uint256_t value(unit_hash);
    BOOST_REQUIRE_EQUAL(value >> 1, uint256_t());
    BOOST_REQUIRE_EQUAL(value >> max_uint32, uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__shift_right__max_hash_1__most_hash)
{
    static const uint256_t value(max_hash);
    BOOST_REQUIRE_EQUAL(value >> 1, uint256_t(most_hash));
}

BOOST_AUTO_TEST_CASE(uint256__shift_right__odd_hash_32__expected)
{
    static const uint256_t value(odd_hash);
    static const auto shifted = value >> 32;
    BOOST_REQUIRE_EQUAL(shifted[0], 0x23aaaaabbcbcc487);
    BOOST_REQUIRE_EQUAL(shifted[1], 0x8cd0923434858989);
    BOOST_REQUIRE_EQUAL(shifted[2], 0x23499ab234bf128e);
    BOOST_REQUIRE_EQUAL(shifted[3], 0x0000000084373902);
}

// add
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(uint256__add__0_to_null_hash__null_hash)
{
    BOOST_REQUIRE_EQUAL(uint256_t() + 0, uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__add__null_hash_to_null_hash__null_hash)
{
    BOOST_REQUIRE_EQUAL(uint256_t() + uint256_t(), uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__add__ones_hash_to_odd_hash__expected)
{
    static const uint256_t value(odd_hash);
    static const auto sum = value + uint256_t(ones_hash);
    BOOST_REQUIRE_EQUAL(sum[0], 0xbcbcc4884353fff5);
    BOOST_REQUIRE_EQUAL(sum[1], 0x3485898a23aaaaac);
    BOOST_REQUIRE_EQUAL(sum[2], 0x34bf128f8cd09235);
    BOOST_REQUIRE_EQUAL(sum[3], 0x8437390323499ab3);
}

BOOST_AUTO_TEST_CASE(uint256__add__1_to_0xffffffff__0x0100000000)
{
    static const uint256_t value(0xffffffff);
    static const auto sum = value + 1;
    BOOST_REQUIRE_EQUAL(sum[0], 0x0100000000);
    BOOST_REQUIRE_EQUAL(sum[1], 0u);
    BOOST_REQUIRE_EQUAL(sum[2], 0u);
    BOOST_REQUIRE_EQUAL(sum[3], 0u);
}

BOOST_AUTO_TEST_CASE(uint256__add__1_to_max_hash__null_hash)
{
    static const uint256_t value(max_hash);
    static const auto sum = value + 1;
    BOOST_REQUIRE_EQUAL(sum, uint256_t());
}

BOOST_AUTO_TEST_CASE(uint256__add__1_to_negative_zero_hash__expected)
{
    static const uint256_t value(negative_zero_hash);
    static const auto sum = value + 1;
    BOOST_REQUIRE_EQUAL(sum[0], 0x0000000000000001);
    BOOST_REQUIRE_EQUAL(sum[1], 0x0000000000000000);
    BOOST_REQUIRE_EQUAL(sum[2], 0x0000000000000000);
    BOOST_REQUIRE_EQUAL(sum[3], 0x8000000000000000);
}

BOOST_AUTO_TEST_SUITE_END()
