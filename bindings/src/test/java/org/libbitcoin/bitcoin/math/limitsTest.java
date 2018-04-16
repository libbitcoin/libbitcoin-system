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

class limitsTest {
	
//	// cast_add
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__cast_add__uint32_to_int64_0__returns_0)
//	{
//		static const int64_t expected = 0;
//		assertEquals(cast_add<int64_t>(0u, 0u), expected);
//	}
//	
//	public static void limits__cast_add__uint32_to_int64_maximum_plus_0__returns_maximum)
//	{
//		static const int64_t expected = max_uint32;
//		assertEquals(cast_add<int64_t>(max_uint32, uint32_t{ 0 }), expected);
//	}
//	
//	public static void limits__cast_add__uint32_to_int64_maximum_plus_maximum__returns_twice_maximum)
//	{
//		static const int64_t expected = 2 * static_cast<int64_t>(max_uint32);
//		assertEquals(cast_add<int64_t>(max_uint32, max_uint32), expected);
//	}
//	
//	// cast_subtract
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__cast_subtract__uint32_to_int64_0__returns_0)
//	{
//		static const int64_t expected = 0;
//		assertEquals(cast_subtract<int64_t>(0u, 0u), expected);
//	}
//	
//	public static void limits__cast_subtract__uint32_to_int64_0_minus_maximum__returns_negtive_maximum)
//	{
//		static const int64_t expected = -1 * static_cast<int64_t>(max_uint32);
//		assertEquals(cast_subtract<int64_t>(uint32_t{ 0 }, max_uint32), expected);
//	}
//	
//	public static void limits__cast_subtract__uint32_to_int64_maximum_minus_maximum__returns_0)
//	{
//		static const int64_t expected = 0;
//		assertEquals(cast_subtract<int64_t>(max_uint32, max_uint32), expected);
//	}
//	
//	static const size_t minimum = 0;
//	static const size_t maximum = max_size_t;
//	static const size_t half = maximum / 2;
//	
//	// ceiling_add
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__ceiling_add__size_t_minimum_plus_minimum__minimum)
//	{
//		assertEquals(ceiling_add(minimum, minimum), minimum);
//	}
//	
//	public static void limits__ceiling_add__size_t_maximum_plus_maximum__maximum)
//	{
//		assertEquals(ceiling_add(maximum, maximum), maximum);
//	}
//	
//	public static void limits__ceiling_add__size_t_minimum_plus_maximum__maximum)
//	{
//		assertEquals(ceiling_add(minimum, maximum), maximum);
//	}
//	
//	public static void limits__ceiling_add__size_t_maximum_plus_minimum__maximum)
//	{
//		assertEquals(ceiling_add(maximum, minimum), maximum);
//	}
//	
//	public static void limits__ceiling_add__size_t_half_plus_maximum__maximum)
//	{
//		assertEquals(ceiling_add(half, maximum), maximum);
//	}
//	
//	// floor_subtract
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__floor_subtract__size_t_minimum_minus_minimum__minimum)
//	{
//		assertEquals(floor_subtract(minimum, minimum), minimum);
//	}
//	
//	public static void limits__floor_subtract__size_t_maximum_minus_maximum__minimum)
//	{
//		assertEquals(floor_subtract(maximum, maximum), minimum);
//	}
//	
//	public static void limits__floor_subtract__size_t_maximum_minus_minimum__maximum)
//	{
//		assertEquals(floor_subtract(maximum, minimum), maximum);
//	}
//	
//	public static void limits__floor_subtract__size_t_minimum_minus_maximum__minimum)
//	{
//		assertEquals(floor_subtract(minimum, maximum), minimum);
//	}
//	
//	public static void limits__floor_subtract__size_t_half_minus_maximum__minimum)
//	{
//		assertEquals(floor_subtract(half, maximum), minimum);
//	}
//	
//	static const uint32_t min_uint32 = 0;
//	static const uint32_t half_uint32 = max_uint32 / 2;
//	
//	// ceiling_add32
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__ceiling_add__min_uint32_plus_minimum__min_uint32)
//	{
//		assertEquals(ceiling_add(min_uint32, min_uint32), min_uint32);
//	}
//	
//	public static void limits__ceiling_add__max_uint32_plus_max_uint32__max_uint32)
//	{
//		assertEquals(ceiling_add(max_uint32, max_uint32), max_uint32);
//	}
//	
//	public static void limits__ceiling_add__min_uint32_plus_max_uint32__max_uint32)
//	{
//		assertEquals(ceiling_add(min_uint32, max_uint32), max_uint32);
//	}
//	
//	public static void limits__ceiling_add__max_uint32_plus_min_uint32__max_uint32)
//	{
//		assertEquals(ceiling_add(max_uint32, min_uint32), max_uint32);
//	}
//	
//	public static void limits__ceiling_add__half_uint32_plus_max_uint32__max_uint32)
//	{
//		assertEquals(ceiling_add(half_uint32, max_uint32), max_uint32);
//	}
//	
//	// floor_subtract32
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__floor_subtract__min_uint32_minus_min_uint32__min_uint32)
//	{
//		assertEquals(floor_subtract(min_uint32, min_uint32), min_uint32);
//	}
//	
//	public static void limits__floor_subtract__max_uint32_minus_max_uint32__min_uint32)
//	{
//		assertEquals(floor_subtract(max_uint32, max_uint32), min_uint32);
//	}
//	
//	public static void limits__floor_subtract__max_uint32_minus_min_uint32__max_uint32)
//	{
//		assertEquals(floor_subtract(max_uint32, min_uint32), max_uint32);
//	}
//	
//	public static void limits__floor_subtract__min_uint32_minus_max_uint32__min_uint32)
//	{
//		assertEquals(floor_subtract(min_uint32, max_uint32), min_uint32);
//	}
//	
//	public static void limits__floor_subtract__half_uint32_minus_max_uint32__min_uint32)
//	{
//		assertEquals(floor_subtract(half_uint32, max_uint32), min_uint32);
//	}
//	
//	static const uint64_t min_uint64 = 0;
//	static const uint64_t half_uint64 = max_uint64 / 2;
//	
//	// ceiling_add64
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__ceiling_add__min_uint64_plus_min_uint64__min_uint64)
//	{
//		assertEquals(ceiling_add(min_uint64, min_uint64), min_uint64);
//	}
//	
//	public static void limits__ceiling_add__max_uint64_plus_max_uint64__max_uint64)
//	{
//		assertEquals(ceiling_add(max_uint64, max_uint64), max_uint64);
//	}
//	
//	public static void limits__ceiling_add__min_uint64_plus_max_uint64__max_uint64)
//	{
//		assertEquals(ceiling_add(min_uint64, max_uint64), max_uint64);
//	}
//	
//	public static void limits__ceiling_add__max_uint64_plus_min_uint64__max_uint64)
//	{
//		assertEquals(ceiling_add(max_uint64, min_uint64), max_uint64);
//	}
//	
//	public static void limits__ceiling_add__half_uint64_plus_max_uint64__max_uint64)
//	{
//		assertEquals(ceiling_add(half_uint64, max_uint64), max_uint64);
//	}
//	
//	// floor_subtract64
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__floor_subtract__min_uint64_minus_min_uint64__min_uint64)
//	{
//		assertEquals(floor_subtract(min_uint64, min_uint64), min_uint64);
//	}
//	
//	public static void limits__floor_subtract__max_uint64_minus_max_uint64__min_uint64)
//	{
//		assertEquals(floor_subtract(max_uint64, max_uint64), min_uint64);
//	}
//	
//	public static void limits__floor_subtract__max_uint64_minus_min_uint64__max_uint64)
//	{
//		assertEquals(floor_subtract(max_uint64, min_uint64), max_uint64);
//	}
//	
//	public static void limits__floor_subtract__min_uint64_minus_max_uint64__min_uint64)
//	{
//		assertEquals(floor_subtract(min_uint64, max_uint64), min_uint64);
//	}
//	
//	public static void limits__floor_subtract__half_uint64_minus_max_uint64__min_uint64)
//	{
//		assertEquals(floor_subtract(half_uint64, max_uint64), min_uint64);
//	}
//	
//	// safe_add
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_add__size_t_minimum_plus_minimum__minimum)
//	{
//		assertEquals(safe_add(minimum, minimum), minimum);
//	}
//	
//	public static void limits__safe_add__size_t_maximum_plus_maximum__throws_overflow)
//	{
//		BOOST_REQUIRE_THROW(safe_add(maximum, maximum), std::overflow_error);
//	}
//	
//	public static void limits__safe_add__size_t_minimum_plus_maximum__maximum)
//	{
//		assertEquals(safe_add(minimum, maximum), maximum);
//	}
//	
//	public static void limits__safe_add__size_t_maximum_plus_minimum__maximum)
//	{
//		assertEquals(safe_add(maximum, minimum), maximum);
//	}
//	
//	public static void limits__safe_add__size_t_half_plus_maximum__throws_overflow)
//	{
//		BOOST_REQUIRE_THROW(safe_add(half, maximum), std::overflow_error);
//	}
//	
//	// safe_subtract
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_subtract__size_t_minimum_minus_minimum__minimum)
//	{
//		assertEquals(safe_subtract(minimum, minimum), minimum);
//	}
//	
//	public static void limits__safe_subtract__size_t_maximum_minus_maximum__minimum)
//	{
//		assertEquals(safe_subtract(maximum, maximum), minimum);
//	}
//	
//	public static void limits__safe_subtract__size_t_maximum_minus_minimum__maximum)
//	{
//		assertEquals(safe_subtract(maximum, minimum), maximum);
//	}
//	
//	public static void limits__safe_subtract__size_t_minimum_minus_maximum__throws_underflow)
//	{
//		BOOST_REQUIRE_THROW(safe_subtract(minimum, maximum), std::underflow_error);
//	}
//	
//	public static void limits__safe_subtract__size_t_half_minus_maximum__throws_underflow)
//	{
//		BOOST_REQUIRE_THROW(safe_subtract(half, maximum), std::underflow_error);
//	}
//	
//	// safe_increment
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_increment__size_t_minimum__expected)
//	{
//		auto value = minimum;
//		static const auto expected = minimum + 1u;
//		safe_increment(value);
//		assertEquals(value, expected);
//	}
//	
//	public static void limits__safe_increment__size_t_half__expected)
//	{
//		auto value = half;
//		static const auto expected = half + 1u;
//		safe_increment(value);
//		assertEquals(value, expected);
//	}
//	
//	public static void limits__safe_increment__size_t_maximum__throws_overflow)
//	{
//		auto value = maximum;
//		BOOST_REQUIRE_THROW(safe_increment(value), std::overflow_error);
//	}
//	
//	// safe_decrement
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_decrement__size_t_maximum__expected)
//	{
//		auto value = maximum;
//		static const auto expected = maximum - 1u;
//		safe_decrement(value);
//		assertEquals(value, expected);
//	}
//	
//	public static void limits__safe_decrement__size_t_half__expected)
//	{
//		auto value = half;
//		static const auto expected = half - 1u;
//		safe_decrement(value);
//		assertEquals(value, expected);
//	}
//	
//	public static void limits__safe_decrement__size_t_minimum__throws_underflow)
//	{
//		auto value = minimum;
//		BOOST_REQUIRE_THROW(safe_decrement(value), std::underflow_error);
//	}
//	
//	// safe_signed
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_signed__min_int32_to_int32__min_int32)
//	{
//		assertEquals(safe_signed<int32_t>(min_int32), min_int32);
//	}
//	
//	public static void limits__safe_signed__max_int32_to_int32__max_int32)
//	{
//		assertEquals(safe_signed<int32_t>(max_int32), max_int32);
//	}
//	
//	public static void limits__safe_signed__min_int64_to_int32__throws_range)
//	{
//		BOOST_REQUIRE_THROW(safe_signed<int32_t>(min_int64), std::range_error);
//	}
//	
//	// safe_unsigned
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_unsigned__min_uint32_to_uint32__min_uint32)
//	{
//		assertEquals(safe_unsigned<uint32_t>(min_uint32), min_uint32);
//	}
//	
//	public static void limits__safe_unsigned__max_uint32_to_uint32__max_uint32)
//	{
//		assertEquals(safe_unsigned<uint32_t>(max_uint32), max_uint32);
//	}
//	
//	public static void limits__safe_unsigned__max_uint64_to_uint32__throws_range)
//	{
//		BOOST_REQUIRE_THROW(safe_unsigned<uint32_t>(max_uint64), std::range_error);
//	}
//	
//	// safe_to_signed
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_to_signed__min_uint32_to_int32__min_uint32)
//	{
//		assertEquals(safe_to_signed<int32_t>(min_uint32), (int32_t)min_uint32);
//	}
//	
//	public static void limits__safe_to_signed__max_uint32_to_int32__throws_range)
//	{
//		BOOST_REQUIRE_THROW(safe_to_signed<int32_t>(max_uint32), std::range_error);
//	}
//	
//	public static void limits__safe_to_signed__min_uint64_to_int32__min_uint64)
//	{
//		assertEquals(safe_to_signed<int32_t>(min_uint64), (int32_t)min_uint64);
//	}
//	
//	// safe_to_unsigned
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__safe_to_unsigned__min_int32_to_uint32__throws_range)
//	{
//		BOOST_REQUIRE_THROW(safe_to_unsigned<uint32_t>(min_int32), std::range_error);
//	}
//	
//	public static void limits__safe_to_unsigned__max_int32_to_uint32__max_int32)
//	{
//		assertEquals(safe_to_unsigned<uint32_t>(max_int32), (uint32_t)max_int32);
//	}
//	
//	public static void limits__safe_to_unsigned__min_int64_to_uint32__throws_range)
//	{
//		BOOST_REQUIRE_THROW(safe_to_unsigned<uint32_t>(min_int64), std::range_error);
//	}
//	
//	// range_constrain
//	//-----------------------------------------------------------------------------
//	
//	public static void limits__range_constrain__over__max)
//	{
//		const size_t expected = 10;
//		const auto result = range_constrain(size_t(42), size_t(1), expected);
//		assertEquals(result, expected);
//	}
//	
//	public static void limits__range_constrain__under__min)
//	{
//		const size_t expected = 50;
//		const auto result = range_constrain(size_t(42), expected, size_t(100));
//		assertEquals(result, expected);
//	}
//	
//	public static void limits__range_constrain__internal__unchanged)
//	{
//		const size_t expected = 42;
//		const auto result = range_constrain(expected, size_t(10), size_t(100));
//		assertEquals(result, expected);
//	}

	public static void main(String[] args) {
	}
}