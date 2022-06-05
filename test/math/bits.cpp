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
// ----------------------------------------------------------------------------

// TODO: test inline overloads.

// twos_complement
// similar to but different than absolute (~n+1)
static_assert(twos_complement(-4) == 4, "");
static_assert(twos_complement(-3) == 3, "");
static_assert(twos_complement(-2) == 2, "");
static_assert(twos_complement(-1) == 1, "");
static_assert(twos_complement(0) == 0, "");
static_assert(twos_complement(1) == -1, "");
static_assert(twos_complement(2) == -2, "");
static_assert(twos_complement(3) == -3, "");
static_assert(twos_complement(4) == -4, "");
static_assert(twos_complement<int8_t>(-4) == 4, "");
static_assert(twos_complement<int8_t>(-3) == 3, "");
static_assert(twos_complement<int8_t>(-2) == 2, "");
static_assert(twos_complement<int8_t>(-1) == 1, "");
static_assert(twos_complement<uint8_t>(0x00) == 0, "");
static_assert(twos_complement<uint8_t>(0xff) == 1, "");
static_assert(twos_complement<uint8_t>(0xfe) == 2, "");
static_assert(twos_complement<uint8_t>(0xfd) == 3, "");
static_assert(twos_complement<uint8_t>(0xfc) == 4, "");
static_assert(std::is_same<decltype(twos_complement<int32_t>(0)), int32_t>::value, "");

 // ones_complement
// inverts all bits (~n, !bool)
static_assert(bit_not(-4) == 3, "");
static_assert(bit_not(-3) == 2, "");
static_assert(bit_not(-2) == 1, "");
static_assert(bit_not(-1) == 0, "");
static_assert(bit_not(0) == -1, "");
static_assert(bit_not(1) == -2, "");
static_assert(bit_not(2) == -3, "");
static_assert(bit_not(3) == -4, "");
static_assert(bit_not(4) == -5, "");
static_assert(bit_not<int8_t>(-4) == 3, "");
static_assert(bit_not<int8_t>(-3) == 2, "");
static_assert(bit_not<int8_t>(-2) == 1, "");
static_assert(bit_not<int8_t>(-1) == 0, "");
static_assert(bit_not<uint8_t>(0x00) == 0xff, "");
static_assert(bit_not<uint8_t>(0xff) == 0, "");
static_assert(bit_not<uint8_t>(0xfe) == 1, "");
static_assert(bit_not<uint8_t>(0xfd) == 2, "");
static_assert(bit_not<uint8_t>(0xfc) == 3, "");
static_assert(std::is_same<decltype(bit_not<int32_t>(0)), int32_t>::value, "");

// bit_all
static_assert(bit_all<uint8_t>() == 0xff, "");
static_assert(bit_all<uint8_t>() == 0xff, "");
static_assert(bit_all<int8_t>() == -1, "");
static_assert(bit_all<int16_t>() == -1, "");
static_assert(bit_all<uint16_t>() == 0xffff, "");
static_assert(bit_all<int32_t>() == -1, "");
static_assert(bit_all<uint32_t>() == 0xffffffff, "");
static_assert(bit_all<int64_t>() == -1, "");
static_assert(bit_all<uint64_t>() == 0xffffffffffffffff, "");
static_assert(std::is_same<decltype(bit_all<int32_t>()), int32_t>::value, "");

// bit_hi
static_assert(bit_hi<char>() == narrow_cast<char>(0x80), "");
static_assert(bit_hi<int8_t>() == narrow_cast<int8_t>(0x80), "");
static_assert(bit_hi<uint8_t>() == uint8_t(0x80), "");
static_assert(bit_hi<int16_t>() == narrow_cast<int16_t>(0x8000), "");
static_assert(bit_hi<uint16_t>() == uint16_t(0x8000), "");
static_assert(bit_hi<int32_t>() == int32_t(0x80000000), "");
static_assert(bit_hi<uint32_t>() == uint32_t(0x80000000), "");
static_assert(bit_hi<int64_t>() == int64_t(0x8000000000000000), "");
static_assert(bit_hi<uint64_t>() == uint64_t(0x8000000000000000), "");
static_assert(std::is_same<decltype(bit_hi<int32_t>()), int32_t>::value, "");

// bit_lo
static_assert(bit_lo<char>() == char(1), "");
static_assert(bit_lo<int8_t>() == int8_t(1), "");
static_assert(bit_lo<uint8_t>() == uint8_t(1), "");
static_assert(bit_lo<int16_t>() == int16_t(1), "");
static_assert(bit_lo<uint16_t>() == uint16_t(1), "");
static_assert(bit_lo<int32_t>() == int32_t(1), "");
static_assert(bit_lo<uint32_t>() == uint32_t(1), "");
static_assert(bit_lo<int64_t>() == int64_t(1), "");
static_assert(bit_lo<uint64_t>() == uint64_t(1), "");
static_assert(std::is_same<decltype(bit_lo<int32_t>()), int32_t>::value, "");

