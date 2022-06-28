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

// Test helpers to verify output signedness.
template <typename Integer, if_signed_integer<Integer> = true>
constexpr bool is_unsigned(Integer) { return false; }
template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr bool is_unsigned(Integer) { return true; }

// to_signed
static_assert(!is_unsigned(to_signed(-1)));
static_assert(!is_unsigned(to_signed(0)));
static_assert(!is_unsigned(to_signed(0u)));
static_assert(!is_unsigned(to_signed(1)));
static_assert(!is_unsigned(to_signed(1u)));
static_assert(to_signed(-2) == -2);
static_assert(to_signed(-1) == -1);
static_assert(to_signed(0) == 0);
static_assert(to_signed(0u) == 0);
static_assert(to_signed(1) == 1);
static_assert(to_signed(1u) == 1);
static_assert(to_signed(2u) == 2);

// to_unsigned
static_assert(is_unsigned(to_unsigned(-1)));
static_assert(is_unsigned(to_unsigned(0)));
static_assert(is_unsigned(to_unsigned(0u)));
static_assert(is_unsigned(to_unsigned(1)));
static_assert(is_unsigned(to_unsigned(1u)));
static_assert(to_unsigned(-2) == 0xfffffffe);
static_assert(to_unsigned(-1) == 0xffffffff);
static_assert(to_unsigned(0) == 0u);
static_assert(to_unsigned(0u) == 0u);
static_assert(to_unsigned(1) == 1u);
static_assert(to_unsigned(1u) == 1u);
static_assert(to_unsigned(2u) == 2u);

// absolute

static_assert(is_unsigned(absolute(-1)));
static_assert(is_unsigned(absolute(0)));
static_assert(is_unsigned(absolute(1u)));
static_assert(absolute(-1) == 1u);
static_assert(absolute(-42) == 42u);
static_assert(absolute(0) == 0u);
static_assert(absolute(0u) == 0u);
static_assert(absolute(1) == 1u);
static_assert(absolute(1u) == 1u);
static_assert(absolute(42) == 42);
static_assert(absolute(42u) == 42);
static_assert(absolute(max_int32) == max_int32);
static_assert(absolute(min_int32 + 1) == max_int32);
static_assert(absolute(max_uint32) == max_uint32);

// Due to lack of promotion, these are not constexpr.
// -minimum<integral> is undefined unless promotable.
////static_assert(absolute(min_int32) == 0);
////static_assert(absolute(min_int64) == 0);

// These are both overflows, allowed due to native int promotion.
static_assert(absolute(min_int8)  == add1<uint16_t>(max_int8));
static_assert(absolute(min_int16) == add1<uint32_t>(max_int16));

// These can be mitigated using explicit domain promotion (except for uint64_t).
static_assert(absolute<int16_t>(min_int8) == add1<uint16_t>(max_int8));
static_assert(absolute<int32_t>(min_int16) == add1<uint32_t>(max_int16));
static_assert(absolute<int64_t>(min_int32) == add1<uint64_t>(max_int32));

// negate

static_assert(negate(0)   ==  0);
static_assert(negate(0u)  ==  0);
static_assert(negate(1)   == -1);
static_assert(negate(1u)  == -1);
static_assert(negate(-1)  ==  1);
static_assert(negate(42)  == -42);
static_assert(negate(-42) ==  42);
static_assert(negate(42u) == -42);

static_assert(negate(max_int8)  == -max_int8);
static_assert(negate(max_int16) == -max_int16);
static_assert(negate(max_int32) == -max_int32);
static_assert(negate(max_int64) == -max_int64);

static_assert(negate(max_uint8)  != -max_uint8);                // -promotion
static_assert(negate(max_uint16) != -max_uint16);               // -promotion
static_assert(negate(max_uint8)  != -max_uint8);                // -promotion
static_assert(negate(max_uint16) != -max_uint16);               // -promotion

