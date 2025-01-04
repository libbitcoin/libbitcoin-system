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
#include "test.hpp"

// Conditions

static_assert(is_zero(0));
static_assert(!is_zero(1u));
static_assert(!is_zero(0xff));
static_assert(is_same_type<decltype(is_zero<int16_t>(0)), bool>);

static_assert(!is_nonzero(0));
static_assert(is_nonzero(1u));
static_assert(is_nonzero(0xff));
static_assert(is_same_type<decltype(is_nonzero<int16_t>(0)), bool>);

static_assert(is_one(1));
static_assert(!is_one(0u));
static_assert(!is_one(0xff));
static_assert(is_same_type<decltype(is_one<int16_t>(0)), bool>);

static_assert(lo_bit(0) == 0);
static_assert(lo_bit(2u) == 0);
static_assert(lo_bit(42) == 0);
static_assert(lo_bit(max_uint32 - 1u) == 0);
static_assert(lo_bit(1) == 1);
static_assert(lo_bit(3u) == 1);
static_assert(lo_bit(5) == 1);
static_assert(lo_bit(max_uint32) == 1);
static_assert(is_same_type<decltype(lo_bit<int16_t>(0)), int16_t>);

static_assert(is_even(0));
static_assert(is_even(2u));
static_assert(is_even(42));
static_assert(is_even(max_uint32 - 1u));
static_assert(!is_even(1));
static_assert(!is_even(3u));
static_assert(!is_even(0xff));
static_assert(!is_even(max_uint32));
static_assert(is_same_type<decltype(is_even<int16_t>(0)), bool>);

static_assert(is_odd(1));
static_assert(is_odd(3u));
static_assert(is_odd(5));
static_assert(is_odd(max_uint32));
static_assert(!is_odd(0u));
static_assert(!is_odd(2));
static_assert(!is_odd(10));
static_assert(!is_odd(max_uint32 - 1u));
static_assert(is_same_type<decltype(is_odd<int16_t>(0)), bool>);

static_assert(is_null(nullptr));
static_assert(is_null(std::nullptr_t{}));
static_assert(is_same_type<decltype(is_null(nullptr)), bool>);

static_assert(!is_byte_sized(0));
static_assert(is_byte_sized(8));
static_assert(is_byte_sized(16));
static_assert(is_byte_sized(24));
static_assert(is_byte_sized(32));
static_assert(is_byte_sized(40));
static_assert(!is_byte_sized(42));
static_assert(is_byte_sized(48));
static_assert(is_byte_sized(56));
static_assert(is_byte_sized(64));
static_assert(is_byte_sized(128));
static_assert(is_byte_sized(256));
static_assert(is_same_type<decltype(is_byte_sized(0)), bool>);

static_assert(!is_integral_sized(0));
static_assert(is_integral_sized(1));
static_assert(is_integral_sized(2));
static_assert(!is_integral_sized(3));
static_assert(is_integral_sized(4));
static_assert(!is_integral_sized(5));
static_assert(!is_integral_sized(6));
static_assert(!is_integral_sized(7));
static_assert(is_integral_sized(8));
static_assert(!is_integral_sized(9));
static_assert(!is_integral_sized(64));
static_assert(is_same_type<decltype(is_integral_sized(0)), bool>);

// Conversions

static_assert(add1(0) == 0 + 1);
static_assert(add1(1u) == 1u + 1u);
static_assert(add1(-42) == -42 + 1);
static_assert(add1(0xff) == 0xff + 1);
static_assert(is_same_type<decltype(add1<int16_t>(0)), int16_t>);

static_assert(sub1(0xff) == 0xff - 1);
static_assert(sub1(1u) == 1u - 1u);
static_assert(sub1(-42) == -42 - 1);
static_assert(sub1(0xff) == 0xff - 1);
static_assert(is_same_type<decltype(sub1<int16_t>(0)), int16_t>);

static_assert(to_bits(0) == 0 * 8);
static_assert(to_bits(1u) == 1u * 8u);
static_assert(to_bits(-42) == -42 * 8);
static_assert(to_bits(0xff) == 0xff * 8);
static_assert(is_same_type<decltype(to_bits<int16_t>(0)), int16_t>);

static_assert(to_byte('\0') == uint8_t{ 0 });
static_assert(to_byte('x') == uint8_t{ 'x' });
static_assert(to_byte('\xff') == uint8_t{ 255 });
static_assert(is_same_type<decltype(to_byte('\0')), uint8_t>);

static_assert(to_half(0) == 0 / 2);
static_assert(to_half(1u) == 1u / 2u);
static_assert(to_half(-42) == -42 / 2);
static_assert(to_half(0xff) == 0xff / 2);
static_assert(is_same_type<decltype(to_half<int16_t>(0)), int16_t>);

static_assert(to_int(true) == 1);
static_assert(to_int(false) == 0);
static_assert(is_same_type<decltype(to_int<int16_t>(0)), int16_t>);

static_assert(to_bool(1));
static_assert(to_bool(-1));
static_assert(to_bool(-42));
static_assert(to_bool(42));
static_assert(!to_bool(0));
static_assert(is_same_type<decltype(to_bool<int16_t>(0)), bool>);

static_assert(variable_size(zero) == 1u);
static_assert(variable_size(1u) == 1u);
static_assert(variable_size(0xfeu) == 3u);
static_assert(variable_size(0x10000u) == 5u);
static_assert(variable_size(0x100000000u) == 9u);
static_assert(variable_size(max_uint8) == 3u);
static_assert(variable_size(max_uint16) == 3u);
static_assert(variable_size(max_uint32) == 5u);
static_assert(variable_size(max_uint32) == 5u);
static_assert(variable_size(max_uint64) == 9u);
static_assert(is_same_type<decltype(variable_size(zero)), size_t>);

static_assert(size_variable(zero) == 1u);
static_assert(size_variable(1) == 1u);
static_assert(size_variable(42) == 1u);
static_assert(size_variable(252) == 1u);
static_assert(size_variable(varint_two_bytes) == 3u);
static_assert(size_variable(varint_four_bytes) == 5u);
static_assert(size_variable(varint_eight_bytes) == 9u);
static_assert(is_same_type<decltype(size_variable(zero)), size_t>);