// bit_left
static_assert(bit_left<uint8_t>(0) == 0x80, "");
static_assert(bit_left<uint8_t>(1) == 64, "");
static_assert(bit_left<uint8_t>(2) == 32, "");
static_assert(bit_left<uint8_t>(3) == 16, "");
static_assert(bit_left<uint8_t>(4) == 8, "");
static_assert(bit_left<uint8_t>(5) == 4, "");
static_assert(bit_left<uint8_t>(6) == 2, "");
static_assert(bit_left<uint8_t>(7) == 1, "");
static_assert(bit_left<uint16_t>(15) == 1, "");
static_assert(bit_left<uint32_t>(31) == 1, "");
static_assert(bit_left<uint64_t>(63) == 1, "");
static_assert(bit_left<int8_t>(0) == -128, "");
static_assert(bit_left<int8_t>(1) == 64, "");
static_assert(bit_left<int8_t>(2) == 32, "");
static_assert(bit_left<int8_t>(3) == 16, "");
static_assert(bit_left<int8_t>(4) == 8, "");
static_assert(bit_left<int8_t>(5) == 4, "");
static_assert(bit_left<int8_t>(6) == 2, "");
static_assert(bit_left<int8_t>(7) == 1, "");
static_assert(bit_left<int16_t>(15) == 1, "");
static_assert(bit_left<int32_t>(31) == 1, "");
static_assert(bit_left<int64_t>(63) == 1, "");
static_assert(bit_left<int16_t>(0) == -32768, "");
static_assert(bit_left<int16_t>(1) == 0x4000, "");
static_assert(bit_left<int16_t>(2) == 0x2000, "");
static_assert(bit_left<int16_t>(3) == 0x1000, "");
static_assert(bit_left<int16_t>(4) == 0x0800, "");
static_assert(bit_left<int16_t>(5) == 0x0400, "");
static_assert(bit_left<int16_t>(6) == 0x0200, "");
static_assert(bit_left<int16_t>(7) == 0x0100, "");
static_assert(bit_left<int16_t>(15) == 0x0001, "");
static_assert(bit_left<uint32_t>(31) == 0x00000001, "");
static_assert(bit_left<uint64_t>(63) == 0x0000000000000001, "");

static_assert(std::is_same<decltype(bit_left<int32_t>(0)), int32_t>::value, "");

// bit_right
static_assert(bit_right<uint8_t>(0) == 1, "");
static_assert(bit_right<uint8_t>(1) == 2, "");
static_assert(bit_right<uint8_t>(2) == 4, "");
static_assert(bit_right<uint8_t>(3) == 8, "");
static_assert(bit_right<uint8_t>(4) == 16, "");
static_assert(bit_right<uint8_t>(5) == 32, "");
static_assert(bit_right<uint8_t>(6) == 64, "");
static_assert(bit_right<uint8_t>(7) == 0x80, "");
static_assert(bit_right<int8_t>(0) == 1, "");
static_assert(bit_right<int8_t>(1) == 2, "");
static_assert(bit_right<int8_t>(2) == 4, "");
static_assert(bit_right<int8_t>(3) == 8, "");
static_assert(bit_right<int8_t>(4) == 16, "");
static_assert(bit_right<int8_t>(5) == 32, "");
static_assert(bit_right<int8_t>(6) == 64, "");
static_assert(bit_right<int8_t>(7) == -128, "");
static_assert(bit_right<int16_t>(0) == 1, "");
static_assert(bit_right<int16_t>(1) == 2, "");
static_assert(bit_right<int16_t>(2) == 4, "");
static_assert(bit_right<int16_t>(3) == 8, "");
static_assert(bit_right<int16_t>(4) == 16, "");
static_assert(bit_right<int16_t>(5) == 32, "");
static_assert(bit_right<int16_t>(6) == 64, "");
static_assert(bit_right<int16_t>(15) == -32768, "");
static_assert(bit_right<uint16_t>(15) == 0x8000, "");
static_assert(bit_right<uint32_t>(31) == 0x80000000, "");
static_assert(bit_right<uint64_t>(63) == 0x8000000000000000, "");
static_assert(std::is_same<decltype(bit_right<int32_t>(0)), int32_t>::value, "");