static_assert(negate(max_uint8)  ==  uint8_t(-max_uint8));      // truncate -promotion
static_assert(negate(max_uint16) == uint16_t(-max_uint16));     // truncate -promotion
static_assert(negate(max_uint32) == to_unsigned(-to_signed(max_uint32)));    // compiler warns on lack of sign change
static_assert(negate(max_uint64) == to_unsigned(-to_signed(max_uint64)));    // compiler warns on lack of sign change

static_assert(negate(0)   == twos_complement(0));
static_assert(negate(0u)  == twos_complement(0u));
static_assert(negate(1)   == twos_complement(1));
static_assert(negate(1u)  == twos_complement(1u));
static_assert(negate(-1)  == twos_complement(-1));
static_assert(negate(-42) == twos_complement(-42));
static_assert(negate(42u) == twos_complement(42u));

static_assert(negate(max_uint8)  == twos_complement(max_uint8));
static_assert(negate(max_uint16) == twos_complement(max_uint16));
static_assert(negate(max_uint32) == twos_complement(max_uint32));
static_assert(negate(max_uint64) == twos_complement(max_uint64));

// Due to lack of promotion, these are not constexpr.
// -minimum<integral> is undefined unless promotable.
////static_assert(negate(min_int32) == 0);
////static_assert(negate(min_int64) == 0);

// These are both overflows, allowed due to native int promotion.
static_assert(negate(min_int8)  == add1(max_int8));
static_assert(negate(min_int16) == add1(max_int16));

// These can be mitigated using explicit domain promotion (except for uint64_t).
static_assert(negate<int16_t>(min_int8)  == add1<int16_t>(max_int8));
static_assert(negate<int32_t>(min_int16) == add1<int32_t>(max_int16));
static_assert(negate<int64_t>(min_int32) == add1<int64_t>(max_int32));

// uint32 requires explicit promotion before negation.
static_assert(negate<int16_t>(min_int8)  == -min_int8);
static_assert(negate<int32_t>(min_int16) == -min_int16);
static_assert(negate<int64_t>(min_int32) == -int64_t(min_int32));
////static_assert(negate<int64_t>(min_int64) == -min_int64);

// is_negative
static_assert(is_negative(min_int32));
static_assert(is_negative(-1));
static_assert(is_negative(-42));
static_assert(!is_negative(0u));
static_assert(!is_negative(1u));
static_assert(!is_negative(42u));
static_assert(!is_negative(max_int32));
static_assert(!is_negative(max_uint32));

// to_unsigned
static_assert(is_greater(1, 0));
static_assert(is_greater(1u, 0));
static_assert(is_greater(1u, 0u));
static_assert(is_greater(0, -1));
static_assert(is_greater(0u, -1));
static_assert(!is_greater(-1, -1));
static_assert(!is_greater(0u, 0));
static_assert(!is_greater(0u, 0u));
static_assert(!is_greater(0, 0u));
static_assert(!is_greater(1, 1));
static_assert(!is_greater(1u, 1));
static_assert(!is_greater(1u, 1u));
static_assert(!is_greater(1, 1u));
static_assert(!is_greater(0, 1));
static_assert(!is_greater(-1, 0));

// to_unsigned
static_assert(!is_lesser(1, 0));
static_assert(!is_lesser(1u, 0));
static_assert(!is_lesser(1u, 0u));
static_assert(!is_lesser(0, -1));
static_assert(!is_lesser(0u, -1));
static_assert(!is_lesser(-1, -1));
static_assert(!is_lesser(0u, 0));
static_assert(!is_lesser(0u, 0u));
static_assert(!is_lesser(0, 0u));
static_assert(!is_lesser(1, 1));
static_assert(!is_lesser(1u, 1));
static_assert(!is_lesser(1u, 1u));
static_assert(!is_lesser(1, 1u));
static_assert(is_lesser(0, 1));
static_assert(is_lesser(-1, 0));

// greater

