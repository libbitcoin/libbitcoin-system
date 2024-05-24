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

BOOST_AUTO_TEST_SUITE(multiplication_tests)

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

BOOST_AUTO_TEST_SUITE_END()