// get_left
static_assert(get_left<uint8_t>(0xff), "");
static_assert(get_left<uint8_t>(0xff, 1), "");
static_assert(get_left<uint8_t>(0xff, 2), "");
static_assert(get_left<uint8_t>(0xff, 3), "");
static_assert(get_left<uint8_t>(0xff, 4), "");
static_assert(get_left<uint8_t>(0xff, 5), "");
static_assert(get_left<uint8_t>(0xff, 6), "");
static_assert(get_left<uint8_t>(0xff, 7), "");
static_assert(get_left<uint16_t>(0xffff, 8), "");
static_assert(get_left<uint16_t>(0xffff, 15), "");
static_assert(get_left<uint32_t>(0xffffffff, 16), "");
static_assert(get_left<uint32_t>(0xffffffff, 31), "");
static_assert(get_left<uint64_t>(0xffffffffffffffff, 32), "");
static_assert(get_left<uint64_t>(0xffffffffffffffff, 63), "");
static_assert(get_left<uint8_t>(0x01, 7), "");
static_assert(get_left<uint8_t>(0x02, 6), "");
static_assert(get_left<uint8_t>(0x04, 5), "");
static_assert(get_left<uint8_t>(0x08, 4), "");
static_assert(get_left<uint8_t>(0x10, 3), "");
static_assert(get_left<uint8_t>(0x20, 2), "");
static_assert(get_left<uint8_t>(0x40, 1), "");
static_assert(get_left<uint8_t>(0x80, 0), "");
static_assert(get_left<uint16_t>(0x0100, 7), "");
static_assert(get_left<uint16_t>(0x8000, 0), "");
static_assert(get_left<uint16_t>(0x0004, 13), "");
static_assert(get_left<uint32_t>(0x00010000, 15), "");
static_assert(get_left<uint32_t>(0x80000000, 0), "");
static_assert(get_left<uint64_t>(0x0000000100000000, 31), "");
static_assert(get_left<uint64_t>(0x8000000000000000, 0), "");
static_assert(get_left<int8_t>(0x01, 7), "");
static_assert(get_left<int8_t>(0x02, 6), "");
static_assert(get_left<int8_t>(0x04, 5), "");
static_assert(get_left<int8_t>(0x08, 4), "");
static_assert(get_left<int8_t>(0x10, 3), "");
static_assert(get_left<int8_t>(0x20, 2), "");
static_assert(get_left<int8_t>(0x40, 1), "");
static_assert(get_left<int8_t>(-128, 0), "");
static_assert(get_left<int16_t>(0x0100, 7), "");
static_assert(get_left<int16_t>(-32768, 0), "");
static_assert(get_left<int16_t>(0x0004, 13), "");
static_assert(get_left<int32_t>(0x00010000, 15), "");
static_assert(get_left<int32_t>(0x80000000, 0), "");
static_assert(get_left<int64_t>(0x0000000100000000, 31), "");
static_assert(get_left<int64_t>(0x8000000000000000, 0), "");
static_assert(!get_left<uint16_t>(0x0001, 11), "");
static_assert(!get_left<uint16_t>(0x0002, 12), "");
static_assert(!get_left<uint16_t>(0x0008, 14), "");
static_assert(!get_left<uint16_t>(0x0010, 15), "");
static_assert(!get_left<uint32_t>(0x00000020, 16), "");
static_assert(!get_left<uint32_t>(0x00000040, 17), "");
static_assert(!get_left<uint32_t>(0x00000080, 18), "");
static_assert(!get_left<uint32_t>(0x00000100, 19), "");
static_assert(!get_left<uint32_t>(0x00008000, 20), "");
static_assert(!get_left<uint32_t>(0x00010000, 21), "");
static_assert(!get_left<uint32_t>(0x80000000, 22), "");
static_assert(!get_left<uint64_t>(0x0000000100000000, 23), "");
static_assert(!get_left<uint64_t>(0x8000000000000000, 24), "");
static_assert(std::is_same<decltype(get_left<int32_t>(0, 0)), bool>::value, "");

// get_right
static_assert(get_right<uint8_t>(0xff), "");
static_assert(get_right<uint8_t>(0xff, 1), "");
static_assert(get_right<uint8_t>(0xff, 2), "");
static_assert(get_right<uint8_t>(0xff, 3), "");
static_assert(get_right<uint8_t>(0xff, 4), "");
static_assert(get_right<uint8_t>(0xff, 5), "");
static_assert(get_right<uint8_t>(0xff, 6), "");
static_assert(get_right<uint8_t>(0xff, 7), "");
static_assert(get_right<uint16_t>(0xffff, 8), "");
static_assert(get_right<uint16_t>(0xffff, 15), "");
static_assert(get_right<uint32_t>(0xffffffff, 16), "");
static_assert(get_right<uint32_t>(0xffffffff, 31), "");
static_assert(get_right<uint64_t>(0xffffffffffffffff, 32), "");
static_assert(get_right<uint64_t>(0xffffffffffffffff, 63), "");
static_assert(get_right<uint8_t>(0x01, 0), "");
static_assert(get_right<uint8_t>(0x02, 1), "");
static_assert(get_right<uint8_t>(0x04, 2), "");
static_assert(get_right<uint8_t>(0x08, 3), "");
static_assert(get_right<uint8_t>(0x10, 4), "");
static_assert(get_right<uint8_t>(0x20, 5), "");
static_assert(get_right<uint8_t>(0x40, 6), "");
static_assert(get_right<uint8_t>(0x80, 7), "");
static_assert(get_right<uint16_t>(0x0100, 8), "");
static_assert(get_right<uint16_t>(0x8000, 15), "");
static_assert(get_right<uint32_t>(0x00010000, 16), "");
static_assert(get_right<uint32_t>(0x80000000, 31), "");
static_assert(get_right<uint64_t>(0x0000000100000000, 32), "");
static_assert(get_right<uint64_t>(0x8000000000000000, 63), "");
static_assert(get_right<int8_t>(0x01, 0), "");
static_assert(get_right<int8_t>(0x02, 1), "");
static_assert(get_right<int8_t>(0x04, 2), "");
static_assert(get_right<int8_t>(0x08, 3), "");
static_assert(get_right<int8_t>(0x10, 4), "");
static_assert(get_right<int8_t>(0x20, 5), "");
static_assert(get_right<int8_t>(0x40, 6), "");
static_assert(get_right<int8_t>(-128, 7), "");
static_assert(get_right<int16_t>(0x0100, 8), "");
static_assert(get_right<int16_t>(-32768, 15), "");
static_assert(get_right<int32_t>(0x00010000, 16), "");
static_assert(get_right<int32_t>(0x80000000, 31), "");
static_assert(get_right<int64_t>(0x0000000100000000, 32), "");
static_assert(get_right<int64_t>(0x8000000000000000, 63), "");
static_assert(!get_right<uint16_t>(0x0001, 11), "");
static_assert(!get_right<uint16_t>(0x0002, 12), "");
static_assert(!get_right<uint16_t>(0x0004, 13), "");
static_assert(!get_right<uint16_t>(0x0008, 14), "");
static_assert(!get_right<uint16_t>(0x0010, 15), "");
static_assert(!get_right<uint32_t>(0x00000020, 16), "");
static_assert(!get_right<uint32_t>(0x00000040, 17), "");
static_assert(!get_right<uint32_t>(0x00000080, 18), "");
static_assert(!get_right<uint32_t>(0x00000100, 19), "");
static_assert(!get_right<uint32_t>(0x00008000, 20), "");
static_assert(!get_right<uint32_t>(0x00010000, 21), "");
static_assert(!get_right<uint32_t>(0x80000000, 22), "");
static_assert(!get_right<uint64_t>(0x0000000100000000, 23), "");
static_assert(!get_right<uint64_t>(0x8000000000000000, 24), "");
static_assert(std::is_same<decltype(get_right<int32_t>(0, 0)), bool>::value, "");

