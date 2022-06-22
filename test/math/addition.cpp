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

////// increment
////static_assert(increment<int64_t>(max_int32) == (int64_t)max_int32 + int64_t{ 1 });
////static_assert(increment<int64_t>(min_uint32) == (int64_t)min_uint32 + int64_t{ 1 });
////static_assert(increment<int64_t>(pos_uint32) == (int64_t)pos_uint32 + int64_t{ 1 });
////static_assert(increment<int64_t>(zer_int32) == (int64_t)zer_int32 + int64_t{ 1 });
////static_assert(increment<int64_t>(neg_int32) == (int64_t)neg_int32 + int64_t{ 1 });
////static_assert(increment<int64_t>(pos_int32) == (int64_t)pos_int32 + int64_t{ 1 });
////static_assert(increment<int32_t>(neg_int32) == (int32_t)neg_int32 + int32_t{ 1 });
////static_assert(increment<int32_t>(min_int32) == (int32_t)min_int32 + int32_t{ 1 });
////static_assert(std::is_same<decltype(increment<uint64_t>(0)), uint64_t>::value);
////
////// decrement
////static_assert(decrement<int64_t>(min_int32) == (int64_t)min_int32 - int64_t{ 1 });
////static_assert(decrement<int64_t>(min_uint32) == (int64_t)min_uint32 - int64_t{ 1 });
////static_assert(decrement<int64_t>(pos_uint32) == (int64_t)pos_uint32 - int64_t{ 1 });
////static_assert(decrement<int64_t>(zer_int32) == (int64_t)zer_int32 - int64_t{ 1 });
////static_assert(decrement<int64_t>(neg_int32) == (int64_t)neg_int32 - int64_t{ 1 });
////static_assert(decrement<int64_t>(pos_int32) == (int64_t)pos_int32 - int64_t{ 1 });
////static_assert(decrement<int32_t>(neg_int32) == (int32_t)neg_int32 - int32_t{ 1 });
////static_assert(std::is_same<decltype(decrement<uint64_t>(0)), uint64_t>::value);

constexpr uint16_t unsigned_minimum = 0;
constexpr uint16_t unsigned_maximum = max_uint16;
constexpr uint16_t unsigned_half = to_half(unsigned_maximum);

constexpr auto signed_minimum = min_int16;
constexpr auto signed_zero = int16_t{ 0 };
constexpr auto signed_maximum = max_int16;
constexpr auto signed_half = to_half(signed_maximum);

// overflows (signed)
static_assert(!overflows(signed_zero, signed_zero));
static_assert(!overflows(signed_minimum, signed_zero));
static_assert(!overflows(signed_zero, signed_minimum));
static_assert(overflows(signed_minimum, signed_minimum));
static_assert(!overflows(signed_minimum, signed_maximum));
static_assert(!overflows(signed_maximum, signed_minimum));
static_assert(overflows(signed_maximum, signed_maximum));
static_assert(overflows(signed_maximum, signed_half));
static_assert(overflows(signed_half, signed_maximum));
static_assert(!overflows(signed_half, signed_half));

// overflows (unsigned)
static_assert(!overflows(unsigned_minimum, unsigned_minimum));
static_assert(!overflows(unsigned_minimum, unsigned_maximum));
static_assert(!overflows(unsigned_maximum, unsigned_minimum));
static_assert(overflows(unsigned_maximum, unsigned_maximum));
static_assert(overflows(unsigned_maximum, unsigned_half));
static_assert(overflows(unsigned_half, unsigned_maximum));
static_assert(!overflows(unsigned_half, unsigned_half) );

// underflows (signed)
static_assert(!underflows(signed_zero, signed_zero));
static_assert(!underflows(signed_minimum, signed_zero));
static_assert(underflows(signed_zero, signed_minimum));
static_assert(!underflows(signed_minimum, signed_zero));
static_assert(!underflows(signed_minimum, signed_minimum));
static_assert(underflows(signed_maximum, signed_minimum));
static_assert(underflows(signed_minimum, signed_maximum));
static_assert(!underflows(signed_maximum, signed_maximum));
static_assert(!underflows(signed_maximum, signed_half));
static_assert(!underflows(signed_half, signed_maximum));
static_assert(!underflows(signed_half, signed_half));