static_assert(greater<signed>(1, 0) == 1);
static_assert(greater<signed>(0, 1) == 1);
static_assert(greater<signed>(1u, 0u) == 1);
static_assert(greater<signed>(0u, 1u) == 1);
static_assert(greater<signed>(1, 0u) == 1);
static_assert(greater<signed>(0, 1u) == 1);
static_assert(greater<signed>(1u, 0) == 1);
static_assert(greater<signed>(0u, 1) == 1);

static_assert(greater<signed>(0, 0) == 0);
static_assert(greater<signed>(1, 1) == 1);
static_assert(greater<signed>(0u, 0u) == 0);
static_assert(greater<signed>(1u, 1u) == 1);
static_assert(greater<signed>(0u, 0) == 0);
static_assert(greater<signed>(1u, 1) == 1);
static_assert(greater<signed>(0, 0u) == 0);
static_assert(greater<signed>(1, 1u) == 1);

static_assert(greater<unsigned>(1, 0) == 1u);
static_assert(greater<unsigned>(0, 1) == 1u);
static_assert(greater<unsigned>(1u, 0u) == 1u);
static_assert(greater<unsigned>(0u, 1u) == 1u);
static_assert(greater<unsigned>(1, 0u) == 1u);
static_assert(greater<unsigned>(0, 1u) == 1u);
static_assert(greater<unsigned>(1u, 0) == 1u);
static_assert(greater<unsigned>(0u, 1) == 1u);

static_assert(greater<unsigned>(0, 0) == 0u);
static_assert(greater<unsigned>(1, 1) == 1u);
static_assert(greater<unsigned>(0u, 0u) == 0u);
static_assert(greater<unsigned>(1u, 1u) == 1u);
static_assert(greater<unsigned>(0u, 0) == 0u);
static_assert(greater<unsigned>(1u, 1) == 1u);
static_assert(greater<unsigned>(0, 0u) == 0u);
static_assert(greater<unsigned>(1, 1u) == 1u);

// lesser

static_assert(lesser<signed>(1, 0) == 0);
static_assert(lesser<signed>(0, 1) == 0);
static_assert(lesser<signed>(1u, 0u) == 0);
static_assert(lesser<signed>(0u, 1u) == 0);
static_assert(lesser<signed>(1, 0u) == 0);
static_assert(lesser<signed>(0, 1u) == 0);
static_assert(lesser<signed>(1u, 0) == 0);
static_assert(lesser<signed>(0u, 1) == 0);

static_assert(lesser<signed>(0, 0) == 0);
static_assert(lesser<signed>(1, 1) == 1);
static_assert(lesser<signed>(0u, 0u) == 0);
static_assert(lesser<signed>(1u, 1u) == 1);
static_assert(lesser<signed>(0u, 0) == 0);
static_assert(lesser<signed>(1u, 1) == 1);
static_assert(lesser<signed>(0, 0u) == 0);
static_assert(lesser<signed>(1, 1u) == 1);

static_assert(lesser<unsigned>(1, 0) == 0u);
static_assert(lesser<unsigned>(0, 1) == 0u);
static_assert(lesser<unsigned>(1u, 0u) == 0u);
static_assert(lesser<unsigned>(0u, 1u) == 0u);
static_assert(lesser<unsigned>(1, 0u) == 0u);
static_assert(lesser<unsigned>(0, 1u) == 0u);
static_assert(lesser<unsigned>(1u, 0) == 0u);
static_assert(lesser<unsigned>(0u, 1) == 0u);

static_assert(lesser<unsigned>(0, 0) == 0u);
static_assert(lesser<unsigned>(1, 1) == 1u);
static_assert(lesser<unsigned>(0u, 0u) == 0u);
static_assert(lesser<unsigned>(1u, 1u) == 1u);
static_assert(lesser<unsigned>(0u, 0) == 0u);
static_assert(lesser<unsigned>(1u, 1) == 1u);
static_assert(lesser<unsigned>(0, 0u) == 0u);
static_assert(lesser<unsigned>(1, 1u) == 1u);