////template <typename Type>
////constexpr bool set_left_test(Type value, size_t offset, bool state, Type expected)
////{
////    return (set_left_into(value, offset, state) == expected) && (value == expected);
////}

// set_left
static_assert(set_left<int8_t>(0x00, 0, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 1, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 2, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 3, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 4, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 5, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 6, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 7, false) == 0x00, "");
static_assert(set_left<int8_t>(0x00, 0, true) == -128, "");
static_assert(set_left<int8_t>(0x00, 1, true) == 0x40, "");
static_assert(set_left<int8_t>(0x00, 2, true) == 0x20, "");
static_assert(set_left<int8_t>(0x00, 3, true) == 0x10, "");
static_assert(set_left<int8_t>(0x00, 4, true) == 0x08, "");
static_assert(set_left<int8_t>(0x00, 5, true) == 0x04, "");
static_assert(set_left<int8_t>(0x00, 6, true) == 0x02, "");
static_assert(set_left<int8_t>(0x00, 7, true) == 0x01, "");
static_assert(set_left<uint8_t>(0x00, 0, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 1, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 2, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 3, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 4, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 5, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 6, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 7, false) == 0x00, "");
static_assert(set_left<uint8_t>(0x00, 0, true) == 0x80, "");
static_assert(set_left<uint8_t>(0x00, 1, true) == 0x40, "");
static_assert(set_left<uint8_t>(0x00, 2, true) == 0x20, "");
static_assert(set_left<uint8_t>(0x00, 3, true) == 0x10, "");
static_assert(set_left<uint8_t>(0x00, 4, true) == 0x08, "");
static_assert(set_left<uint8_t>(0x00, 5, true) == 0x04, "");
static_assert(set_left<uint8_t>(0x00, 6, true) == 0x02, "");
static_assert(set_left<uint8_t>(0x00, 7, true) == 0x01, "");
static_assert(set_left<uint8_t>(0xff, 0, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 1, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 2, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 3, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 4, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 5, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 6, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 7, true) == 0xff, "");
static_assert(set_left<uint8_t>(0xff, 0, false) == 0x7f, "");
static_assert(set_left<uint8_t>(0xff, 1, false) == 0xbf, "");
static_assert(set_left<uint8_t>(0xff, 2, false) == 0xdf, "");
static_assert(set_left<uint8_t>(0xff, 3, false) == 0xef, "");
static_assert(set_left<uint8_t>(0xff, 4, false) == 0xf7, "");
static_assert(set_left<uint8_t>(0xff, 5, false) == 0xfb, "");
static_assert(set_left<uint8_t>(0xff, 6, false) == 0xfd, "");
static_assert(set_left<uint8_t>(0xff, 7, false) == 0xfe, "");
static_assert(std::is_same<decltype(set_left<uint8_t>(0, 0, false)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool set_right_test(Type value, size_t offset, bool state, Type expected)
////{
////    return (set_right_into(value, offset, state) == expected) && (value == expected);
////}

// set_right
static_assert(set_right<int8_t>(0x00, 0, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 1, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 2, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 3, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 4, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 5, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 6, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 7, false) == 0x00, "");
static_assert(set_right<int8_t>(0x00, 0, true) == 0x01, "");
static_assert(set_right<int8_t>(0x00, 1, true) == 0x02, "");
static_assert(set_right<int8_t>(0x00, 2, true) == 0x04, "");
static_assert(set_right<int8_t>(0x00, 3, true) == 0x08, "");
static_assert(set_right<int8_t>(0x00, 4, true) == 0x10, "");
static_assert(set_right<int8_t>(0x00, 5, true) == 0x20, "");
static_assert(set_right<int8_t>(0x00, 6, true) == 0x40, "");
static_assert(set_right<int8_t>(0x00, 7, true) == -128, "");
static_assert(set_right<uint8_t>(0x00, 0, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 1, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 2, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 3, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 4, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 5, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 6, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 7, false) == 0x00, "");
static_assert(set_right<uint8_t>(0x00, 0, true) == 0x01, "");
static_assert(set_right<uint8_t>(0x00, 1, true) == 0x02, "");
static_assert(set_right<uint8_t>(0x00, 2, true) == 0x04, "");
static_assert(set_right<uint8_t>(0x00, 3, true) == 0x08, "");
static_assert(set_right<uint8_t>(0x00, 4, true) == 0x10, "");
static_assert(set_right<uint8_t>(0x00, 5, true) == 0x20, "");
static_assert(set_right<uint8_t>(0x00, 6, true) == 0x40, "");
static_assert(set_right<uint8_t>(0x00, 7, true) == 0x80, "");
static_assert(set_right<uint8_t>(0xff, 0, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 1, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 2, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 3, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 4, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 5, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 6, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 7, true) == 0xff, "");
static_assert(set_right<uint8_t>(0xff, 0, false) == 0xfe, "");
static_assert(set_right<uint8_t>(0xff, 1, false) == 0xfd, "");
static_assert(set_right<uint8_t>(0xff, 2, false) == 0xfb, "");
static_assert(set_right<uint8_t>(0xff, 3, false) == 0xf7, "");
static_assert(set_right<uint8_t>(0xff, 4, false) == 0xef, "");
static_assert(set_right<uint8_t>(0xff, 5, false) == 0xdf, "");
static_assert(set_right<uint8_t>(0xff, 6, false) == 0xbf, "");
static_assert(set_right<uint8_t>(0xff, 7, false) == 0x7f, "");
static_assert(std::is_same<decltype(set_right<uint8_t>(0, 0, false)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool unmask_left_test(Type target, size_t bits, Type expected)
////{
////    return (unmask_left_into(target, bits) == expected) && (target == expected);
////}

// unmask_left
static_assert(unmask_left<uint8_t>(0) == 0x00, "");
static_assert(unmask_left<uint8_t>(1) == 0x80, "");
static_assert(unmask_left<uint8_t>(2) == 0xc0, "");
static_assert(unmask_left<uint8_t>(3) == 0xe0, "");
static_assert(unmask_left<uint8_t>(4) == 0xf0, "");
static_assert(unmask_left<uint8_t>(5) == 0xf8, "");
static_assert(unmask_left<uint8_t>(6) == 0xfc, "");
static_assert(unmask_left<uint8_t>(7) == 0xfe, "");
static_assert(unmask_left<uint8_t>(8) == 0xff, "");
static_assert(unmask_left<uint16_t>(8) == 0xff00, "");
static_assert(unmask_left<uint32_t>(8) == 0xff000000, "");
static_assert(unmask_left<uint64_t>(8) == 0xff00000000000000, "");
static_assert(unmask_left<uint8_t>(0x00, 8) == 0xff, "");
static_assert(unmask_left<uint16_t>(0x0000, 8) == 0xff00, "");
static_assert(unmask_left<uint32_t>(0x00000000, 8) == 0xff000000, "");
static_assert(unmask_left<uint64_t>(0x0000000000000000, 8) == 0xff00000000000000, "");
static_assert(std::is_same<decltype(unmask_left<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool mask_left_test(Type target, size_t bits, Type expected)
////{
////    return (mask_left_into(target, bits) == expected) && (target == expected);
////}

// mask_left
static_assert(mask_left<uint8_t>(0) == 0xff, "");
static_assert(mask_left<uint8_t>(1) == 0x7f, "");
static_assert(mask_left<uint8_t>(2) == 0x3f, "");
static_assert(mask_left<uint8_t>(3) == 0x1f, "");
static_assert(mask_left<uint8_t>(4) == 0x0f, "");
static_assert(mask_left<uint8_t>(5) == 0x07, "");
static_assert(mask_left<uint8_t>(6) == 0x03, "");
static_assert(mask_left<uint8_t>(7) == 0x01, "");
static_assert(mask_left<uint8_t>(8) == 0x00, "");
static_assert(mask_left<uint16_t>(8) == 0x00ff, "");
static_assert(mask_left<uint32_t>(8) == 0x00ffffff, "");
static_assert(mask_left<uint64_t>(8) == 0x00ffffffffffffff, "");
static_assert(mask_left<uint8_t>(0xff, 8) == 0x00, "");
static_assert(mask_left<uint16_t>(0xffff, 8) == 0x00ff, "");
static_assert(mask_left<uint32_t>(0xffffffff, 8) == 0x00ffffff, "");
static_assert(mask_left<uint64_t>(0xffffffffffffffff, 8) == 0x00ffffffffffffff, "");
static_assert(std::is_same<decltype(mask_left<uint8_t>(0)), uint8_t>::value, "");

template <typename Type>
constexpr bool unmask_right_test(Type target, size_t bits, Type expected)
{
    return (unmask_right_into(target, bits) == expected) && (target == expected);
}

// unmask_right
static_assert(unmask_right<uint8_t>(0) == 0, "");
static_assert(unmask_right<uint8_t>(1) == 0x01, "");
static_assert(unmask_right<uint8_t>(2) == 0x03, "");
static_assert(unmask_right<uint8_t>(3) == 0x07, "");
static_assert(unmask_right<uint8_t>(4) == 0x0f, "");
static_assert(unmask_right<uint8_t>(5) == 0x1f, "");
static_assert(unmask_right<uint8_t>(6) == 0x3f, "");
static_assert(unmask_right<uint8_t>(7) == 0x7f, "");
static_assert(unmask_right<uint8_t>(8) == 0xff, "");
static_assert(unmask_right<uint16_t>(8) == 0x00ff, "");
static_assert(unmask_right<uint32_t>(8) == 0x000000ff, "");
static_assert(unmask_right<uint64_t>(8) == 0x00000000000000ff, "");
static_assert(unmask_right<uint8_t>(0x00, 8) == 0xff, "");
static_assert(unmask_right<uint16_t>(0x0000, 8) == 0x00ff, "");
static_assert(unmask_right<uint32_t>(0x00000000, 8) == 0x000000ff, "");
static_assert(unmask_right<uint64_t>(0x0000000000000000, 8) == 0x00000000000000ff, "");
static_assert(std::is_same<decltype(unmask_right<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool mask_right_test(Type target, size_t bits, Type expected)
////{
////    return (mask_right_into(target, bits) == expected) && (target == expected);
////}

// mask_right
static_assert(mask_right<uint8_t>(0) == 0xff, "");
static_assert(mask_right<uint8_t>(1) == 0xfe, "");
static_assert(mask_right<uint8_t>(2) == 0xfc, "");
static_assert(mask_right<uint8_t>(3) == 0xf8, "");
static_assert(mask_right<uint8_t>(4) == 0xf0, "");
static_assert(mask_right<uint8_t>(5) == 0xe0, "");
static_assert(mask_right<uint8_t>(6) == 0xc0, "");
static_assert(mask_right<uint8_t>(7) == 0x80, "");
static_assert(mask_right<uint8_t>(8) == 0x00, "");
static_assert(mask_right<uint16_t>(8) == 0xff00, "");
static_assert(mask_right<uint32_t>(8) == 0xffffff00, "");
static_assert(mask_right<uint64_t>(8) == 0xffffffffffffff00, "");
static_assert(mask_right<uint8_t>(0xff, 8) == 0x00, "");
static_assert(mask_right<uint16_t>(0xffff, 8) == 0xff00, "");
static_assert(mask_right<uint32_t>(0xffffffff, 8) == 0xffffff00, "");
static_assert(mask_right<uint64_t>(0xffffffffffffffff, 8) == 0xffffffffffffff00, "");
static_assert(std::is_same<decltype(mask_right<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool rotate_right_test(Type value, size_t bits, Type expected)
////{
////    return (rotate_right_into(value, bits) == expected) && (value == expected);
////}

// rotate_right
static_assert(rotate_right<uint8_t>(0x00, 0) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 1) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 2) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 3) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 5) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 6) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 7) == 0x00, "");
static_assert(rotate_right<uint8_t>(0x00, 8) == 0x00, "");
static_assert(rotate_right<uint8_t>(0xff, 0) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 1) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 2) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 3) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 5) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 6) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 7) == 0xff, "");
static_assert(rotate_right<uint8_t>(0xff, 8) == 0xff, "");
static_assert(rotate_right<uint8_t>(0x81, 0) == 0x81, "");
static_assert(rotate_right<uint8_t>(0x81, 1) == 0xc0, "");
static_assert(rotate_right<uint8_t>(0x81, 2) == 0x60, "");
static_assert(rotate_right<uint8_t>(0x81, 3) == 0x30, "");
static_assert(rotate_right<uint8_t>(0x81, 4) == 0x18, "");
static_assert(rotate_right<uint8_t>(0x81, 5) == 0x0c, "");
static_assert(rotate_right<uint8_t>(0x81, 6) == 0x06, "");
static_assert(rotate_right<uint8_t>(0x81, 7) == 0x03, "");
static_assert(rotate_right<uint8_t>(0x81, 8) == 0x81, "");
static_assert(std::is_same<decltype(rotate_right<uint8_t>(0, 0)), uint8_t>::value, "");

