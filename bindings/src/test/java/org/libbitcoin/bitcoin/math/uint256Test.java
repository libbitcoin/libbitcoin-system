/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
package org.libbitcoin.bitcoin.math;

class uint256Test {
	private static final String MAX_HASH = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	private static final hash_literal max_hash = new hash_literal(MAX_HASH);

	private static final String NEGATIVE1_HASH = "8000000000000000000000000000000000000000000000000000000000000000";
	private static final hash_literal negative_zero_hash = new hash_literal(NEGATIVE1_HASH);
	
	private static final String MOST_HASH = "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
	private static final hash_literal most_hash = new hash_literal(MOST_HASH);
	
	private static final String ODD_HASH = "8437390223499ab234bf128e8cd092343485898923aaaaabbcbcc4874353fff4";
	private static final hash_literal odd_hash = new hash_literal(ODD_HASH);
	
	private static final String HALF_HASH = "00000000000000000000000000000000ffffffffffffffffffffffffffffffff";
	private static final hash_literal half_hash = new hash_literal(HALF_HASH);
	
	private static final String QUARTER_HASH = "000000000000000000000000000000000000000000000000ffffffffffffffff";
	private static final hash_literal quarter_hash = new hash_literal(QUARTER_HASH);
	
	private static final String UNIT_HASH = "0000000000000000000000000000000000000000000000000000000000000001";
	private static final hash_literal unit_hash = new hash_literal(UNIT_HASH);
	
	private static final String ONES_HASH = "0000000100000001000000010000000100000001000000010000000100000001";
	private static final hash_literal ones_hash = new hash_literal(ONES_HASH);
	
	private static final String FIVES_HASH = "5555555555555555555555555555555555555555555555555555555555555555";
	private static final hash_literal fives_hash = new hash_literal(FIVES_HASH);

	// constructors
	//-----------------------------------------------------------------------------
	
	public static void uint256__constructor_default__always__equates_to_0()
	{
	//    uint256_t minimum;
	//    assertEquals(minimum > 0, false);
	//    assertEquals(minimum < 0, false);
	//    assertEquals(minimum >= 0, true);
	//    assertEquals(minimum <= 0, true);
	//    assertEquals(minimum == 0, true);
	//    assertEquals(minimum != 0, false);
	}
	
	public static void uint256__constructor_move__42__equals_42()
	{
	//    static const auto expected = 42u;
	//    static const uint256_t value(uint256_t{ expected });
	//    assertEquals(value, expected);
	}
	
