/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

static_assert(zero == 0u, "constants__zero__always__zero_unsigned");
static_assert(one == 1u, "constants__one__always__one_unsigned");
static_assert(two == 2u, "constants__negative_one__always__negative_one");
static_assert(byte_bits == 8u, "constants__byte_bits__always__eight_unsigned");
static_assert(negative_one == -1, "constants__negative_one__always__negative_one");

static_assert(is_zero(0), "constants__is_zero__true");
static_assert(!is_zero(1u), "constants__is_zero__false");
static_assert(!is_zero(0xff), "constants__is_zero__false");

static_assert(is_one(1), "constants__is_one__true");
static_assert(!is_one(0u), "constants__is_one__false");
static_assert(!is_one(0xff), "constants__is_one__false");

static_assert(is_even(0), "constants__is_even__true");
static_assert(is_even(2u), "constants__is_even__true");
static_assert(is_even(42), "constants__is_even__true");
static_assert(is_even(max_uint32 - 1u), "constants__is_even__true");

static_assert(!is_even(1), "constants__is_even__false");
static_assert(!is_even(3u), "constants__is_even__false");
static_assert(!is_even(0xff), "constants__is_even__false");
static_assert(!is_even(max_uint32), "constants__is_even__false");

static_assert(is_odd(1), "constants__is_odd__true");
static_assert(is_odd(3u), "constants__is_odd__true");
static_assert(is_odd(5), "constants__is_odd__true");
static_assert(is_odd(max_uint32), "constants__is_odd__true");

static_assert(!is_odd(0u), "constants__is_odd__false");
static_assert(!is_odd(2), "constants__is_odd__false");
static_assert(!is_odd(10), "constants__is_odd__false");
static_assert(!is_odd(max_uint32 - 1u), "constants__is_odd__false");

static_assert(is_byte_aligned(0), "constants__is_byte_aligned__true");
static_assert(is_byte_aligned(8), "constants__is_byte_aligned__true");
static_assert(is_byte_aligned(16), "constants__is_byte_aligned__true");
static_assert(is_byte_aligned(128), "constants__is_byte_aligned__true");

static_assert(!is_byte_aligned(1), "constants__is_byte_aligned__false");
static_assert(!is_byte_aligned(2), "constants__is_byte_aligned__false");
static_assert(!is_byte_aligned(-42), "constants__is_byte_aligned__false");
static_assert(!is_byte_aligned(0xff), "constants__is_byte_aligned__false");

static_assert(to_bits(0) == 0 * 8, "constants__to_bits__multiplied_by_eight");
static_assert(to_bits(1u) == 1u * 8u, "constants__to_bits__multiplied_by_eight");
static_assert(to_bits(-42) == -42 * 8, "constants__to_bits__multiplied_by_eight");
static_assert(to_bits(0xff) == 0xff * 8, "constants__to_bits__multiplied_by_eight");

static_assert(to_bytes(0) == 0 / 8, "constants__to_bytes__divided_by_eight");
static_assert(to_bytes(1u) == 1u / 8u, "constants__to_bytes__divided_by_eight");
static_assert(to_bytes(-42) == -42 / 8, "constants__to_bytes__divided_by_eight");
static_assert(to_bytes(0xff) == 0xff / 8, "constants__to_bytes__divided_by_eight");

static_assert(to_half(0) == 0 / 2, "constants__to_half__divided_by_two");
static_assert(to_half(1u) == 1u / 2u, "constants__to_half__divided_by_two");
static_assert(to_half(-42) == -42 / 2, "constants__to_half__divided_by_two");
static_assert(to_half(0xff) == 0xff / 2, "constants__to_half__divided_by_two");

static_assert(add1(0) == 0 + 1, "constants__add1__one_more");
static_assert(add1(1u) == 1u + 1u, "constants__add1__one_more");
static_assert(add1(-42) == -42 + 1, "constants__add1__one_more");
static_assert(add1(0xff) == 0xff + 1, "constants__add1__one_more");

static_assert(sub1(0xff) == 0xff - 1, "constants__sub1__one_less");
static_assert(sub1(1u) == 1u - 1u, "constants__sub1__one_less");
static_assert(sub1(-42) == -42 - 1, "constants__sub1__one_less");
static_assert(sub1(0xff) == 0xff - 1, "constants__sub1__one_less");