// en.cppreference.com/w/cpp/numeric/rotr
static_assert(rotate_right<uint8_t>(0x1d, 0) == 0x1d, "");
static_assert(rotate_right<uint8_t>(0x1d, 1) == 0x8e, "");
static_assert(rotate_right<uint8_t>(0x1d, 9) == 0x8e, "");

////template <typename Type>
////constexpr bool rotate_left_test(Type value, size_t bits, Type expected)
////{
////    return (rotate_left_into(value, bits) == expected) && (value == expected);
////}

// rotate_left
static_assert(rotate_left<uint8_t>(0x00, 0) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 1) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 2) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 3) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 5) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 6) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 7) == 0x00, "");
static_assert(rotate_left<uint8_t>(0x00, 8) == 0x00, "");
static_assert(rotate_left<uint8_t>(0xff, 0) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 1) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 2) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 3) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 5) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 6) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 7) == 0xff, "");
static_assert(rotate_left<uint8_t>(0xff, 8) == 0xff, "");
static_assert(rotate_left<uint8_t>(0x81, 0) == 0x81, "");
static_assert(rotate_left<uint8_t>(0x81, 1) == 0x03, "");
static_assert(rotate_left<uint8_t>(0x81, 2) == 0x06, "");
static_assert(rotate_left<uint8_t>(0x81, 3) == 0x0c, "");
static_assert(rotate_left<uint8_t>(0x81, 4) == 0x18, "");
static_assert(rotate_left<uint8_t>(0x81, 5) == 0x30, "");
static_assert(rotate_left<uint8_t>(0x81, 6) == 0x60, "");
static_assert(rotate_left<uint8_t>(0x81, 7) == 0xc0, "");
static_assert(rotate_left<uint8_t>(0x81, 8) == 0x81, "");
static_assert(std::is_same<decltype(rotate_left<uint8_t>(0, 0)), uint8_t>::value, "");

