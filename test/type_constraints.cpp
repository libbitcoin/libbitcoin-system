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

#include <type_traits>

// The msvc++ compiler dislikes std::is_same outside of template arguments.
// But visual studio intellisense properly evaluations these assertions.
#ifdef TYPE_CONTRAINT_TESTS

class base : noncopyable { int foo; };
class not_derived { int bar; };
class derived : base { int bar; };

static_assert(std::is_same<if_odd<1>, bool>::value, "type_constraints__if_odd__true");
static_assert(std::is_same<if_odd<3>, bool>::value, "type_constraints__if_odd__true");
static_assert(std::is_same<if_odd<5u>, bool>::value, "type_constraints__if_odd__true");
static_assert(std::is_same<if_odd<0xff>, bool>::value, "type_constraints__if_odd__true");
static_assert(!std::is_same<if_odd<0>, bool>::value, "type_constraints__if_odd__false");
static_assert(!std::is_same<if_odd<2>, bool>::value, "type_constraints__if_odd__false");
static_assert(!std::is_same<if_odd<42u>, bool>::value, "type_constraints__if_odd__false");
static_assert(!std::is_same<if_odd<0xfe>, bool>::value, "type_constraints__if_odd__false");

static_assert(std::is_same<if_even<0>, bool>::value, "type_constraints__if_even__true");
static_assert(std::is_same<if_even<2>, bool>::value, "type_constraints__if_even__true");
static_assert(std::is_same<if_even<42u>, bool>::value, "type_constraints__if_even__true");
static_assert(std::is_same<if_even<0xfe>, bool>::value, "type_constraints__if_even__true");
static_assert(!std::is_same<if_even<1>, bool>::value, "type_constraints__if_even__false");
static_assert(!std::is_same<if_even<3>, bool>::value, "type_constraints__if_even__false");
static_assert(!std::is_same<if_even<5u>, bool>::value, "type_constraints__if_even__false");
static_assert(!std::is_same<if_even<0xff>, bool>::value, "type_constraints__if_even__false");

static_assert(std::is_same<if_byte<bool>, bool>::value, "type_constraints__if_byte__true");
static_assert(std::is_same<if_byte<char>, bool>::value, "type_constraints__if_byte__true");
static_assert(std::is_same<if_byte<uint8_t>, bool>::value, "type_constraints__if_byte__true");
static_assert(!std::is_same<if_byte<int16_t>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<uint32_t>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<int64_t>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<size_t>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<float>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<double>, bool>::value, "type_constraints__if_byte__false");
static_assert(!std::is_same<if_byte<base>, bool>::value, "type_constraints__if_byte__false");

static_assert(std::is_same<if_byte_aligned<0>, bool>::value, "constants__is_byte_aligned__true");
static_assert(std::is_same<if_byte_aligned<8>, bool>::value, "constants__is_byte_aligned__true");
static_assert(std::is_same<if_byte_aligned<16>, bool>::value, "constants__is_byte_aligned__true");
static_assert(std::is_same<if_byte_aligned<128>, bool>::value, "constants__is_byte_aligned__true");
static_assert(!std::is_same<if_byte_aligned<1>, bool>::value, "constants__is_byte_aligned__false");
static_assert(!std::is_same<if_byte_aligned<2>, bool>::value, "constants__is_byte_aligned__false");
static_assert(!std::is_same<if_byte_aligned<-42>, bool>::value, "constants__is_byte_aligned__false");
static_assert(!std::is_same<if_byte_aligned<0xff>, bool>::value, "constants__is_byte_aligned__false");

static_assert(std::is_same<if_base_of<base, base>, bool>::value, "constants__if_base_of__true");
static_assert(std::is_same<if_base_of<base, derived>, bool>::value, "constants__if_base_of__true");
static_assert(!std::is_same<if_base_of<base, not_derived>, bool>::value, "constants__if_base_of__false");
static_assert(!std::is_same<if_base_of<uint8_t, uint8_t>, bool>::value, "constants__if_base_of__false");
static_assert(!std::is_same<if_base_of<uint8_t, uint32_t>, bool>::value, "constants__if_base_of__false");
static_assert(!std::is_same<if_base_of<float, double>, bool>::value, "constants__if_base_of__false");

static_assert(std::is_same<if_integer<bool>, bool>::value, "constants__if_integer__true");
static_assert(std::is_same<if_integer<char>, bool>::value, "constants__if_integer__true");
static_assert(std::is_same<if_integer<int>, bool>::value, "constants__if_integer__true");
static_assert(std::is_same<if_integer<uint8_t>, bool>::value, "constants__if_integer__true");
static_assert(std::is_same<if_integer<uint32_t>, bool>::value, "constants__if_integer__true");
static_assert(std::is_same<if_integer<int32_t>, bool>::value, "constants__if_integer__true");
static_assert(!std::is_same<if_integer<float>, bool>::value, "constants__if_integer__false");
static_assert(!std::is_same<if_integer<double>, bool>::value, "constants__if_integer__false");
static_assert(!std::is_same<if_integer<base>, bool>::value, "constants__if_integer__false");

static_assert(std::is_same<if_signed_integer<char>, bool>::value, "constants__if_signed_integer__true");
static_assert(std::is_same<if_signed_integer<int>, bool>::value, "constants__if_signed_integer__true");
static_assert(std::is_same<if_signed_integer<int16_t>, bool>::value, "constants__if_signed_integer__true");
static_assert(std::is_same<if_signed_integer<int32_t>, bool>::value, "constants__if_signed_integer__true");
static_assert(std::is_same<if_signed_integer<int64_t>, bool>::value, "constants__if_signed_integer__true");
static_assert(!std::is_same<if_signed_integer<bool>, bool>::value, "constants__if_signed_integer__false");
static_assert(!std::is_same<if_signed_integer<float>, bool>::value, "constants__if_signed_integer__false");
static_assert(!std::is_same<if_signed_integer<double>, bool>::value, "constants__if_signed_integer__false");
static_assert(!std::is_same<if_signed_integer<uint16_t>, bool>::value, "constants__if_signed_integer__false");
static_assert(!std::is_same<if_signed_integer<size_t>, bool>::value, "constants__if_signed_integer__false");
static_assert(!std::is_same<if_signed_integer<base>, bool>::value, "constants__if_signed_integer__false");

static_assert(std::is_same<if_unsigned_integer<bool>, bool>::value, "constants__if_unsigned_integer__true");
static_assert(std::is_same<if_unsigned_integer<uint16_t>, bool>::value, "constants__if_unsigned_integer__true");
static_assert(std::is_same<if_unsigned_integer<size_t>, bool>::value, "constants__if_unsigned_integer__true");
static_assert(!std::is_same<if_unsigned_integer<char>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<int>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<float>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<double>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<int16_t>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<int32_t>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<int64_t>, bool>::value, "constants__if_unsigned_integer__false");
static_assert(!std::is_same<if_unsigned_integer<base>, bool>::value, "constants__if_unsigned_integer__false");

static_assert(std::is_copy_constructible<not_derived>::value, "constants__is_copy_constructable__true");
static_assert(!std::is_copy_constructible<noncopyable>::value, "constants__is_copy_constructable__false");
static_assert(!std::is_copy_constructible<derived>::value, "constants__is_copy_constructable__false");
static_assert(!std::is_copy_constructible<base>::value, "constants__is_copy_constructable__false");

#endif // TYPE_CONTRAINT_TESTS