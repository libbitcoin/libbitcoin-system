/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

 // creates addition_tests namespace
BOOST_AUTO_TEST_SUITE(addition_tests)

constexpr uint32_t pos_uint32 = 42;
constexpr  int32_t pos_int32 = 42;
constexpr  int32_t zer_int32 = 0;
constexpr  int32_t neg_int32 = -42;

// add
// ----------------------------------------------------------------------------

BC_PUSH_WARNING(TRUNCATED_CONSTANT)
static_assert(add(min_uint32, min_uint32) == (int64_t)min_uint32 + (int64_t)min_uint32);
////static_assert(add(min_uint32, max_uint32) == (int32_t)min_uint32 + (int32_t)max_uint32);
static_assert(add(pos_uint32, pos_uint32) == (int64_t)pos_uint32 + (int64_t)pos_uint32);
static_assert(add(pos_uint32, max_uint32) == (int32_t)pos_uint32 + (int32_t)max_uint32);
static_assert(add(pos_uint32, min_uint32) == (int64_t)pos_uint32 + (int64_t)min_uint32);
static_assert(add(min_uint32, pos_uint32) == (int32_t)min_uint32 + (int32_t)pos_uint32);
static_assert(add(zer_int32, zer_int32) == (int64_t)zer_int32 + (int64_t)zer_int32);
static_assert(add(zer_int32, max_int32) == (int32_t)zer_int32 + (int32_t)max_int32);
static_assert(add(neg_int32, neg_int32) == (int64_t)neg_int32 + (int64_t)neg_int32);
static_assert(add(neg_int32, max_int32) == (int32_t)neg_int32 + (int32_t)max_int32);
static_assert(add<int64_t>(neg_int32, min_int32) == (int64_t)neg_int32 + (int64_t)min_int32);
static_assert(add(pos_int32, neg_int32) == (int64_t)pos_int32 + (int64_t)neg_int32);
static_assert(add(neg_int32, pos_int32) == (int32_t)neg_int32 + (int32_t)pos_int32);
static_assert(add(pos_int32, min_int32) == (int64_t)pos_int32 + (int64_t)min_int32);
static_assert(add(min_int32, pos_int32) == (int32_t)min_int32 + (int32_t)pos_int32);
BC_POP_WARNING()

// Unsigned overflow (signed overflow fails constexpr evaluation).
static_assert(add(max_uint32, max_uint32) == sub1(max_uint32));
static_assert(is_add_overflow(max_uint32, max_uint32));

// TODO: value tests for default and explicit typing.

