/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BC_AUTO_TEST_SUITE(multiplication_tests)

// multiply
// ----------------------------------------------------------------------------

constexpr uint32_t pos_uint32 = 42;
constexpr  int32_t pos_int32 = 42;
constexpr  int32_t zer_int32 = 0;
constexpr  int32_t neg_int32 = -42;

static_assert(is_zero(multiply(min_uint32, max_uint32)));
static_assert(multiply(pos_uint32, pos_uint32) == 1'764);
static_assert(is_zero(multiply(zer_int32, max_int32)));
static_assert(multiply(neg_int32, neg_int32) == 1'764);
static_assert(multiply(pos_int32, neg_int32) == -1'764);
static_assert(multiply(pos_uint32, 2_u64) == 84);
static_assert(multiply<int64_t>(max_int32, max_int32) == 4'611'686'014'132'420'609);
static_assert(multiply<uint64_t>(max_uint32, max_uint32) == 18'446'744'065'119'617'025_u64);
static_assert(is_zero(multiply<uint8_t>(16_u16, 16_u32)));

// Unsigned overflow (signed overflow fails constexpr evaluation).
static_assert(is_one(multiply(max_uint32, max_uint32)));
static_assert(is_multiply_overflow(max_uint32, max_uint32));

// Single type (signed).
static_assert(is_same_type<decltype(multiply(0, 0)), int>);
static_assert(is_same_type<decltype(multiply(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(multiply(0_i16, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(multiply(0_i32, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(multiply(0_i64, 0_i64)), int64_t>);

// Single type (unsigned).
static_assert(is_same_type<decltype(multiply(0_u08, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(multiply(0_u16, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(multiply(0_u32, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(multiply(0_u64, 0_u64)), uint64_t>);

// Implicit type (signed).
static_assert(is_same_type<decltype(multiply(0_i08, 0_i16)), int16_t>);
static_assert(is_same_type<decltype(multiply(0_i16, 0_i32)), int32_t>);
static_assert(is_same_type<decltype(multiply(0_i32, 0_i64)), int64_t>);
static_assert(is_same_type<decltype(multiply(0_i16, 0_i8)), int16_t>);
static_assert(is_same_type<decltype(multiply(0_i32, 0_i16)), int32_t>);
static_assert(is_same_type<decltype(multiply(0_i64, 0_i32)), int64_t>);

// Implicit type (unsigned).
static_assert(is_same_type<decltype(multiply(0_u08, 0_u16)), uint16_t>);
static_assert(is_same_type<decltype(multiply(0_u16, 0_u32)), uint32_t>);
static_assert(is_same_type<decltype(multiply(0_u32, 0_u64)), uint64_t>);
static_assert(is_same_type<decltype(multiply(0_u16, 0_u08)), uint16_t>);
static_assert(is_same_type<decltype(multiply(0_u32, 0_u16)), uint32_t>);
static_assert(is_same_type<decltype(multiply(0_u64, 0_u32)), uint64_t>);

// Explicit narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(multiply<int8_t>(0, 0)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_i08)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_i16)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_i32)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_i64)), int8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_u08)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_u16)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_u32)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_u64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, same signs.
static_assert(is_same_type<decltype(multiply<int8_t>(0_u08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_u08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_u08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_u08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_i8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_i8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_i8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_i8, 0_i64)), uint8_t>);

// Explicit reassigning narrowing/non-narrowing, mixed signs.
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_u08)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_u16)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_u32)), int8_t>);
static_assert(is_same_type<decltype(multiply<int8_t>(0_i08, 0_u64)), int8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_i08)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_i16)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_i32)), uint8_t>);
static_assert(is_same_type<decltype(multiply<uint8_t>(0_u8, 0_i64)), uint8_t>);

// is_multiple
// ----------------------------------------------------------------------------

static_assert(is_multiple(-1, 1));
static_assert(is_multiple(-2, 1));
static_assert(is_multiple(-3, 1));
static_assert(is_multiple(-4, 1));

static_assert(is_multiple(1, -1));
static_assert(is_multiple(2, -1));
static_assert(is_multiple(3, -1));
static_assert(is_multiple(4, -1));

static_assert(is_multiple(-1, -1));
static_assert(is_multiple(-2, -1));
static_assert(is_multiple(-3, -1));
static_assert(is_multiple(-4, -1));

static_assert(!is_multiple(-1, 2));
static_assert( is_multiple(-2, 2)); // <=
static_assert(!is_multiple(-3, 2));
static_assert( is_multiple(-4, 2)); // <=

static_assert(!is_multiple(1, -2));
static_assert( is_multiple(2, -2)); // <=
static_assert(!is_multiple(3, -2));
static_assert( is_multiple(4, -2)); // <=

static_assert(!is_multiple(-1, -2));
static_assert( is_multiple(-2, -2)); // <=
static_assert(!is_multiple(-3, -2));
static_assert( is_multiple(-4, -2)); // <=

static_assert(!is_multiple(-1, 3));
static_assert(!is_multiple(-2, 3));
static_assert( is_multiple(-3, 3)); // <=
static_assert(!is_multiple(-4, 3));

static_assert(!is_multiple(1, -3));
static_assert(!is_multiple(2, -3));
static_assert( is_multiple(3, -3)); // <=
static_assert(!is_multiple(4, -3));

static_assert(!is_multiple(-1, -3));
static_assert(!is_multiple(-2, -3));
static_assert( is_multiple(-3, -3)); // <=
static_assert(!is_multiple(-4, -3));

static_assert(!is_multiple(-1, 4));
static_assert(!is_multiple(-2, 4));
static_assert(!is_multiple(-3, 4));
static_assert( is_multiple(-4, 4)); // <=

static_assert(!is_multiple(1, -4));
static_assert(!is_multiple(2, -4));
static_assert(!is_multiple(3, -4));
static_assert( is_multiple(4, -4)); // <=

static_assert(!is_multiple(-1, -4));
static_assert(!is_multiple(-2, -4));
static_assert(!is_multiple(-3, -4));
static_assert( is_multiple(-4, -4)); // <=

static_assert( is_multiple(0, 0)); // <=
static_assert(!is_multiple(1, 0));
static_assert(!is_multiple(2, 0));
static_assert(!is_multiple(3, 0));
static_assert(!is_multiple(4, 0));

static_assert(is_multiple(0, 1));
static_assert(is_multiple(1, 1));
static_assert(is_multiple(2, 1));
static_assert(is_multiple(3, 1));
static_assert(is_multiple(4, 1));

static_assert( is_multiple(0, 2)); // <=
static_assert(!is_multiple(1, 2));
static_assert( is_multiple(2, 2)); // <=
static_assert(!is_multiple(3, 2));
static_assert( is_multiple(4, 2)); // <=

static_assert( is_multiple(0, 3)); // <=
static_assert(!is_multiple(1, 3));
static_assert(!is_multiple(2, 3));
static_assert( is_multiple(3, 3)); // <=
static_assert(!is_multiple(4, 3));

static_assert( is_multiple(0_u8, max_uint8)); // <=
static_assert(!is_multiple(1_u8, max_uint8));
static_assert(!is_multiple(2_u8, max_uint8));
static_assert(!is_multiple(3_u8, max_uint8));
static_assert(!is_multiple(max_uint8, 8_u8));
static_assert(!is_multiple(max_uint8, min_uint8));
static_assert( is_multiple(max_uint8, max_uint8)); // <=
static_assert( is_multiple(min_uint8, max_uint8)); // <=
static_assert( is_multiple(subtract<uint8_t>(max_uint8, sub1(byte_bits)), 8_u8)); // <=

// is_product
// ----------------------------------------------------------------------------

// 0

static_assert( is_product(0, 0, 0)); // <=
static_assert( is_product(0, 0, 1)); // <=
static_assert( is_product(0, 0, 2)); // <=
static_assert( is_product(0, 0, 3)); // <=
static_assert( is_product(0, 0, 4)); // <=

static_assert( is_product(0, 1, 0)); // <=
static_assert(!is_product(0, 1, 1));
static_assert(!is_product(0, 1, 2));
static_assert(!is_product(0, 1, 3));
static_assert(!is_product(0, 1, 4));

static_assert( is_product(0, 2, 0)); // <=
static_assert(!is_product(0, 2, 1));
static_assert(!is_product(0, 2, 2));
static_assert(!is_product(0, 2, 3));
static_assert(!is_product(0, 2, 4));

static_assert( is_product(0, 3, 0)); // <=
static_assert(!is_product(0, 3, 1));
static_assert(!is_product(0, 3, 2));
static_assert(!is_product(0, 3, 3));
static_assert(!is_product(0, 3, 4));

static_assert( is_product(0, 4, 0)); // <=
static_assert(!is_product(0, 4, 1));
static_assert(!is_product(0, 4, 2));
static_assert(!is_product(0, 4, 3));
static_assert(!is_product(0, 4, 4));

static_assert( is_product(0, -1, 0)); // <=
static_assert(!is_product(0, -1, 1));
static_assert(!is_product(0, -1, 2));
static_assert(!is_product(0, -1, 3));
static_assert(!is_product(0, -1, 4));

static_assert( is_product(0, -2, 0)); // <=
static_assert(!is_product(0, -2, 1));
static_assert(!is_product(0, -2, 2));
static_assert(!is_product(0, -2, 3));
static_assert(!is_product(0, -2, 4));

static_assert( is_product(0, -3, 0)); // <=
static_assert(!is_product(0, -3, 1));
static_assert(!is_product(0, -3, 2));
static_assert(!is_product(0, -3, 3));
static_assert(!is_product(0, -3, 4));

static_assert( is_product(0, -4, 0)); // <=
static_assert(!is_product(0, -4, 1));
static_assert(!is_product(0, -4, 2));
static_assert(!is_product(0, -4, 3));
static_assert(!is_product(0, -4, 4));

static_assert(!is_product(0, 1, -1));
static_assert(!is_product(0, 1, -2));
static_assert(!is_product(0, 1, -3));
static_assert(!is_product(0, 1, -4));

static_assert(!is_product(0, 2, -1));
static_assert(!is_product(0, 2, -2));
static_assert(!is_product(0, 2, -3));
static_assert(!is_product(0, 2, -4));

static_assert(!is_product(0, 3, -1));
static_assert(!is_product(0, 3, -2));
static_assert(!is_product(0, 3, -3));
static_assert(!is_product(0, 3, -4));

static_assert(!is_product(0, 4, -1));
static_assert(!is_product(0, 4, -2));
static_assert(!is_product(0, 4, -3));
static_assert(!is_product(0, 4, -4));

static_assert(!is_product(0, -1, -1));
static_assert(!is_product(0, -1, -2));
static_assert(!is_product(0, -1, -3));
static_assert(!is_product(0, -1, -4));

static_assert(!is_product(0, -2, -1));
static_assert(!is_product(0, -2, -2));
static_assert(!is_product(0, -2, -3));
static_assert(!is_product(0, -2, -4));

static_assert(!is_product(0, -3, -1));
static_assert(!is_product(0, -3, -2));
static_assert(!is_product(0, -3, -3));
static_assert(!is_product(0, -3, -4));

static_assert(!is_product(0, -4, -1));
static_assert(!is_product(0, -4, -2));
static_assert(!is_product(0, -4, -3));
static_assert(!is_product(0, -4, -4));

// 1

static_assert(!is_product(1, 0, 0));
static_assert(!is_product(1, 0, 1));
static_assert(!is_product(1, 0, 2));
static_assert(!is_product(1, 0, 3));
static_assert(!is_product(1, 0, 4));

static_assert(!is_product(1, 1, 0));
static_assert( is_product(1, 1, 1)); // <=
static_assert(!is_product(1, 1, 2));
static_assert(!is_product(1, 1, 3));
static_assert(!is_product(1, 1, 4));

static_assert(!is_product(1, 2, 0));
static_assert(!is_product(1, 2, 1));
static_assert(!is_product(1, 2, 2));
static_assert(!is_product(1, 2, 3));
static_assert(!is_product(1, 2, 4));

static_assert(!is_product(1, 3, 0));
static_assert(!is_product(1, 3, 1));
static_assert(!is_product(1, 3, 2));
static_assert(!is_product(1, 3, 3));
static_assert(!is_product(1, 3, 4));

static_assert(!is_product(1, 4, 0));
static_assert(!is_product(1, 4, 1));
static_assert(!is_product(1, 4, 2));
static_assert(!is_product(1, 4, 3));
static_assert(!is_product(1, 4, 4));

static_assert(!is_product(-1, 1, 0));
static_assert(!is_product(-1, 1, 1));
static_assert(!is_product(-1, 1, 2));
static_assert(!is_product(-1, 1, 3));
static_assert(!is_product(-1, 1, 4));

static_assert(!is_product(-1, 2, 0));
static_assert(!is_product(-1, 2, 1));
static_assert(!is_product(-1, 2, 2));
static_assert(!is_product(-1, 2, 3));
static_assert(!is_product(-1, 2, 4));

static_assert(!is_product(-1, 3, 0));
static_assert(!is_product(-1, 3, 1));
static_assert(!is_product(-1, 3, 2));
static_assert(!is_product(-1, 3, 3));
static_assert(!is_product(-1, 3, 4));

static_assert(!is_product(-1, 4, 0));
static_assert(!is_product(-1, 4, 1));
static_assert(!is_product(-1, 4, 2));
static_assert(!is_product(-1, 4, 3));
static_assert(!is_product(-1, 4, 4));

static_assert(!is_product(-1, -1, 0));
static_assert( is_product(-1, -1, 1)); // <=
static_assert(!is_product(-1, -1, 2));
static_assert(!is_product(-1, -1, 3));
static_assert(!is_product(-1, -1, 4));

static_assert(!is_product(-1, -2, 0));
static_assert(!is_product(-1, -2, 1));
static_assert(!is_product(-1, -2, 2));
static_assert(!is_product(-1, -2, 3));
static_assert(!is_product(-1, -2, 4));

static_assert(!is_product(-1, -3, 0));
static_assert(!is_product(-1, -3, 1));
static_assert(!is_product(-1, -3, 2));
static_assert(!is_product(-1, -3, 3));
static_assert(!is_product(-1, -3, 4));

static_assert(!is_product(-1, -4, 0));
static_assert(!is_product(-1, -4, 1));
static_assert(!is_product(-1, -4, 2));
static_assert(!is_product(-1, -4, 3));
static_assert(!is_product(-1, -4, 4));

static_assert(!is_product(-1, -1, -1));
static_assert(!is_product(-1, -1, -2));
static_assert(!is_product(-1, -1, -3));
static_assert(!is_product(-1, -1, -4));

static_assert(!is_product(-1, -2, -1));
static_assert(!is_product(-1, -2, -2));
static_assert(!is_product(-1, -2, -3));
static_assert(!is_product(-1, -2, -4));

static_assert(!is_product(-1, -3, -1));
static_assert(!is_product(-1, -3, -2));
static_assert(!is_product(-1, -3, -3));
static_assert(!is_product(-1, -3, -4));

static_assert(!is_product(-1, -4, -1));
static_assert(!is_product(-1, -4, -2));
static_assert(!is_product(-1, -4, -3));
static_assert(!is_product(-1, -4, -4));

static_assert( is_product(1, -1, -1)); // <=
static_assert(!is_product(1, -1, -2));
static_assert(!is_product(1, -1, -3));
static_assert(!is_product(1, -1, -4));

static_assert(!is_product(1, -2, -1));
static_assert(!is_product(1, -2, -2));
static_assert(!is_product(1, -2, -3));
static_assert(!is_product(1, -2, -4));

static_assert(!is_product(1, -3, -1));
static_assert(!is_product(1, -3, -2));
static_assert(!is_product(1, -3, -3));
static_assert(!is_product(1, -3, -4));

static_assert(!is_product(1, -4, -1));
static_assert(!is_product(1, -4, -2));
static_assert(!is_product(1, -4, -3));
static_assert(!is_product(1, -4, -4));

// 2

static_assert(!is_product(2, 0, 0));
static_assert(!is_product(2, 0, 1));
static_assert(!is_product(2, 0, 2));
static_assert(!is_product(2, 0, 3));
static_assert(!is_product(2, 0, 4));

static_assert(!is_product(2, 1, 0));
static_assert(!is_product(2, 1, 1));
static_assert( is_product(2, 1, 2)); // <=
static_assert(!is_product(2, 1, 3));
static_assert(!is_product(2, 1, 4));

static_assert(!is_product(2, 2, 0));
static_assert( is_product(2, 2, 1)); // <=
static_assert(!is_product(2, 2, 2));
static_assert(!is_product(2, 2, 3));
static_assert(!is_product(2, 2, 4));

static_assert(!is_product(2, 3, 0));
static_assert(!is_product(2, 3, 1));
static_assert(!is_product(2, 3, 2));
static_assert(!is_product(2, 3, 3));
static_assert(!is_product(2, 3, 4));

static_assert(!is_product(2, 4, 0));
static_assert(!is_product(2, 4, 1));
static_assert(!is_product(2, 4, 2));
static_assert(!is_product(2, 4, 3));
static_assert(!is_product(2, 4, 4));

static_assert(!is_product(-2, 0, 1));
static_assert(!is_product(-2, 0, 2));
static_assert(!is_product(-2, 0, 3));
static_assert(!is_product(-2, 0, 4));

static_assert(!is_product(-2, -1, 0));
static_assert(!is_product(-2, -1, 1));
static_assert( is_product(-2, -1, 2)); // <=
static_assert(!is_product(-2, -1, 3));
static_assert(!is_product(-2, -1, 4));

static_assert(!is_product(-2, -2, 0));
static_assert( is_product(-2, -2, 1)); // <=
static_assert(!is_product(-2, -2, 2));
static_assert(!is_product(-2, -2, 3));
static_assert(!is_product(-2, -2, 4));

static_assert(!is_product(-2, -3, 0));
static_assert(!is_product(-2, -3, 1));
static_assert(!is_product(-2, -3, 2));
static_assert(!is_product(-2, -3, 3));
static_assert(!is_product(-2, -3, 4));

static_assert(!is_product(-2, -4, 0));
static_assert(!is_product(-2, -4, 1));
static_assert(!is_product(-2, -4, 2));
static_assert(!is_product(-2, -4, 3));
static_assert(!is_product(-2, -4, 4));

static_assert(!is_product(-2, 0, -1));
static_assert(!is_product(-2, 0, -2));
static_assert(!is_product(-2, 0, -3));
static_assert(!is_product(-2, 0, -4));

static_assert(!is_product(-2, -1, -1));
static_assert(!is_product(-2, -1, -2));
static_assert(!is_product(-2, -1, -3));
static_assert(!is_product(-2, -1, -4));

static_assert(!is_product(-2, -2, -1));
static_assert(!is_product(-2, -2, -2));
static_assert(!is_product(-2, -2, -3));
static_assert(!is_product(-2, -2, -4));

static_assert(!is_product(-2, -3, -1));
static_assert(!is_product(-2, -3, -2));
static_assert(!is_product(-2, -3, -3));
static_assert(!is_product(-2, -3, -4));

static_assert(!is_product(-2, -4, -1));
static_assert(!is_product(-2, -4, -2));
static_assert(!is_product(-2, -4, -3));
static_assert(!is_product(-2, -4, -4));

static_assert(!is_product(-2, 1, -1));
static_assert( is_product(-2, 1, -2)); // <=
static_assert(!is_product(-2, 1, -3));
static_assert(!is_product(-2, 1, -4));

static_assert( is_product(-2, 2, -1)); // <=
static_assert(!is_product(-2, 2, -2));
static_assert(!is_product(-2, 2, -3));
static_assert(!is_product(-2, 2, -4));

static_assert(!is_product(-2, 3, -1));
static_assert(!is_product(-2, 3, -2));
static_assert(!is_product(-2, 3, -3));
static_assert(!is_product(-2, 3, -4));

static_assert(!is_product(-2, 4, -1));
static_assert(!is_product(-2, 4, -2));
static_assert(!is_product(-2, 4, -3));
static_assert(!is_product(-2, 4, -4));

// 3

static_assert(!is_product(3, 0, 0));
static_assert(!is_product(3, 0, 1));
static_assert(!is_product(3, 0, 2));
static_assert(!is_product(3, 0, 3));
static_assert(!is_product(3, 0, 4));

static_assert(!is_product(3, 1, 0));
static_assert(!is_product(3, 1, 1));
static_assert(!is_product(3, 1, 2));
static_assert( is_product(3, 1, 3)); // <=
static_assert(!is_product(3, 1, 4));

static_assert(!is_product(3, 2, 0));
static_assert(!is_product(3, 2, 1));
static_assert(!is_product(3, 2, 2));
static_assert(!is_product(3, 2, 3));
static_assert(!is_product(3, 2, 4));

static_assert(!is_product(3, 3, 0));
static_assert( is_product(3, 3, 1)); // <=
static_assert(!is_product(3, 3, 2));
static_assert(!is_product(3, 3, 3));
static_assert(!is_product(3, 3, 4));

static_assert(!is_product(3, 4, 0));
static_assert(!is_product(3, 4, 1));
static_assert(!is_product(3, 4, 2));
static_assert(!is_product(3, 4, 3));
static_assert(!is_product(3, 4, 4));

// 4

static_assert(!is_product(4, 0, 0));
static_assert(!is_product(4, 0, 1));
static_assert(!is_product(4, 0, 2));
static_assert(!is_product(4, 0, 3));
static_assert(!is_product(4, 0, 4));

static_assert(!is_product(4, 1, 0));
static_assert(!is_product(4, 1, 1));
static_assert(!is_product(4, 1, 2));
static_assert(!is_product(4, 1, 3));
static_assert( is_product(4, 1, 4)); // <=

static_assert(!is_product(4, 2, 0));
static_assert(!is_product(4, 2, 1));
static_assert( is_product(4, 2, 2)); // <=
static_assert(!is_product(4, 2, 3));
static_assert(!is_product(4, 2, 4));

static_assert(!is_product(4, 3, 0));
static_assert(!is_product(4, 3, 1));
static_assert(!is_product(4, 3, 2));
static_assert(!is_product(4, 3, 3));
static_assert(!is_product(4, 3, 4));

static_assert(!is_product(4, 4, 0));
static_assert( is_product(4, 4, 1)); // <=
static_assert(!is_product(4, 4, 2));
static_assert(!is_product(4, 4, 3));
static_assert(!is_product(4, 4, 4));

static_assert( is_product(0_u8, 0_u8, max_uint8)); // <=
static_assert(!is_product(1_u8, 1_u8, max_uint8));
static_assert(!is_product(2_u8, 2_u8, max_uint8));
static_assert(!is_product(3_u8, 2_u8, max_uint8));
static_assert(!is_product(to_half(max_uint8), max_uint8, 2_u8));
static_assert(!is_product(to_half(max_uint8), to_half(subtract<uint8_t>(max_uint8, sub1(byte_bits))), 2_u8));

// ceilinged_multiply
// ----------------------------------------------------------------------------

static_assert(ceilinged_multiply(0_u8, 0_u8) == 0_u8);
static_assert(ceilinged_multiply(1_u8, 1_u8) == 1_u8);
static_assert(ceilinged_multiply(2_u8, max_uint8) == max_uint8);
static_assert(ceilinged_multiply(max_uint8, 2_u8) == max_uint8);
static_assert(ceilinged_multiply(max_uint8, max_uint8) == max_uint8);

static_assert(ceilinged_multiply(0_u16, 0_u16) == 0_u16);
static_assert(ceilinged_multiply(1_u16, 1_u16) == 1_u16);
static_assert(ceilinged_multiply(2_u16, max_uint16) == max_uint16);
static_assert(ceilinged_multiply(max_uint16, 2_u16) == max_uint16);
static_assert(ceilinged_multiply(max_uint16, max_uint16) == max_uint16);

static_assert(ceilinged_multiply(0_u32, 0_u32) == 0_u32);
static_assert(ceilinged_multiply(1_u32, 1_u32) == 1_u32);
static_assert(ceilinged_multiply(2_u32, max_uint32) == max_uint32);
static_assert(ceilinged_multiply(max_uint32, 2_u32) == max_uint32);
static_assert(ceilinged_multiply(max_uint32, max_uint32) == max_uint32);

static_assert(ceilinged_multiply(0_u64, 0_u64) == 0_u64);
static_assert(ceilinged_multiply(1_u64, 1_u64) == 1_u64);
static_assert(ceilinged_multiply(2_u64, max_uint64) == max_uint64);
static_assert(ceilinged_multiply(max_uint64, 2_u64) == max_uint64);
static_assert(ceilinged_multiply(max_uint64, max_uint64) == max_uint64);

BC_AUTO_TEST_SUITE_END()