// en.cppreference.com/w/cpp/numeric/rotl
static_assert(rotate_left<uint8_t>(0x1d, 0) == 0x1d, "");
static_assert(rotate_left<uint8_t>(0x1d, 1) == 0x3a, "");
static_assert(rotate_left<uint8_t>(0x1d, 9) == 0x3a, "");

////template <typename Type>
////constexpr bool shift_left_test(Type value, size_t bits, bool overflow, Type expected)
////{
////    return (shift_left_into(value, bits) == expected) && (value == expected);
////}

// shift_left
static_assert(shift_left<uint8_t>(0, 0, true) == 0, "");
static_assert(shift_left<uint8_t>(0, 1, true) == 0, "");
static_assert(shift_left<uint8_t>(0, 8, true) == 0, "");
static_assert(shift_left<uint8_t>(1, 0, true) == 1, "");
static_assert(shift_left<uint8_t>(1, 1, true) == 2, "");
static_assert(shift_left<uint8_t>(1, 2, true) == 4, "");
static_assert(shift_left<uint8_t>(1, 3, true) == 8, "");
static_assert(shift_left<uint8_t>(1, 4, true) == 16, "");
static_assert(shift_left<uint8_t>(1, 5, true) == 32, "");
static_assert(shift_left<uint8_t>(1, 6, true) == 64, "");
static_assert(shift_left<uint8_t>(1, 7, true) == 128, "");
static_assert(shift_left<uint8_t>(1, 8, true) == 0, "");
static_assert(shift_left<uint8_t>(1, 9, true) == 0, "");
static_assert(shift_left<uint8_t>(0, 0, false) == 0, "");
static_assert(shift_left<uint8_t>(0, 1, false) == 0, "");
static_assert(shift_left<uint8_t>(0, 8, false) == 0, "");
static_assert(shift_left<uint8_t>(1, 0, false) == 1, "");
static_assert(shift_left<uint8_t>(1, 1, false) == 2, "");
static_assert(shift_left<uint8_t>(1, 2, false) == 4, "");
static_assert(shift_left<uint8_t>(1, 3, false) == 8, "");
static_assert(shift_left<uint8_t>(1, 4, false) == 16, "");
static_assert(shift_left<uint8_t>(1, 5, false) == 32, "");
static_assert(shift_left<uint8_t>(1, 6, false) == 64, "");
static_assert(shift_left<uint8_t>(1, 7, false) == 128, "");
static_assert(shift_left<uint8_t>(1, 8, false) == 1, "");
static_assert(shift_left<uint8_t>(1, 9, false) == 2, "");

