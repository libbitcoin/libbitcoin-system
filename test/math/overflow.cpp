/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

// creates overflow_tests namespace
BOOST_AUTO_TEST_SUITE(overflow_tests)

constexpr uint16_t unsigned_min = 0_u16;
constexpr uint16_t unsigned_max = max_uint16;
constexpr uint16_t unsigned_half = to_half(unsigned_max);

constexpr auto signed_min = min_int16;
constexpr auto signed_zero = 0_i16;
constexpr auto signed_max = max_int16;
constexpr auto signed_half = to_half(signed_max);

// is_left_shift_overflow
// ----------------------------------------------------------------------------

// unsigned only
static_assert(!is_left_shift_overflow<uint8_t>(0b00000000, 0));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000000, 1));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000000, 2));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000000, 4));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000000, 8));

static_assert(!is_left_shift_overflow<uint8_t>(0b00000001, 0));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000001, 1));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000001, 2));
static_assert(!is_left_shift_overflow<uint8_t>(0b00000001, 4));
static_assert( is_left_shift_overflow<uint8_t>(0b00000001, 8));

static_assert(!is_left_shift_overflow<uint8_t>(0b10000000, 0));
static_assert( is_left_shift_overflow<uint8_t>(0b10000000, 1));
static_assert( is_left_shift_overflow<uint8_t>(0b10000000, 2));
static_assert( is_left_shift_overflow<uint8_t>(0b10000000, 4));
static_assert( is_left_shift_overflow<uint8_t>(0b10000000, 8));

static_assert(!is_left_shift_overflow<uint8_t>(0b00101010, 0));
static_assert(!is_left_shift_overflow<uint8_t>(0b00101010, 1));
static_assert(!is_left_shift_overflow<uint8_t>(0b00101010, 2));
static_assert( is_left_shift_overflow<uint8_t>(0b00101010, 4));
static_assert( is_left_shift_overflow<uint8_t>(0b00101010, 8));

static_assert(!is_left_shift_overflow<uint8_t>(0b11111111, 0));
static_assert( is_left_shift_overflow<uint8_t>(0b11111111, 1));
static_assert( is_left_shift_overflow<uint8_t>(0b11111111, 2));
static_assert( is_left_shift_overflow<uint8_t>(0b11111111, 4));
static_assert( is_left_shift_overflow<uint8_t>(0b11111111, 8));