	//public static void uint256__constructor_copy__odd_hash__equals_odd_hash)
	//{
	//    static const auto expected = to_uint256(odd_hash);
	//    static const uint256_t value(expected);
	//    assertEquals(value, expected);
	//}
	//
	//public static void uint256__constructor_uint32__minimum__equals_0)
	//{
	//    static const auto expected = 0u;
	//    static const uint256_t value(expected);
	//    BOOST_REQUIRE(value == expected);
	//}
	//
	//public static void uint256__constructor_uint32__42__equals_42)
	//{
	//    static const auto expected = 42u;
	//    static const uint256_t value(expected);
	//    BOOST_REQUIRE(value == expected);
	//}
	//
	//public static void uint256__constructor_uint32__maximum__equals_maximum)
	//{
	//    static const auto expected = max_uint32;
	//    static const uint256_t value(expected);
	//    BOOST_REQUIRE(value == expected);
	//}
	//
	//// bit_length
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__bit_length__null_hash__returns_0)
	//{
	//    static const uint256_t value{ null_hash };
	//    assertEquals(value.bit_length(), 0u);
	//}
	//
	//public static void uint256__bit_length__unit_hash__returns_1)
	//{
	//    static const uint256_t value{ unit_hash };
	//    assertEquals(value.bit_length(), 1u);
	//}
	//
	//public static void uint256__bit_length__quarter_hash__returns_64)
	//{
	//    static const uint256_t value{ quarter_hash };
	//    assertEquals(value.bit_length(), 64u);
	//}
	//
	//public static void uint256__bit_length__half_hash__returns_128)
	//{
	//    static const uint256_t value{ half_hash };
	//    assertEquals(value.bit_length(), 128u);
	//}
	//
	//public static void uint256__bit_length__most_hash__returns_255)
	//{
	//    static const uint256_t value{ most_hash };
	//    assertEquals(value.bit_length(), 255u);
	//}
	//
	//public static void uint256__bit_length__negative_zero_hash__returns_256)
	//{
	//    static const uint256_t value{ negative_zero_hash };
	//    assertEquals(value.bit_length(), 256u);
	//}
	//
	//public static void uint256__bit_length__max_hash__returns_256)
	//{
	//    static const uint256_t value{ max_hash };
	//    assertEquals(value.bit_length(), 256u);
	//}
	//
	//// byte_length
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__byte_length__null_hash__returns_0)
	//{
	//    static const uint256_t value{ null_hash };
	//    assertEquals(value.byte_length(), 0u);
	//}
	//
	//public static void uint256__byte_length__unit_hash__returns_1)
	//{
	//    static const uint256_t value{ unit_hash };
	//    assertEquals(value.byte_length(), 1u);
	//}
	//
	//public static void uint256__byte_length__quarter_hash__returns_8)
	//{
	//    static const uint256_t value{ quarter_hash };
	//    assertEquals(value.byte_length(), 8u);
	//}
	//
	//public static void uint256__byte_length__half_hash__returns_16)
	//{
	//    static const uint256_t value{ half_hash };
	//    assertEquals(value.byte_length(), 16u);
	//}
	//
	//public static void uint256__byte_length__most_hash__returns_32)
	//{
	//    static const uint256_t value{ most_hash };
	//    assertEquals(value.byte_length(), 32u);
	//}
	//
	//public static void uint256__byte_length__negative_zero_hash__returns_32)
	//{
	//    static const uint256_t value{ negative_zero_hash };
	//    assertEquals(value.byte_length(), 32u);
	//}
	//
	//public static void uint256__byte_length__max_hash__returns_32)
	//{
	//    static const uint256_t value{ max_hash };
	//    assertEquals(value.byte_length(), 32u);
	//}
	//
	//// hash
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__hash__default__returns_null_hash)
	//{
	//    static const uint256_t value;
	//    BOOST_REQUIRE(value.hash() == null_hash);
	//}
	//
	//public static void uint256__hash__1__returns_unit_hash)
	//{
	//    static const uint256_t value(1);
	//    BOOST_REQUIRE(value.hash() == unit_hash);
	//}
	//
	//public static void uint256__hash__negative_1__returns_negative_zero_hash)
	//{
	//    static const uint256_t value(1);
	//    BOOST_REQUIRE(value.hash() == unit_hash);
	//}
	//
	//// array operator
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__array__default__expected)
	//{
	//    static const uint256_t value;
	//    assertEquals(value[0], 0x0000000000000000);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__array__42__expected)
	//{
	//    static const uint256_t value(42);
	//    assertEquals(value[0], 0x000000000000002a);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__array__0x87654321__expected)
	//{
	//    static const uint256_t value(0x87654321);
	//    assertEquals(value[0], 0x0000000087654321);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__array__negative_1__expected)
	//{
	//    static const uint256_t value(negative_zero_hash);
	//    assertEquals(value[0], 0x0000000000000000);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x8000000000000000);
	//}
	//
	//public static void uint256__array__odd_hash__expected)
	//{
	//    static const uint256_t value(odd_hash);
	//    assertEquals(value[0], 0xbcbcc4874353fff4);
	//    assertEquals(value[1], 0x3485898923aaaaab);
	//    assertEquals(value[2], 0x34bf128e8cd09234);
	//    assertEquals(value[3], 0x8437390223499ab2);
	//}
	//
	//// comparison operators
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__comparison_operators__null_hash__expected)
	//{
	//    static const uint256_t value(null_hash);
	//
	//    assertEquals(value > 0, false);
	//    assertEquals(value < 0, false);
	//    assertEquals(value >= 0, true);
	//    assertEquals(value <= 0, true);
	//    assertEquals(value == 0, true);
	//    assertEquals(value != 0, false);
	//
	//    assertEquals(value > 1, false);
	//    assertEquals(value < 1, true);
	//    assertEquals(value >= 1, false);
	//    assertEquals(value <= 1, true);
	//    assertEquals(value == 1, false);
	//    assertEquals(value != 1, true);
	//}
	//
	//public static void uint256__comparison_operators__unit_hash__expected)
	//{
	//    static const uint256_t value(unit_hash);
	//
	//    assertEquals(value > 1, false);
	//    assertEquals(value < 1, false);
	//    assertEquals(value >= 1, true);
	//    assertEquals(value <= 1, true);
	//    assertEquals(value == 1, true);
	//    assertEquals(value != 1, false);
	//
	//    assertEquals(value > 0, true);
	//    assertEquals(value < 0, false);
	//    assertEquals(value >= 0, true);
	//    assertEquals(value <= 0, false);
	//    assertEquals(value == 0, false);
	//    assertEquals(value != 0, true);
	//}
	//
	//public static void uint256__comparison_operators__negative_zero_hash__expected)
	//{
	//    static const uint256_t value(negative_zero_hash);
	//    static const uint256_t most(most_hash);
	//    static const uint256_t maximum(max_hash);
	//
	//    assertEquals(value > 1, true);
	//    assertEquals(value < 1, false);
	//    assertEquals(value >= 1, true);
	//    assertEquals(value <= 1, false);
	//    assertEquals(value == 1, false);
	//    assertEquals(value != 1, true);
	//
	//    BOOST_REQUIRE_GT(value, most);
	//    BOOST_REQUIRE_LT(value, maximum);
	//
	//    BOOST_REQUIRE_GE(value, most);
	//    BOOST_REQUIRE_LE(value, maximum);
	//
	//    assertEquals(value, value);
	//    BOOST_REQUIRE_NE(value, most);
	//    BOOST_REQUIRE_NE(value, maximum);
	//}
	//
	//// not
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__not__minimum__maximum)
	//{
	//    assertEquals(~uint256_t(), uint256_t(max_hash));
	//}
	//
	//public static void uint256__not__maximum__minimum)
	//{
	//    assertEquals(~uint256_t(max_hash), uint256_t());
	//}
	//
	//public static void uint256__not__most_hash__negative_zero_hash)
	//{
	//    assertEquals(~uint256_t(most_hash), uint256_t(negative_zero_hash));
	//}
	//
	//public static void uint256__not__not_odd_hash__odd_hash)
	//{
	//    assertEquals(~~uint256_t(odd_hash), uint256_t(odd_hash));
	//}
	//
	//public static void uint256__not__odd_hash__expected)
	//{
	//    static const uint256_t value(odd_hash);
	//    static const auto not_value = ~value;
	//    assertEquals(not_value[0], ~0xbcbcc4874353fff4);
	//    assertEquals(not_value[1], ~0x3485898923aaaaab);
	//    assertEquals(not_value[2], ~0x34bf128e8cd09234);
	//    assertEquals(not_value[3], ~0x8437390223499ab2);
	//}
	//
	//// two's compliment (negate)
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__twos_compliment__null_hash__null_hash)
	//{
	//    assertEquals(-uint256_t(), uint256_t());
	//}
	//
	//public static void uint256__twos_compliment__unit_hash__max_hash)
	//{
	//    assertEquals(-uint256_t(unit_hash), uint256_t(max_hash));
	//}
	//
	//public static void uint256__twos_compliment__odd_hash__expected)
	//{
	//    static const uint256_t value(odd_hash);
	//    static const auto compliment = -value;
	//    assertEquals(compliment[0], ~0xbcbcc4874353fff4 + 1);
	//    assertEquals(compliment[1], ~0x3485898923aaaaab);
	//    assertEquals(compliment[2], ~0x34bf128e8cd09234);
	//    assertEquals(compliment[3], ~0x8437390223499ab2);
	//}
	//
	//// shift right
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__shift_right__null_hash__null_hash)
	//{
	//    assertEquals(uint256_t() >> 0, uint256_t());
	//    assertEquals(uint256_t() >> 1, uint256_t());
	//    assertEquals(uint256_t() >> max_uint32, uint256_t());
	//}
	//
	//public static void uint256__shift_right__unit_hash_0__unit_hash)
	//{
	//    static const uint256_t value(unit_hash);
	//    assertEquals(value >> 0, value);
	//}
	//
	//public static void uint256__shift_right__unit_hash_positive__null_hash)
	//{
	//    static const uint256_t value(unit_hash);
	//    assertEquals(value >> 1, uint256_t());
	//    assertEquals(value >> max_uint32, uint256_t());
	//}
	//
	//public static void uint256__shift_right__max_hash_1__most_hash)
	//{
	//    static const uint256_t value(max_hash);
	//    assertEquals(value >> 1, uint256_t(most_hash));
	//}
	//
	//public static void uint256__shift_right__odd_hash_32__expected)
	//{
	//    static const uint256_t value(odd_hash);
	//    static const auto shifted = value >> 32;
	//    assertEquals(shifted[0], 0x23aaaaabbcbcc487);
	//    assertEquals(shifted[1], 0x8cd0923434858989);
	//    assertEquals(shifted[2], 0x23499ab234bf128e);
	//    assertEquals(shifted[3], 0x0000000084373902);
	//}
	//
	//// add256
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__add256__0_to_null_hash__null_hash)
	//{
	//    assertEquals(uint256_t() + 0, uint256_t());
	//}
	//
	//public static void uint256__add256__null_hash_to_null_hash__null_hash)
	//{
	//    assertEquals(uint256_t() + uint256_t(), uint256_t());
	//}
	//
	//public static void uint256__add256__1_to_max_hash__null_hash)
	//{
	//    static const uint256_t value(max_hash);
	//    static const auto sum = value + 1;
	//    assertEquals(sum, uint256_t());
	//}
	//
	//public static void uint256__add256__ones_hash_to_odd_hash__expected)
	//{
	//    static const uint256_t value(odd_hash);
	//    static const auto sum = value + uint256_t(ones_hash);
	//    assertEquals(sum[0], 0xbcbcc4884353fff5);
	//    assertEquals(sum[1], 0x3485898a23aaaaac);
	//    assertEquals(sum[2], 0x34bf128f8cd09235);
	//    assertEquals(sum[3], 0x8437390323499ab3);
	//}
	//
	//public static void uint256__add256__1_to_0xffffffff__0x0100000000)
	//{
	//    static const uint256_t value(0xffffffff);
	//    static const auto sum = value + 1;
	//    assertEquals(sum[0], 0x0000000100000000);
	//    assertEquals(sum[1], 0x0000000000000000);
	//    assertEquals(sum[2], 0x0000000000000000);
	//    assertEquals(sum[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__add256__1_to_negative_zero_hash__expected)
	//{
	//    static const uint256_t value(negative_zero_hash);
	//    static const auto sum = value + 1;
	//    assertEquals(sum[0], 0x0000000000000001);
	//    assertEquals(sum[1], 0x0000000000000000);
	//    assertEquals(sum[2], 0x0000000000000000);
	//    assertEquals(sum[3], 0x8000000000000000);
	//}
	//
	//// divide256
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__divide256__unit_hash_by_null_hash__throws_overflow_error)
	//{
	//    BOOST_REQUIRE_THROW(uint256_t(unit_hash) / uint256_t(0), std::overflow_error);
	//}
	//
	//public static void uint256__divide256__null_hash_by_unit_hash__null_hash)
	//{
	//    assertEquals(uint256_t(null_hash) / uint256_t(unit_hash), uint256_t());
	//}
	//
	//public static void uint256__divide256__max_hash_by_3__fives_hash)
	//{
	//    assertEquals(uint256_t(max_hash) / uint256_t(3), uint256_t(fives_hash));
	//}
	//
	//public static void uint256__divide256__max_hash_by_max_hash__1)
	//{
	//    assertEquals(uint256_t(max_hash) / uint256_t(max_hash), uint256_t(1));
	//}
	//
	//public static void uint256__divide256__max_hash_by_256__shifts_right_8_bits)
	//{
	//    static const uint256_t value(max_hash);
	//    static const auto quotient = value / uint256_t(256);
	//    assertEquals(quotient[0], 0xffffffffffffffff);
	//    assertEquals(quotient[1], 0xffffffffffffffff);
	//    assertEquals(quotient[2], 0xffffffffffffffff);
	//    assertEquals(quotient[3], 0x00ffffffffffffff);
	//}
	//
	//// increment
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__increment__0__1)
	//{
	//    assertEquals(++uint256_t(0), uint256_t(1));
	//}
	//
	//public static void uint256__increment__1__2)
	//{
	//    assertEquals(++uint256_t(1), uint256_t(2));
	//}
	//
	//public static void uint256__increment__max_hash__null_hash)
	//{
	//    assertEquals(++uint256_t(max_hash), uint256_t());
	//}
	//
	//public static void uint256__increment__0xffffffff__0x0100000000)
	//{
	//    static const auto increment = ++uint256_t(0xffffffff);
	//    assertEquals(increment[0], 0x0000000100000000);
	//    assertEquals(increment[1], 0x0000000000000000);
	//    assertEquals(increment[2], 0x0000000000000000);
	//    assertEquals(increment[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__increment__negative_zero_hash__expected)
	//{
	//    static const auto increment = ++uint256_t(negative_zero_hash);
	//    assertEquals(increment[0], 0x0000000000000001);
	//    assertEquals(increment[1], 0x0000000000000000);
	//    assertEquals(increment[2], 0x0000000000000000);
	//    assertEquals(increment[3], 0x8000000000000000);
	//}
	//
	//// assign32
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign__null_hash_0__null_hash)
	//{
	//    uint256_t value(null_hash);
	//    value = 0;
	//    assertEquals(value, uint256_t());
	//}
	//
	//public static void uint256__assign__max_hash_0__null_hash)
	//{
	//    uint256_t value(max_hash);
	//    value = 0;
	//    assertEquals(value, uint256_t());
	//}
	//
	//public static void uint256__assign__odd_hash_to_42__42)
	//{
	//    uint256_t value(odd_hash);
	//    value = 42;
	//    assertEquals(value[0], 0x000000000000002a);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x0000000000000000);
	//}
	//
	//// assign shift right
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_shift_right__null_hash__null_hash)
	//{
	//    uint256_t value1;
	//    uint256_t value2;
	//    uint256_t value3;
	//    value1 >>= 0;
	//    value2 >>= 1;
	//    value3 >>= max_uint32;
	//    assertEquals(value1, uint256_t());
	//    assertEquals(value2, uint256_t());
	//    assertEquals(value3, uint256_t());
	//}
	//
	//public static void uint256__assign_shift_right__unit_hash_0__unit_hash)
	//{
	//    uint256_t value(unit_hash);
	//    value >>= 0;
	//    assertEquals(value, uint256_t(unit_hash));
	//}
	//
	//public static void uint256__assign_shift_right__unit_hash_positive__null_hash)
	//{
	//    uint256_t value1(unit_hash);
	//    uint256_t value2(unit_hash);
	//    value1 >>= 1;
	//    value2 >>= max_uint32;
	//    assertEquals(value1, uint256_t());
	//    assertEquals(value2, uint256_t());
	//}
	//
	//public static void uint256__assign_shift_right__max_hash_1__most_hash)
	//{
	//    uint256_t value(max_hash);
	//    value >>= 1;
	//    assertEquals(value, uint256_t(most_hash));
	//}
	//
	//public static void uint256__assign_shift_right__odd_hash_32__expected)
	//{
	//    uint256_t value(odd_hash);
	//    value >>= 32;
	//    assertEquals(value[0], 0x23aaaaabbcbcc487);
	//    assertEquals(value[1], 0x8cd0923434858989);
	//    assertEquals(value[2], 0x23499ab234bf128e);
	//    assertEquals(value[3], 0x0000000084373902);
	//}
	//
	//// assign shift left
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_shift_left__null_hash__null_hash)
	//{
	//    uint256_t value1;
	//    uint256_t value2;
	//    uint256_t value3;
	//    value1 <<= 0;
	//    value2 <<= 1;
	//    value3 <<= max_uint32;
	//    assertEquals(value1, uint256_t());
	//    assertEquals(value2, uint256_t());
	//    assertEquals(value3, uint256_t());
	//}
	//
	//public static void uint256__assign_shift_left__unit_hash_0__1)
	//{
	//    uint256_t value(unit_hash);
	//    value <<= 0;
	//    assertEquals(value, uint256_t(1));
	//}
	//
	//public static void uint256__assign_shift_left__unit_hash_1__2)
	//{
	//    uint256_t value(unit_hash);
	//    value <<= 1;
	//    assertEquals(value, uint256_t(2));
	//}
	//
	//public static void uint256__assign_shift_left__unit_hash_31__0x80000000)
	//{
	//    uint256_t value(unit_hash);
	//    value <<= 31;
	//    assertEquals(value, uint256_t(0x80000000));
	//}
	//
	//public static void uint256__assign_shift_left__max_hash_1__expected)
	//{
	//    uint256_t value(max_hash);
	//    value <<= 1;
	//    assertEquals(value[0], 0xfffffffffffffffe);
	//    assertEquals(value[1], 0xffffffffffffffff);
	//    assertEquals(value[2], 0xffffffffffffffff);
	//    assertEquals(value[3], 0xffffffffffffffff);
	//}
	//
	//public static void uint256__assign_shift_left__odd_hash_32__expected)
	//{
	//    uint256_t value(odd_hash);
	//    value <<= 32;
	//    assertEquals(value[0], 0x4353fff400000000);
	//    assertEquals(value[1], 0x23aaaaabbcbcc487);
	//    assertEquals(value[2], 0x8cd0923434858989);
	//    assertEquals(value[3], 0x23499ab234bf128e);
	//}
	//
	//// assign multiply32
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_multiply32__0_by_0__0)
	//{
	//    uint256_t value;
	//    value *= 0;
	//    assertEquals(value, uint256_t(0));
	//}
	//
	//public static void uint256__assign_multiply32__0_by_1__0)
	//{
	//    uint256_t value;
	//    value *= 1;
	//    assertEquals(value, uint256_t(0));
	//}
	//
	//public static void uint256__assign_multiply32__1_by_1__1)
	//{
	//    uint256_t value(1);
	//    value *= 1;
	//    assertEquals(value, uint256_t(1));
	//}
	//
	//public static void uint256__assign_multiply32__42_by_1__42)
	//{
	//    uint256_t value(42);
	//    value *= 1;
	//    assertEquals(value, uint256_t(42));
	//}
	//
	//public static void uint256__assign_multiply32__1_by_42__42)
	//{
	//    uint256_t value(1);
	//    value *= 42;
	//    assertEquals(value, uint256_t(42));
	//}
	//
	//public static void uint256__assign_multiply32__fives_hash_by_3__max_hash)
	//{
	//    uint256_t value(fives_hash);
	//    value *= 3;
	//    assertEquals(value, uint256_t(max_hash));
	//}
	//
	//public static void uint256__assign_multiply32__ones_hash_by_max_uint32__max_hash)
	//{
	//    uint256_t value(ones_hash);
	//    value *= max_uint32;
	//    assertEquals(value, uint256_t(max_hash));
	//}
	//
	//public static void uint256__assign_multiply32__max_hash_by_256__shifts_left_8_bits)
	//{
	//    uint256_t value(max_hash);
	//    value *= 256;
	//    assertEquals(value[0], 0xffffffffffffff00);
	//    assertEquals(value[1], 0xffffffffffffffff);
	//    assertEquals(value[2], 0xffffffffffffffff);
	//    assertEquals(value[3], 0xffffffffffffffff);
	//}
	//
	//// assign divide32
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_divide32__unit_hash_by_null_hash__throws_overflow_error)
	//{
	//    uint256_t value(unit_hash);
	//    BOOST_REQUIRE_THROW(value /= 0, std::overflow_error);
	//}
	//
	//public static void uint256__assign_divide32__null_hash_by_unit_hash__null_hash)
	//{
	//    uint256_t value;
	//    value /= 1;
	//    assertEquals(value, uint256_t(null_hash));
	//}
	//
	//public static void uint256__assign_divide32__max_hash_by_3__fives_hash)
	//{
	//    uint256_t value(max_hash);
	//    value /= 3;
	//    assertEquals(value, uint256_t(fives_hash));
	//}
	//
	//public static void uint256__assign_divide32__max_hash_by_max_uint32__ones_hash)
	//{
	//    uint256_t value(max_hash);
	//    value /= max_uint32;
	//    assertEquals(value, uint256_t(ones_hash));
	//}
	//
	//public static void uint256__assign_divide32__max_hash_by_256__shifts_right_8_bits)
	//{
	//    uint256_t value(max_hash);
	//    value /= 256;
	//    assertEquals(value[0], 0xffffffffffffffff);
	//    assertEquals(value[1], 0xffffffffffffffff);
	//    assertEquals(value[2], 0xffffffffffffffff);
	//    assertEquals(value[3], 0x00ffffffffffffff);
	//}
	//
	//// assign add256
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_add256__0_to_null_hash__null_hash)
	//{
	//    uint256_t value;
	//    value += 0;
	//    assertEquals(value, uint256_t(0));
	//}
	//
	//public static void uint256__assign_add256__null_hash_to_null_hash__null_hash)
	//{
	//    uint256_t value;
	//    value += uint256_t();
	//    assertEquals(uint256_t() + uint256_t(), uint256_t());
	//}
	//
	//public static void uint256__assign_add256__1_to_max_hash__null_hash)
	//{
	//    uint256_t value(max_hash);
	//    value += 1;
	//    assertEquals(value, uint256_t());
	//}
	//
	//public static void uint256__assign_add256__ones_hash_to_odd_hash__expected)
	//{
	//    uint256_t value(odd_hash);
	//    value += uint256_t(ones_hash);
	//    assertEquals(value[0], 0xbcbcc4884353fff5);
	//    assertEquals(value[1], 0x3485898a23aaaaac);
	//    assertEquals(value[2], 0x34bf128f8cd09235);
	//    assertEquals(value[3], 0x8437390323499ab3);
	//}
	//
	//public static void uint256__assign_add256__1_to_0xffffffff__0x0100000000)
	//{
	//    uint256_t value(0xffffffff);
	//    value += 1;
	//    assertEquals(value[0], 0x0000000100000000);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x0000000000000000);
	//}
	//
	//public static void uint256__assign_add256__1_to_negative_zero_hash__expected)
	//{
	//    uint256_t value(negative_zero_hash);
	//    value += 1;
	//    assertEquals(value[0], 0x0000000000000001);
	//    assertEquals(value[1], 0x0000000000000000);
	//    assertEquals(value[2], 0x0000000000000000);
	//    assertEquals(value[3], 0x8000000000000000);
	//}
	//
	//// assign subtract256
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_subtract256__0_from_null_hash__null_hash)
	//{
	//    uint256_t value;
	//    value -= 0;
	//    assertEquals(value, uint256_t(0));
	//}
	//
	//public static void uint256__assign_subtract256__null_hash_from_null_hash__null_hash)
	//{
	//    uint256_t value;
	//    value -= uint256_t();
	//    assertEquals(uint256_t() + uint256_t(), uint256_t());
	//}
	//
	//public static void uint256__assign_subtract256__1_from_null_hash__max_hash)
	//{
	//    uint256_t value;
	//    value -= 1;
	//    assertEquals(value, uint256_t(max_hash));
	//}
	//
	//public static void uint256__assign_subtract256__1_from_max_hash__expected)
	//{
	//    uint256_t value(max_hash);
	//    value -= 1;
	//    assertEquals(value[0], 0xfffffffffffffffe);
	//    assertEquals(value[1], 0xffffffffffffffff);
	//    assertEquals(value[2], 0xffffffffffffffff);
	//    assertEquals(value[3], 0xffffffffffffffff);
	//}
	//
	//public static void uint256__assign_subtract256__ones_hash_from_odd_hash__expected)
	//{
	//    uint256_t value(odd_hash);
	//    value -= uint256_t(ones_hash);
	//    assertEquals(value[0], 0xbcbcc4864353fff3);
	//    assertEquals(value[1], 0x3485898823aaaaaa);
	//    assertEquals(value[2], 0x34bf128d8cd09233);
	//    assertEquals(value[3], 0x8437390123499ab1);
	//}
	//
	//public static void uint256__assign_subtract256__1_from_0xffffffff__0x0100000000)
	//{
	//    uint256_t value(0xffffffff);
	//    value -= 1;
	//    assertEquals(value, uint256_t(0xfffffffe));
	//}
	//
	//public static void uint256__assign_subtract256__1_from_negative_zero_hash__most_hash)
	//{
	//    uint256_t value(negative_zero_hash);
	//    value -= 1;
	//    assertEquals(value, uint256_t(most_hash));
	//}
	//
	//// assign divide256
	////-----------------------------------------------------------------------------
	//
	//public static void uint256__assign_divide__unit_hash_by_null_hash__throws_overflow_error)
	//{
	//    uint256_t value(unit_hash);
	//    BOOST_REQUIRE_THROW(value /= uint256_t(0), std::overflow_error);
	//}
	//
	//public static void uint256__assign_divide__null_hash_by_unit_hash__null_hash)
	//{
	//    uint256_t value;
	//    value /= uint256_t(unit_hash);
	//    assertEquals(value, uint256_t(null_hash));
	//}
	//
	//public static void uint256__assign_divide__max_hash_by_3__fives_hash)
	//{
	//    uint256_t value(max_hash);
	//    value /= 3;
	//    assertEquals(value, uint256_t(fives_hash));
	//}
	//
	//public static void uint256__assign_divide__max_hash_by_max_hash__1)
	//{
	//    uint256_t value(max_hash);
	//    value /= uint256_t(max_hash);
	//    assertEquals(value, uint256_t(1));
	//}
	//
	//public static void uint256__assign_divide__max_hash_by_256__shifts_right_8_bits)
	//{
	//    static const uint256_t value(max_hash);
	//    static const auto quotient = value / uint256_t(256);
	//    assertEquals(quotient[0], 0xffffffffffffffff);
	//    assertEquals(quotient[1], 0xffffffffffffffff);
	//    assertEquals(quotient[2], 0xffffffffffffffff);
	//    assertEquals(quotient[3], 0x00ffffffffffffff);
	//}
	//
	//BOOST_AUTO_TEST_SUITE_END()
	public static void main(String[] args) {
	}
}