static_assert(std::is_same<decltype(shift_left<uint8_t>(0, 0, true)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool shift_right_test(Type value, size_t bits, bool overflow, Type expected)
////{
////    return (shift_right_into(value, bits) == expected) && (value == expected);
////}

// shift_left
static_assert(shift_right<uint8_t>(0x00, 0, true) == 0, "");
static_assert(shift_right<uint8_t>(0x00, 1, true) == 0, "");
static_assert(shift_right<uint8_t>(0x00, 8, true) == 0, "");
static_assert(shift_right<uint8_t>(0x80, 0, true) == 0x80, "");
static_assert(shift_right<uint8_t>(0x80, 1, true) == 0x40, "");
static_assert(shift_right<uint8_t>(0x80, 2, true) == 0x20, "");
static_assert(shift_right<uint8_t>(0x80, 3, true) == 0x10, "");
static_assert(shift_right<uint8_t>(0x80, 4, true) == 0x08, "");
static_assert(shift_right<uint8_t>(0x80, 5, true) == 0x04, "");
static_assert(shift_right<uint8_t>(0x80, 6, true) == 0x02, "");
static_assert(shift_right<uint8_t>(0x80, 7, true) == 0x01, "");
static_assert(shift_right<uint8_t>(0x80, 8, true) == 0, "");
static_assert(shift_right<uint8_t>(0x80, 9, true) == 0, "");
static_assert(shift_right<uint8_t>(0x00, 0, false) == 0, "");
static_assert(shift_right<uint8_t>(0x00, 1, false) == 0, "");
static_assert(shift_right<uint8_t>(0x00, 8, false) == 0, "");
static_assert(shift_right<uint8_t>(0x80, 0, false) == 0x80, "");
static_assert(shift_right<uint8_t>(0x80, 1, false) == 0x40, "");
static_assert(shift_right<uint8_t>(0x80, 2, false) == 0x20, "");
static_assert(shift_right<uint8_t>(0x80, 3, false) == 0x10, "");
static_assert(shift_right<uint8_t>(0x80, 4, false) == 0x08, "");
static_assert(shift_right<uint8_t>(0x80, 5, false) == 0x04, "");
static_assert(shift_right<uint8_t>(0x80, 6, false) == 0x02, "");
static_assert(shift_right<uint8_t>(0x80, 7, false) == 0x01, "");
static_assert(shift_right<uint8_t>(0x80, 9, false) == 0x40, "");

static_assert(std::is_same<decltype(shift_left<uint8_t>(0, 0, true)), uint8_t>::value, "");

// Verify compiler "usual arithmetic conversion" expectations.
// ----------------------------------------------------------------------------

// Shift works like a unary operator (right operand is not incorporated).
// Order of operands does not affect (other) binary operators.

// https://en.cppreference.com/w/cpp/types/common_type
// (std::common_type<Left, Right>::type) == decltype(Left op Right)

// Compiler warns on bool safety for ~b, <<b, >>b, /b, %b, b/, b%.

// ones_complement
static_assert(std::is_same<decltype(~char{0}),                   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int8_t{0}),                 int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int16_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int32_t{0}),                int32_t>::value,  "unpromoted (>=32");
static_assert(std::is_same<decltype(~int64_t{0}),                int64_t>::value,  "unpromoted (>=32");
////static_assert(std::is_same<decltype(~bool{false}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint8_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint16_t{0}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint32_t{0}),               uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(~uint64_t{0}),               uint64_t>::value, "unpromoted (>=32");