static_assert(!is_left_shift_overflow<uint16_t>(0b00010000'00000000, 0));
static_assert(!is_left_shift_overflow<uint16_t>(0b00010000'00000000, 1));
static_assert(!is_left_shift_overflow<uint16_t>(0b00010000'00000000, 2));
static_assert( is_left_shift_overflow<uint16_t>(0b00010000'00000000, 4));
static_assert( is_left_shift_overflow<uint16_t>(0b00010000'00000000, 8));
static_assert( is_left_shift_overflow<uint16_t>(0b00000000'00000001, bits<uint16_t>));
static_assert(!is_left_shift_overflow<uint16_t>(0b00000000'00000001, sub1(bits<uint16_t>)));

static_assert(!is_left_shift_overflow<uint32_t>(0b00010000'00000000'00000000'00000000, 0));
static_assert(!is_left_shift_overflow<uint32_t>(0b00010000'00000000'00000000'00000000, 1));
static_assert(!is_left_shift_overflow<uint32_t>(0b00010000'00000000'00000000'00000000, 2));
static_assert( is_left_shift_overflow<uint32_t>(0b00010000'00000000'00000000'00000000, 4));
static_assert( is_left_shift_overflow<uint32_t>(0b00010000'00000000'00000000'00000000, 8));
static_assert( is_left_shift_overflow<uint32_t>(0b00000000'00000000'00000000'00000001, bits<uint32_t>));
static_assert(!is_left_shift_overflow<uint32_t>(0b00000000'00000000'00000000'00000001, sub1(bits<uint32_t>)));

static_assert(!is_left_shift_overflow<uint64_t>(0b00010000'00000000'00000000'00000000'00010000'00000000'00000000'00000000, 0));
static_assert(!is_left_shift_overflow<uint64_t>(0b00010000'00000000'00000000'00000000'00010000'00000000'00000000'00000000, 1));
static_assert(!is_left_shift_overflow<uint64_t>(0b00010000'00000000'00000000'00000000'00010000'00000000'00000000'00000000, 2));
static_assert( is_left_shift_overflow<uint64_t>(0b00010000'00000000'00000000'00000000'00010000'00000000'00000000'00000000, 4));
static_assert( is_left_shift_overflow<uint64_t>(0b00010000'00000000'00000000'00000000'00010000'00000000'00000000'00000000, 8));
static_assert( is_left_shift_overflow<uint64_t>(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001, bits<uint64_t>));
static_assert(!is_left_shift_overflow<uint64_t>(0b00000000'00000000'00000000'00000000'00000000'00000000'00000000'00000001, sub1(bits<uint64_t>)));

// is_right_shift_overflow
// ----------------------------------------------------------------------------

// is_negate_overflow
// ----------------------------------------------------------------------------

// signed only
static_assert(!is_negate_overflow(0));
static_assert(!is_negate_overflow(1));
static_assert(!is_negate_overflow(-1));
static_assert(!is_negate_overflow(42));
static_assert(!is_negate_overflow(-42));
static_assert(is_negate_overflow(min_int8));
static_assert(is_negate_overflow(min_int16));
static_assert(is_negate_overflow(min_int32));
static_assert(is_negate_overflow(min_int64));

// is_add_overflow
// ----------------------------------------------------------------------------

// signed
static_assert(!is_add_overflow(signed_zero, signed_zero));
static_assert(!is_add_overflow(signed_min, signed_zero));
static_assert(!is_add_overflow(signed_zero, signed_min));
static_assert(is_add_overflow(signed_min, signed_min));
static_assert(!is_add_overflow(signed_min, signed_max));
static_assert(!is_add_overflow(signed_max, signed_min));
static_assert(is_add_overflow(signed_max, signed_max));
static_assert(is_add_overflow(signed_max, signed_half));
static_assert(is_add_overflow(signed_half, signed_max));
static_assert(!is_add_overflow(signed_half, signed_half));

// unsigned
static_assert(!is_add_overflow(unsigned_min, unsigned_min));
static_assert(!is_add_overflow(unsigned_min, unsigned_max));
static_assert(!is_add_overflow(unsigned_max, unsigned_min));
static_assert(is_add_overflow(unsigned_max, unsigned_max));
static_assert(is_add_overflow(unsigned_max, unsigned_half));
static_assert(is_add_overflow(unsigned_half, unsigned_max));
static_assert(!is_add_overflow(unsigned_half, unsigned_half));

// is_subtract_overflow
// ----------------------------------------------------------------------------

// signed
static_assert(!is_subtract_overflow(signed_zero, signed_zero));
static_assert(!is_subtract_overflow(signed_min, signed_zero));
static_assert(is_subtract_overflow(signed_zero, signed_min));
static_assert(!is_subtract_overflow(signed_min, signed_zero));
static_assert(!is_subtract_overflow(signed_min, signed_min));
static_assert(is_subtract_overflow(signed_max, signed_min));
static_assert(is_subtract_overflow(signed_min, signed_max));
static_assert(!is_subtract_overflow(signed_max, signed_max));
static_assert(!is_subtract_overflow(signed_max, signed_half));
static_assert(!is_subtract_overflow(signed_half, signed_max));
static_assert(!is_subtract_overflow(signed_half, signed_half));

// unsigned
static_assert(!is_subtract_overflow(unsigned_min, unsigned_min));
static_assert(!is_subtract_overflow(unsigned_max, unsigned_min));
static_assert(is_subtract_overflow(unsigned_min, unsigned_max));
static_assert(!is_subtract_overflow(unsigned_max, unsigned_max));
static_assert(!is_subtract_overflow(unsigned_max, unsigned_half));
static_assert(is_subtract_overflow(unsigned_half, unsigned_max));
static_assert(!is_subtract_overflow(unsigned_half, unsigned_half));

// is_multiply_overflow
// ----------------------------------------------------------------------------

// unsigned only
static_assert(!is_multiply_overflow(0u, 0u));
static_assert(!is_multiply_overflow(1u, 0u));
static_assert(!is_multiply_overflow(0u, 1u));
static_assert(!is_multiply_overflow(unsigned_max, 1_u16));
static_assert(!is_multiply_overflow(1_u16, unsigned_max));
static_assert(!is_multiply_overflow(unsigned_min, 1_u16));
static_assert(!is_multiply_overflow(1_u16, unsigned_min));
static_assert(!is_multiply_overflow(unsigned_min, 2_u16));
static_assert(!is_multiply_overflow(2_u16, unsigned_min));
static_assert(is_multiply_overflow(unsigned_max, 2_u16));
static_assert(is_multiply_overflow(2_u16, unsigned_max));

// is_divide_overflow
// ----------------------------------------------------------------------------

// both signed
static_assert(is_divide_overflow(0, 0));
static_assert(is_divide_overflow(1, 0));
static_assert(!is_divide_overflow(0, 1));
static_assert(!is_divide_overflow(signed_max, 1));
static_assert(!is_divide_overflow(signed_min, 1));
static_assert(!is_divide_overflow(42, signed_max));
static_assert(!is_divide_overflow(42, signed_min));

// both unsigned
static_assert(is_divide_overflow(0u, 0u));
static_assert(is_divide_overflow(1u, 0u));
static_assert(!is_divide_overflow(0u, 1u));
static_assert(!is_divide_overflow(unsigned_max, 1u));
static_assert(!is_divide_overflow(unsigned_min, 1u));
static_assert(!is_divide_overflow(42u, unsigned_max));
static_assert(is_divide_overflow(42u, unsigned_min));

// safe_...
// ----------------------------------------------------------------------------

constexpr size_t minimal = 0;
constexpr size_t maximal = max_size_t;
constexpr size_t half = to_half(maximal);

// safe_negate (signed only)
static_assert(safe_negate(0) == 0);
static_assert(safe_negate(1) == -1);
static_assert(safe_negate(-1) == +1);
static_assert(safe_negate(-42) == +42);
static_assert(safe_negate(max_int32) == add1(min_int32));
static_assert(safe_negate(add1(min_int32)) == max_int32);
////static_assert(safe_negate(min_int32) == (-min_int32));

// safe_add (same-signed only)
static_assert(safe_add(-1, -1) == -2);
static_assert(safe_add(1u, 1u) == 2u);
static_assert(safe_add(minimal, minimal) == minimal);
static_assert(safe_add(minimal, maximal) == maximal);
static_assert(safe_add(maximal, minimal) == maximal);
static_assert(add(maximal, maximal) == sub1(maximal));      // overflow
////static_assert(safe_add(maximal, maximal) == sub1(maximal));

// safe_subtract (same-signed only)
static_assert(safe_subtract(-1, -1) == 0);
static_assert(safe_subtract(1u, 1u) == 0);
static_assert(safe_subtract(maximal, minimal) == maximal);
static_assert(safe_subtract(maximal, maximal) == 0_size);
static_assert(subtract(minimal, maximal) == add1(minimal)); // overflow
////static_assert(safe_subtract(minimal, maximal) == add1(minimal));

// safe_multiply (unsigned only)
static_assert(safe_multiply(1u, 1u) == 1u);
static_assert(safe_multiply(minimal, minimal) == minimal);
static_assert(safe_multiply(minimal, maximal) == minimal);
static_assert(safe_multiply(maximal, minimal) == minimal);
static_assert(safe_multiply(half, 2_size) == sub1(maximal));
static_assert(safe_multiply(2_size, half) == sub1(maximal));
////static_assert(safe_multiply(maximal, maximal) == (maximal * maximal));

// TODO:
// safe_divide (signed/unsigned)
// safe_modulo (signed/unsigned)

// TODO: boost test is not catching these thrown exceptions (related to disabled exceptions?).
#if defined(ENABLED)

BOOST_AUTO_TEST_CASE(overflow__safe_negate__int32_minimum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_negate(min_int32), overflow_exception);
}

BOOST_AUTO_TEST_CASE(overflow__safe_add__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(maximal, maximal), overflow_exception);
}

BOOST_AUTO_TEST_CASE(overflow__safe_subtract__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_subtract(minimal, maximal), overflow_exception);
}

BOOST_AUTO_TEST_CASE(overflow__safe_multiply__size_t_maximum_times_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(maximal, maximal), overflow_exception);
}

BOOST_AUTO_TEST_CASE(overflow__safe_divide__by_zero__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_divide(maximal, minimal), overflow_exception);
}

BOOST_AUTO_TEST_CASE(overflow__safe_modulo__by_zero__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_modulo(maximal, minimal), overflow_exception);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