// Single type (signed).
static_assert(is_same_type<decltype(add(0, 0)), int>);
static_assert(is_same_type<decltype(add(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(add(0_i16, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(add(0_i32, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(add(0_i64, 0_i64)), int64_t>);

// Single type (unsigned).
static_assert(is_same_type<decltype(add(0_u08, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(add(0_u16, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(add(0_u32, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(add(0_u64, 0_u64)), uint64_t>);

// Implicit type (signed).
static_assert(is_same_type<decltype(add(0_i08, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(add(0_i16, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(add(0_i32, 0_i64)), int64_t>);
static_assert(is_same_type<decltype(add(0_i16, 0_i8)), int16_t>);
static_assert(is_same_type<decltype(add(0_i32, 0_i16)), int32_t>);
static_assert(is_same_type<decltype(add(0_i64, 0_i32)), int64_t>);

// Implicit type (unsigned).
static_assert(is_same_type<decltype(add(0_u08, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(add(0_u16, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(add(0_u32, 0_u64)), uint64_t>);
static_assert(is_same_type<decltype(add(0_u16, 0_u08)), uint16_t>);
static_assert(is_same_type<decltype(add(0_u32, 0_u16)), uint32_t>);
static_assert(is_same_type<decltype(add(0_u64, 0_u32)), uint64_t>);

// Explicit narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(add<int8_t>(0, 0)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_i16)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_i32)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_i64)), int8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_u16)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_u32)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_u64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(add<int8_t>(0_u08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_u08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_u08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_u08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_i8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_i8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_i8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_i8, 0_i64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, mixed signs.
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(add<int8_t>(0_i08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(add<uint8_t>(0_u8, 0_i64)), uint8_t>);

// subtract
// ----------------------------------------------------------------------------

BC_PUSH_WARNING(TRUNCATED_CONSTANT)
static_assert(subtract(min_uint32, min_uint32) == (int64_t)min_uint32 - (int64_t)min_uint32);
static_assert(subtract(min_uint32, max_uint32) == (int32_t)min_uint32 - (int32_t)max_uint32);
static_assert(subtract(pos_uint32, pos_uint32) == (int64_t)pos_uint32 - (int64_t)pos_uint32);
static_assert(subtract(pos_uint32, max_uint32) == (int32_t)pos_uint32 - (int32_t)max_uint32);
static_assert(subtract(pos_uint32, min_uint32) == (int64_t)pos_uint32 - (int64_t)min_uint32);
////static_assert(subtract(min_uint32, pos_uint32) == (int32_t)min_uint32 - (int32_t)pos_uint32);
static_assert(subtract(zer_int32, zer_int32) == (int64_t)zer_int32 - (int64_t)zer_int32);
static_assert(subtract(zer_int32, max_int32) == (int32_t)zer_int32 - (int32_t)max_int32);
static_assert(subtract(neg_int32, neg_int32) == (int64_t)neg_int32 - (int64_t)neg_int32);
static_assert(subtract(neg_int32, min_int32) == (int64_t)neg_int32 - (int64_t)min_int32);
static_assert(subtract(min_int32, neg_int32) == (int32_t)min_int32 - (int32_t)neg_int32);
static_assert(subtract(pos_int32, neg_int32) == (int64_t)pos_int32 - (int64_t)neg_int32);
static_assert(subtract(neg_int32, pos_int32) == (int32_t)neg_int32 - (int32_t)pos_int32);
static_assert(subtract<int64_t>(pos_int32, min_int32) == (int64_t)pos_int32 - (int64_t)min_int32);
BC_POP_WARNING()

// Unsigned underflow (signed underflow fails constexpr evaluation).
static_assert(subtract(min_uint32, max_uint32) == one);
static_assert(is_subtract_overflow(min_uint32, max_uint32));

// TODO: value tests for default and explicit typing.

// Single type (signed).
static_assert(is_same_type<decltype(subtract(0, 0)), int>);
static_assert(is_same_type<decltype(subtract(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(subtract(0_i16, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(subtract(0_i32, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(subtract(0_i64, 0_i64)), int64_t>);

// Single type (unsigned).
static_assert(is_same_type<decltype(subtract(0_u08, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(subtract(0_u16, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(subtract(0_u32, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(subtract(0_u64, 0_u64)), uint64_t>);

// Implicit type (signed).
static_assert(is_same_type<decltype(subtract(0_i08, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(subtract(0_i16, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(subtract(0_i32, 0_i64)), int64_t>);
static_assert(is_same_type<decltype(subtract(0_i16, 0_i8)), int16_t>);
static_assert(is_same_type<decltype(subtract(0_i32, 0_i16)), int32_t>);
static_assert(is_same_type<decltype(subtract(0_i64, 0_i32)), int64_t>);

// Implicit type (unsigned).
static_assert(is_same_type<decltype(subtract(0_u08, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(subtract(0_u16, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(subtract(0_u32, 0_u64)), uint64_t>);
static_assert(is_same_type<decltype(subtract(0_u16, 0_u08)), uint16_t>);
static_assert(is_same_type<decltype(subtract(0_u32, 0_u16)), uint32_t>);
static_assert(is_same_type<decltype(subtract(0_u64, 0_u32)), uint64_t>);

// Explicit narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(subtract<int8_t>(0, 0)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_i16)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_i32)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_i64)), int8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_u16)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_u32)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_u64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(subtract<int8_t>(0_u08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_u08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_u08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_u08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_i8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_i8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_i8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_i8, 0_i64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, mixed signs.
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(subtract<int8_t>(0_i08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(subtract<uint8_t>(0_u8, 0_i64)), uint8_t>);

// ----------------------------------------------------------------------------

constexpr uint16_t unsigned_min = 0;
constexpr uint16_t unsigned_max = max_uint16;
constexpr uint16_t unsigned_half = to_half(unsigned_max);

constexpr auto signed_min = min_int16;
constexpr auto signed_zero = int16_t{ 0 };
constexpr auto signed_max = max_int16;
constexpr auto signed_half = to_half(signed_max);

// ceilinged_add
// ----------------------------------------------------------------------------

// ceilinged_add (signed)
static_assert(ceilinged_add(signed_zero, signed_zero) == signed_zero);
static_assert(ceilinged_add(signed_min, signed_zero) == signed_min);
static_assert(ceilinged_add(signed_zero, signed_min) == signed_min);
static_assert(ceilinged_add(signed_min, signed_min) == signed_min);
static_assert(ceilinged_add(signed_min, signed_max) == -1);
static_assert(ceilinged_add(signed_max, signed_min) == -1);
static_assert(ceilinged_add(signed_max, signed_max) == signed_max);
static_assert(ceilinged_add(signed_max, signed_half) == signed_max);
static_assert(ceilinged_add(signed_half, signed_max) == signed_max);
static_assert(ceilinged_add(signed_half, signed_half) == sub1(signed_max));
static_assert(is_same_type<decltype(ceilinged_add<uint32_t>(0, 0)), uint32_t>);

// ceilinged_add (unsigned)
static_assert(ceilinged_add(unsigned_min, unsigned_min) == unsigned_min);
static_assert(ceilinged_add(unsigned_min, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_min) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_half) == unsigned_max);
static_assert(ceilinged_add(unsigned_half, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_half, unsigned_half) == sub1(unsigned_max));
static_assert(is_same_type<decltype(ceilinged_add<uint16_t>(0, 0)), uint16_t>);

// TODO: value/types tests for default and explicit typing.

// floor_subtract
// ----------------------------------------------------------------------------

// floor_subtract (signed)
static_assert(floored_subtract(signed_zero, signed_zero) == signed_zero);
static_assert(floored_subtract(signed_min, signed_zero) == signed_min);
static_assert(floored_subtract(signed_zero, signed_min) == signed_max);
static_assert(floored_subtract(signed_min, signed_zero) == signed_min);
static_assert(floored_subtract(signed_min, signed_min) == 0);
static_assert(floored_subtract(signed_max, signed_min) == signed_max);
static_assert(floored_subtract(signed_min, signed_max) == signed_min);
static_assert(floored_subtract(signed_max, signed_max) == 0);
static_assert(floored_subtract(signed_max, signed_half) == add1(+signed_half));
static_assert(floored_subtract(signed_half, signed_max) == sub1(-signed_half));
static_assert(floored_subtract(signed_half, signed_half) == 0);
static_assert(is_same_type<decltype(floored_subtract<int16_t>(0, 0)), int16_t>);

// floor_subtract (unsigned)
static_assert(floored_subtract(unsigned_min, unsigned_min) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_min) == unsigned_max);
static_assert(floored_subtract(unsigned_min, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_half) == add1(unsigned_half));
static_assert(floored_subtract(unsigned_half, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_half, unsigned_half) == unsigned_min);
static_assert(is_same_type<decltype(floored_subtract<uint16_t>(0, 0)), uint16_t>);

BOOST_AUTO_TEST_SUITE_END()