// shift_left
static_assert(std::is_same<decltype(uint8_t{0}  << char{0}),     int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << uint8_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << uint16_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} << uint32_t{0}), int >::value,     "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} << uint64_t{0}), uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(uint64_t{0} << uint64_t{0}), uint64_t>::value, "unpromoted (>=32");
////static_assert(std::is_same<decltype(uint8_t{0}  << bool{false}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(bool{false} << uint32_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << int8_t{0}),   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << int16_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} << int32_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} << int64_t{0}),  uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(int64_t{0}  << uint32_t{0}), int64_t>::value,  "unpromoted (>=32");

// shift_right
static_assert(std::is_same<decltype(uint8_t{0}  >> char{0}),     int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> uint8_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> uint16_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} >> uint32_t{0}), int >::value,     "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} >> uint64_t{0}), uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(uint64_t{0} >> uint64_t{0}), uint64_t>::value, "unpromoted (>=32");
////static_assert(std::is_same<decltype(uint8_t{0}  >> bool{false}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(bool{false} >> uint32_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> int8_t{0}),   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> int16_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} >> int32_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} >> int64_t{0}),  uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(int64_t{0}  >> uint32_t{0}), int64_t>::value,  "unpromoted (>=32");

// bit_and
static_assert(std::is_same<decltype(uint8_t{0}  & char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} & uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} & uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} & uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  & bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} & int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} & int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  & uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// bit_or
static_assert(std::is_same<decltype(uint8_t{0}  | char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} | uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} | uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} | uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  | bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} | int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} | int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  | uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// bit_xor
static_assert(std::is_same<decltype(uint8_t{0}  ^ char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} ^ uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} ^ uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} ^ uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} ^ int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} ^ int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  ^ uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// add
static_assert(std::is_same<decltype(uint8_t{0}  + char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} + uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} + uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} + uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  + bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} + int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} + int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  + uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// subtract
static_assert(std::is_same<decltype(uint8_t{0}  - char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  - bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} - int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  - uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// subtract (reversed)
static_assert(std::is_same<decltype(char{0}     - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} - uint16_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint32_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(bool{false} - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  - uint16_t{0}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  - uint32_t{0}), int64_t>::value,  "unsigned (two >=32, s-u)");
static_assert(std::is_same<decltype(uint32_t{0} - int64_t{0}),  int64_t>::value,  "unsigned (two >=32, u-s)");

// multiply
static_assert(std::is_same<decltype(uint8_t{0}  * char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} * uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} * uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} * uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  * bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} * int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} * int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  * uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// divide
static_assert(std::is_same<decltype(uint8_t{0}  / char{1}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint16_t{1}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / uint32_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(uint8_t{0}  / bool{true}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / int8_t{1}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / int16_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / int32_t{1}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} / int64_t{1}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  / uint32_t{1}), int64_t>::value,  "unsigned (two >=32, u/s)");

// divide (reversed)
static_assert(std::is_same<decltype(char{0}     / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} / uint16_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint32_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(bool{false} / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  / uint16_t{1}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  / uint32_t{1}), int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(uint32_t{0} / int64_t{1}),  int64_t>::value,  "unsigned (two >=32, u/s)");

// modulo
static_assert(std::is_same<decltype(uint8_t{0}  % char{1}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint16_t{1}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % uint32_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(uint8_t{0}  % bool{true}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % int8_t{1}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % int16_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % int32_t{1}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} % int64_t{1}),  int64_t>::value,  "unsigned (two >=32, s%u)");
static_assert(std::is_same<decltype(int64_t{0}  % uint32_t{1}), int64_t>::value,  "unsigned (two >=32, u%s)");

// modulo (reversed)
static_assert(std::is_same<decltype(char{0}     % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} % uint16_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint32_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(bool{false} % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  % uint16_t{1}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  % uint32_t{1}), int64_t>::value,  "unsigned (two >=32, s%u)");
static_assert(std::is_same<decltype(uint32_t{0} % int64_t{1}),  int64_t>::value,  "unsigned (two >=32, u%s)");