// underflows (unsigned)
static_assert(!underflows(unsigned_minimum, unsigned_minimum));
static_assert(!underflows(unsigned_maximum, unsigned_minimum));
static_assert(underflows(unsigned_minimum, unsigned_maximum));
static_assert(!underflows(unsigned_maximum, unsigned_maximum));
static_assert(!underflows(unsigned_maximum, unsigned_half));
static_assert(underflows(unsigned_half, unsigned_maximum));
static_assert(!underflows(unsigned_half, unsigned_half));

// ceilinged_add (signed)
static_assert(ceilinged_add(signed_zero, signed_zero) == signed_zero);
static_assert(ceilinged_add(signed_minimum, signed_zero) == signed_minimum);
static_assert(ceilinged_add(signed_zero, signed_minimum) == signed_minimum);
static_assert(ceilinged_add(signed_minimum, signed_minimum) == signed_minimum);
static_assert(ceilinged_add(signed_minimum, signed_maximum) == -1);
static_assert(ceilinged_add(signed_maximum, signed_minimum) == -1);
static_assert(ceilinged_add(signed_maximum, signed_maximum) == signed_maximum);
static_assert(ceilinged_add(signed_maximum, signed_half) == signed_maximum);
static_assert(ceilinged_add(signed_half, signed_maximum) == signed_maximum);
static_assert(ceilinged_add(signed_half, signed_half) == sub1(signed_maximum));
static_assert(std::is_same<decltype(ceilinged_add<uint16_t>(0, 0)), uint16_t>::value);

// ceilinged_add (unsigned)
static_assert(ceilinged_add(unsigned_minimum, unsigned_minimum) == unsigned_minimum);
static_assert(ceilinged_add(unsigned_minimum, unsigned_maximum) == unsigned_maximum);
static_assert(ceilinged_add(unsigned_maximum, unsigned_minimum) == unsigned_maximum);
static_assert(ceilinged_add(unsigned_maximum, unsigned_maximum) == unsigned_maximum);
static_assert(ceilinged_add(unsigned_maximum, unsigned_half) == unsigned_maximum);
static_assert(ceilinged_add(unsigned_half, unsigned_maximum) == unsigned_maximum);
static_assert(ceilinged_add(unsigned_half, unsigned_half) == sub1(unsigned_maximum));
static_assert(std::is_same<decltype(ceilinged_add<uint16_t>(0, 0)), uint16_t>::value);

// floor_subtract (signed)
static_assert(floored_subtract(signed_zero, signed_zero) == signed_zero);
static_assert(floored_subtract(signed_minimum, signed_zero) == signed_minimum);
static_assert(floored_subtract(signed_zero, signed_minimum) == signed_maximum);
static_assert(floored_subtract(signed_minimum, signed_zero) == signed_minimum);
static_assert(floored_subtract(signed_minimum, signed_minimum) == 0);
static_assert(floored_subtract(signed_maximum, signed_minimum) == signed_maximum);
static_assert(floored_subtract(signed_minimum, signed_maximum) == signed_minimum);
static_assert(floored_subtract(signed_maximum, signed_maximum) == 0);
static_assert(floored_subtract(signed_maximum, signed_half) == add1(+signed_half));
static_assert(floored_subtract(signed_half, signed_maximum) == sub1(-signed_half));
static_assert(floored_subtract(signed_half, signed_half) == 0);
static_assert(std::is_same<decltype(floored_subtract<int16_t>(0, 0)), int16_t>::value);

// floor_subtract (unsigned)
static_assert(floored_subtract(unsigned_minimum, unsigned_minimum) == unsigned_minimum);
static_assert(floored_subtract(unsigned_maximum, unsigned_minimum) == unsigned_maximum);
static_assert(floored_subtract(unsigned_minimum, unsigned_maximum) == unsigned_minimum);
static_assert(floored_subtract(unsigned_maximum, unsigned_maximum) == unsigned_minimum);
static_assert(floored_subtract(unsigned_maximum, unsigned_half) == add1(unsigned_half));
static_assert(floored_subtract(unsigned_half, unsigned_maximum) == unsigned_minimum);
static_assert(floored_subtract(unsigned_half, unsigned_half) == unsigned_minimum);
static_assert(std::is_same<decltype(floored_subtract<uint16_t>(0, 0)), uint16_t>::value);

BC_POP_WARNING()
