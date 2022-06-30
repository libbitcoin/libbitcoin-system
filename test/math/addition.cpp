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
#include <cstddef>

// Suppress C4310: cast truncates constant value (intended behavior).
BC_PUSH_WARNING(TRUNCATED_CONSTANT)

constexpr uint32_t pos_uint32 = 42;
constexpr  int32_t pos_int32 = 42;
constexpr  int32_t zer_int32 = 0;
constexpr  int32_t neg_int32 = -42;

// add
static_assert(add<int64_t>(min_uint32, min_uint32) == (int64_t)min_uint32 + (int64_t)min_uint32);
static_assert(add<int32_t>(min_uint32, max_uint32) == (int32_t)min_uint32 + (int32_t)max_uint32);
static_assert(add<int64_t>(pos_uint32, pos_uint32) == (int64_t)pos_uint32 + (int64_t)pos_uint32);
static_assert(add<int32_t>(pos_uint32, max_uint32) == (int32_t)pos_uint32 + (int32_t)max_uint32);
static_assert(add<int64_t>(pos_uint32, min_uint32) == (int64_t)pos_uint32 + (int64_t)min_uint32);
static_assert(add<int32_t>(min_uint32, pos_uint32) == (int32_t)min_uint32 + (int32_t)pos_uint32);
static_assert(add<int64_t>(zer_int32, zer_int32) == (int64_t)zer_int32 + (int64_t)zer_int32);
static_assert(add<int32_t>(zer_int32, max_int32) == (int32_t)zer_int32 + (int32_t)max_int32);
static_assert(add<int64_t>(neg_int32, neg_int32) == (int64_t)neg_int32 + (int64_t)neg_int32);
static_assert(add<int32_t>(neg_int32, max_int32) == (int32_t)neg_int32 + (int32_t)max_int32);
static_assert(add<int64_t>(neg_int32, min_int32) == (int64_t)neg_int32 + (int64_t)min_int32);
static_assert(add<int64_t>(pos_int32, neg_int32) == (int64_t)pos_int32 + (int64_t)neg_int32);
static_assert(add<int32_t>(neg_int32, pos_int32) == (int32_t)neg_int32 + (int32_t)pos_int32);
static_assert(add<int64_t>(pos_int32, min_int32) == (int64_t)pos_int32 + (int64_t)min_int32);
static_assert(add<int32_t>(min_int32, pos_int32) == (int32_t)min_int32 + (int32_t)pos_int32);
static_assert(std::is_same<decltype(add<uint64_t>(0, 0)), uint64_t>::value);

// subtract
static_assert(subtract<int64_t>(min_uint32, min_uint32) == (int64_t)min_uint32 - (int64_t)min_uint32);
static_assert(subtract<int32_t>(min_uint32, max_uint32) == (int32_t)min_uint32 - (int32_t)max_uint32);
static_assert(subtract<int64_t>(pos_uint32, pos_uint32) == (int64_t)pos_uint32 - (int64_t)pos_uint32);
static_assert(subtract<int32_t>(pos_uint32, max_uint32) == (int32_t)pos_uint32 - (int32_t)max_uint32);
static_assert(subtract<int64_t>(pos_uint32, min_uint32) == (int64_t)pos_uint32 - (int64_t)min_uint32);
static_assert(subtract<int32_t>(min_uint32, pos_uint32) == (int32_t)min_uint32 - (int32_t)pos_uint32);
static_assert(subtract<int64_t>(zer_int32, zer_int32) == (int64_t)zer_int32 - (int64_t)zer_int32);
static_assert(subtract<int32_t>(zer_int32, max_int32) == (int32_t)zer_int32 - (int32_t)max_int32);
static_assert(subtract<int64_t>(neg_int32, neg_int32) == (int64_t)neg_int32 - (int64_t)neg_int32);
static_assert(subtract<int64_t>(neg_int32, min_int32) == (int64_t)neg_int32 - (int64_t)min_int32);
static_assert(subtract<int32_t>(min_int32, neg_int32) == (int32_t)min_int32 - (int32_t)neg_int32);
static_assert(subtract<int64_t>(pos_int32, neg_int32) == (int64_t)pos_int32 - (int64_t)neg_int32);
static_assert(subtract<int32_t>(neg_int32, pos_int32) == (int32_t)neg_int32 - (int32_t)pos_int32);
static_assert(subtract<int64_t>(pos_int32, min_int32) == (int64_t)pos_int32 - (int64_t)min_int32);
static_assert(std::is_same<decltype(subtract<uint64_t>(0, 0)), uint64_t>::value);

constexpr uint16_t unsigned_min = 0;
constexpr uint16_t unsigned_max = max_uint16;
constexpr uint16_t unsigned_half = to_half(unsigned_max);

constexpr auto signed_min = min_int16;
constexpr auto signed_zero = int16_t{ 0 };
constexpr auto signed_max = max_int16;
constexpr auto signed_half = to_half(signed_max);

// overflows (signed)
static_assert(!overflows(signed_zero, signed_zero));
static_assert(!overflows(signed_min, signed_zero));
static_assert(!overflows(signed_zero, signed_min));
static_assert(overflows(signed_min, signed_min));
static_assert(!overflows(signed_min, signed_max));
static_assert(!overflows(signed_max, signed_min));
static_assert(overflows(signed_max, signed_max));
static_assert(overflows(signed_max, signed_half));
static_assert(overflows(signed_half, signed_max));
static_assert(!overflows(signed_half, signed_half));

// overflows (unsigned)
static_assert(!overflows(unsigned_min, unsigned_min));
static_assert(!overflows(unsigned_min, unsigned_max));
static_assert(!overflows(unsigned_max, unsigned_min));
static_assert(overflows(unsigned_max, unsigned_max));
static_assert(overflows(unsigned_max, unsigned_half));
static_assert(overflows(unsigned_half, unsigned_max));
static_assert(!overflows(unsigned_half, unsigned_half) );

// underflows (signed)
static_assert(!underflows(signed_zero, signed_zero));
static_assert(!underflows(signed_min, signed_zero));
static_assert(underflows(signed_zero, signed_min));
static_assert(!underflows(signed_min, signed_zero));
static_assert(!underflows(signed_min, signed_min));
static_assert(underflows(signed_max, signed_min));
static_assert(underflows(signed_min, signed_max));
static_assert(!underflows(signed_max, signed_max));
static_assert(!underflows(signed_max, signed_half));
static_assert(!underflows(signed_half, signed_max));
static_assert(!underflows(signed_half, signed_half));

// underflows (unsigned)
static_assert(!underflows(unsigned_min, unsigned_min));
static_assert(!underflows(unsigned_max, unsigned_min));
static_assert(underflows(unsigned_min, unsigned_max));
static_assert(!underflows(unsigned_max, unsigned_max));
static_assert(!underflows(unsigned_max, unsigned_half));
static_assert(underflows(unsigned_half, unsigned_max));
static_assert(!underflows(unsigned_half, unsigned_half));

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
static_assert(std::is_same<decltype(ceilinged_add<uint16_t>(0, 0)), uint16_t>::value);

// ceilinged_add (unsigned)
static_assert(ceilinged_add(unsigned_min, unsigned_min) == unsigned_min);
static_assert(ceilinged_add(unsigned_min, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_min) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_max, unsigned_half) == unsigned_max);
static_assert(ceilinged_add(unsigned_half, unsigned_max) == unsigned_max);
static_assert(ceilinged_add(unsigned_half, unsigned_half) == sub1(unsigned_max));
static_assert(std::is_same<decltype(ceilinged_add<uint16_t>(0, 0)), uint16_t>::value);

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
static_assert(std::is_same<decltype(floored_subtract<int16_t>(0, 0)), int16_t>::value);

// floor_subtract (unsigned)
static_assert(floored_subtract(unsigned_min, unsigned_min) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_min) == unsigned_max);
static_assert(floored_subtract(unsigned_min, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_max, unsigned_half) == add1(unsigned_half));
static_assert(floored_subtract(unsigned_half, unsigned_max) == unsigned_min);
static_assert(floored_subtract(unsigned_half, unsigned_half) == unsigned_min);
static_assert(std::is_same<decltype(floored_subtract<uint16_t>(0, 0)), uint16_t>::value);

BC_POP_WARNING()
