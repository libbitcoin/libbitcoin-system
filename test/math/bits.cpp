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

// bit_width (unsigned/positive)
static_assert(bit_width(0u) == 0u, "");
static_assert(bit_width(1u) == 1u, "");
static_assert(bit_width(2u) == 2u, "");
static_assert(bit_width(3u) == 2u, "");
static_assert(bit_width(4u) == 3u, "");
static_assert(bit_width(5u) == 3u, "");
static_assert(bit_width(6u) == 3u, "");
static_assert(bit_width(7u) == 3u, "");
static_assert(bit_width(8u) == 4u, "");
static_assert(bit_width(9u) == 4u, "");
static_assert(bit_width(9u) == 4u, "");
static_assert(bit_width(0x80u) == 8u, "");
static_assert(bit_width(0x8000u) == 16u, "");
static_assert(bit_width(0x800100u) == 24u, "");
static_assert(bit_width(0x80000000u) == 32u, "");
static_assert(bit_width(0x8001000000u) == 40u, "");
static_assert(bit_width(0x800000000000u) == 48u, "");
static_assert(bit_width(0x80010000000000u) == 56u, "");
static_assert(bit_width(0x8000000000000000u) == 64u, "");

// bit_width (signed/positive)
static_assert(bit_width(0) == 0, "");
static_assert(bit_width(1) == 1, "");
static_assert(bit_width(2) == 2, "");
static_assert(bit_width(3) == 2, "");
static_assert(bit_width(4) == 3, "");
static_assert(bit_width(5) == 3, "");
static_assert(bit_width(6) == 3, "");
static_assert(bit_width(7) == 3, "");
static_assert(bit_width(8) == 4, "");
static_assert(bit_width(9) == 4, "");
static_assert(bit_width(9) == 4, "");
static_assert(bit_width(0x80) == 8, "");
static_assert(bit_width(0x8000) == 16, "");
static_assert(bit_width(0x800100) == 24, "");
static_assert(bit_width(0x80000000) == 32, "");
static_assert(bit_width(0x8001000000) == 40, "");
static_assert(bit_width(0x800000000000) == 48, "");
static_assert(bit_width(0x80010000000000) == 56, "");
static_assert(bit_width(0x8000000000000000) == 64, "");
static_assert(bit_width<int64_t>(0x80) == 8, "");
static_assert(bit_width<int64_t>(0x8000) == 16, "");
static_assert(bit_width<int64_t>(0x800100) == 24, "");
static_assert(bit_width<int64_t>(0x80000000) == 32, "");
static_assert(bit_width<int64_t>(0x8001000000) == 40, "");
static_assert(bit_width<int64_t>(0x800000000000) == 48, "");
static_assert(bit_width<int64_t>(0x80010000000000) == 56, "");
static_assert(bit_width<int64_t>(0x8000000000000000) == 64, "");

// bit_width (signed/negative)
// negative always sets the high order bit of the domain
static_assert(bit_width(-1) == 32u, "");
static_assert(bit_width(-2) == 32u, "");
static_assert(bit_width(-3) == 32u, "");
static_assert(bit_width(-4) == 32u, "");
static_assert(bit_width(-5) == 32u, "");
static_assert(bit_width(-6) == 32u, "");
static_assert(bit_width(-7) == 32u, "");
static_assert(bit_width(-8) == 32u, "");
static_assert(bit_width(-9) == 32u, "");
static_assert(bit_width(-0x80) == 32u, "");
static_assert(bit_width(-0x8000) == 32u, "");
static_assert(bit_width(-0x800100) == 32u, "");
////static_assert(bit_width(-0x80000000l) == 32u, "");
static_assert(bit_width(-0x8001000000) == 64u, "");
static_assert(bit_width(-0x800000000000) == 64u, "");
static_assert(bit_width(-0x80010000000000) == 64u, "");
////static_assert(bit_width(-0x8000000000000000ll) == 64u, "");
static_assert(bit_width<int64_t>(-0x80) == 64u, "");
static_assert(bit_width<int64_t>(-0x8000) == 64u, "");
static_assert(bit_width<int64_t>(-0x800100) == 64u, "");
////static_assert(bit_width<int64_t>(-0x80000000l) == 32u, "");
static_assert(bit_width<int64_t>(-0x8001000000) == 64u, "");
static_assert(bit_width<int64_t>(-0x800000000000) == 64u, "");
static_assert(bit_width<int64_t>(-0x80010000000000) == 64u, "");
////static_assert(bit_width<int64_t>(-0x8000000000000000ll) == 64u, "");

// byte_width (unsigned/positive)
static_assert(byte_width(0u) == 0u, "");
static_assert(byte_width(1u) == 1u, "");
static_assert(byte_width(2u) == 1u, "");
static_assert(byte_width(3u) == 1u, "");
static_assert(byte_width(4u) == 1u, "");
static_assert(byte_width(5u) == 1u, "");
static_assert(byte_width(6u) == 1u, "");
static_assert(byte_width(7u) == 1u, "");
static_assert(byte_width(8u) == 1u, "");
static_assert(byte_width(9u) == 1u, "");
static_assert(byte_width(9u) == 1u, "");
static_assert(byte_width(0x80u) == 1u, "");
static_assert(byte_width(0x8000u) == 2u, "");
static_assert(byte_width(0x800100u) == 3u, "");
static_assert(byte_width(0x80000000u) == 4u, "");
static_assert(byte_width(0x8001000000u) == 5u, "");
static_assert(byte_width(0x800000000000u) == 6u, "");
static_assert(byte_width(0x80010000000000u) == 7u, "");
static_assert(byte_width(0x8000000000000000u) == 8u, "");
static_assert(byte_width<int64_t>(0x80u) == 1u, "");
static_assert(byte_width<int64_t>(0x8000u) == 2u, "");
static_assert(byte_width<int64_t>(0x800100u) == 3u, "");
static_assert(byte_width<int64_t>(0x80000000u) == 4u, "");
static_assert(byte_width<int64_t>(0x8001000000u) == 5u, "");
static_assert(byte_width<int64_t>(0x800000000000u) == 6u, "");
static_assert(byte_width<int64_t>(0x80010000000000u) == 7u, "");
static_assert(byte_width<int64_t>(0x8000000000000000u) == 8u, "");

// byte_width (signed/positive)
static_assert(byte_width(0) == 0u, "");
static_assert(byte_width(1) == 1u, "");
static_assert(byte_width(2) == 1u, "");
static_assert(byte_width(3) == 1u, "");
static_assert(byte_width(4) == 1u, "");
static_assert(byte_width(5) == 1u, "");
static_assert(byte_width(6) == 1u, "");
static_assert(byte_width(7) == 1u, "");
static_assert(byte_width(8) == 1u, "");
static_assert(byte_width(9) == 1u, "");
static_assert(byte_width(9) == 1u, "");
static_assert(byte_width(0x80) == 1u, "");
static_assert(byte_width(0x8000) == 2u, "");
static_assert(byte_width(0x800100) == 3u, "");
static_assert(byte_width(0x80000000) == 4u, "");
static_assert(byte_width(0x8001000000) == 5u, "");
static_assert(byte_width(0x800000000000) == 6u, "");
static_assert(byte_width(0x80010000000000) == 7u, "");
static_assert(byte_width(0x8000000000000000) == 8u, "");
static_assert(byte_width<int64_t>(0x80) == 1u, "");
static_assert(byte_width<int64_t>(0x8000) == 2u, "");
static_assert(byte_width<int64_t>(0x800100) == 3u, "");
static_assert(byte_width<int64_t>(0x80000000) == 4u, "");
static_assert(byte_width<int64_t>(0x8001000000) == 5u, "");
static_assert(byte_width<int64_t>(0x800000000000) == 6u, "");
static_assert(byte_width<int64_t>(0x80010000000000) == 7u, "");
static_assert(byte_width<int64_t>(0x8000000000000000) == 8u, "");

// byte_width (signed/negative)
// negative always sets the high order bit of the domain
static_assert(byte_width(-1) == 4u, "");
static_assert(byte_width(-2) == 4u, "");
static_assert(byte_width(-3) == 4u, "");
static_assert(byte_width(-4) == 4u, "");
static_assert(byte_width(-5) == 4u, "");
static_assert(byte_width(-6) == 4u, "");
static_assert(byte_width(-7) == 4u, "");
static_assert(byte_width(-8) == 4u, "");
static_assert(byte_width(-9) == 4u, "");
static_assert(byte_width(-0x80) == 4u, "");
static_assert(byte_width(-0x8000) == 4u, "");
static_assert(byte_width(-0x800100) == 4u, "");
////static_assert(byte_width(-0x80000000l) == 4u, "");
static_assert(byte_width(-0x8001000000) == 8u, "");
static_assert(byte_width(-0x800000000000) == 8u, "");
static_assert(byte_width(-0x80010000000000) == 8u, "");
////static_assert(byte_width(-0x8000000000000000ll) == 8u, "");
static_assert(byte_width<int64_t>(-0x80) == 8u, "");
static_assert(byte_width<int64_t>(-0x8000) == 8u, "");
static_assert(byte_width<int64_t>(-0x800100) == 8u, "");
////static_assert(byte_width<int64_t>(-0x80000000l) == 4u, ""); // <= int64_t but divides evenly to 32.
static_assert(byte_width<int64_t>(-0x8001000000) == 8u, ""); 
static_assert(byte_width<int64_t>(-0x800000000000) == 8u, "");
static_assert(byte_width<int64_t>(-0x80010000000000) == 8u, "");
////static_assert(byte_width<int64_t>(-0x8000000000000000ll) == 8u, "");
static_assert(std::is_same<decltype(byte_width<int8_t>(0)), size_t>::value, "");

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
static_assert(twos_complement<uint8_t>(0x00) == 0u, "");
static_assert(twos_complement<uint8_t>(0xff) == 1u, "");
static_assert(twos_complement<uint8_t>(0xfe) == 2u, "");
static_assert(twos_complement<uint8_t>(0xfd) == 3u, "");
static_assert(twos_complement<uint8_t>(0xfc) == 4u, "");
static_assert(std::is_same<decltype(twos_complement<int8_t>(0)), int8_t>::value, "");

// ones_complement (NOT)
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
static_assert(bit_not<uint8_t>(0x00) == 0xffu, "");
static_assert(bit_not<uint8_t>(0xff) == 0u, "");
static_assert(bit_not<uint8_t>(0xfe) == 1u, "");
static_assert(bit_not<uint8_t>(0xfd) == 2u, "");
static_assert(bit_not<uint8_t>(0xfc) == 3u, "");
static_assert(std::is_same<decltype(bit_not<int8_t>(0)), int8_t>::value, "");

// AND
static_assert(bit_and(0, 0) == 0, "");
static_assert(bit_and(1, 0) == 0, "");
static_assert(bit_and(0, 1) == 0, "");
static_assert(bit_and(1, 1) == 1, "");
static_assert(std::is_same<decltype(bit_and<int8_t>(0, 0)), int8_t>::value, "");

// OR
static_assert(bit_or(0, 0) == 0, "");
static_assert(bit_or(1, 0) == 1, "");
static_assert(bit_or(0, 1) == 1, "");
static_assert(bit_or(1, 1) == 1, "");
static_assert(std::is_same<decltype(bit_or<int8_t>(0, 0)), int8_t>::value, "");

// XOR
static_assert(bit_xor(0, 0) == 0, "");
static_assert(bit_xor(1, 0) == 1, "");
static_assert(bit_xor(0, 1) == 1, "");
static_assert(bit_xor(1, 1) == 0, "");
static_assert(std::is_same<decltype(bit_xor<int8_t>(0, 0)), int8_t>::value, "");

// bit_all
static_assert(bit_all<uint8_t>() == 0xffu, "");
static_assert(bit_all<uint8_t>() == 0xffu, "");
static_assert(bit_all<int8_t>() == -1, "");
static_assert(bit_all<int16_t>() == -1, "");
static_assert(bit_all<uint16_t>() == 0xffffu, "");
static_assert(bit_all<int32_t>() == -1, "");
static_assert(bit_all<uint32_t>() == 0xffffffffu, "");
static_assert(bit_all<int64_t>() == -1, "");
static_assert(bit_all<uint64_t>() == 0xffffffffffffffffu, "");
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
static_assert(std::is_same<decltype(bit_hi<int8_t>()), int8_t>::value, "");

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
static_assert(std::is_same<decltype(bit_lo<int8_t>()), int8_t>::value, "");

// bit_left
static_assert(bit_left<uint8_t>(0) == 0x80u, "");
static_assert(bit_left<uint8_t>(1) == 64u, "");
static_assert(bit_left<uint8_t>(2) == 32u, "");
static_assert(bit_left<uint8_t>(3) == 16u, "");
static_assert(bit_left<uint8_t>(4) == 8u, "");
static_assert(bit_left<uint8_t>(5) == 4u, "");
static_assert(bit_left<uint8_t>(6) == 2u, "");
static_assert(bit_left<uint8_t>(7) == 1u, "");
static_assert(bit_left<uint16_t>(15) == 1u, "");
static_assert(bit_left<uint32_t>(31) == 1u, "");
static_assert(bit_left<uint64_t>(63) == 1u, "");
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
static_assert(bit_left<uint32_t>(31) == 0x00000001u, "");
static_assert(bit_left<uint64_t>(63) == 0x0000000000000001u, "");
static_assert(std::is_same<decltype(bit_left<int8_t>(0)), int8_t>::value, "");

// bit_right
static_assert(bit_right<uint8_t>(0) == 1u, "");
static_assert(bit_right<uint8_t>(1) == 2u, "");
static_assert(bit_right<uint8_t>(2) == 4u, "");
static_assert(bit_right<uint8_t>(3) == 8u, "");
static_assert(bit_right<uint8_t>(4) == 16u, "");
static_assert(bit_right<uint8_t>(5) == 32u, "");
static_assert(bit_right<uint8_t>(6) == 64u, "");
static_assert(bit_right<uint8_t>(7) == 0x80u, "");
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
static_assert(bit_right<uint16_t>(15) == 0x8000u, "");
static_assert(bit_right<uint32_t>(31) == 0x80000000u, "");
static_assert(bit_right<uint64_t>(63) == 0x8000000000000000u, "");
static_assert(std::is_same<decltype(bit_right<int8_t>(0)), int8_t>::value, "");

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
static_assert(std::is_same<decltype(get_left<int8_t>(0, 0)), bool>::value, "");

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
static_assert(std::is_same<decltype(get_right<int8_t>(0, 0)), bool>::value, "");

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
static_assert(set_left<uint8_t>(0x00, 0, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 1, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 2, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 3, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 4, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 5, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 6, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 7, false) == 0x00u, "");
static_assert(set_left<uint8_t>(0x00, 0, true) == 0x80u, "");
static_assert(set_left<uint8_t>(0x00, 1, true) == 0x40u, "");
static_assert(set_left<uint8_t>(0x00, 2, true) == 0x20u, "");
static_assert(set_left<uint8_t>(0x00, 3, true) == 0x10u, "");
static_assert(set_left<uint8_t>(0x00, 4, true) == 0x08u, "");
static_assert(set_left<uint8_t>(0x00, 5, true) == 0x04u, "");
static_assert(set_left<uint8_t>(0x00, 6, true) == 0x02u, "");
static_assert(set_left<uint8_t>(0x00, 7, true) == 0x01u, "");
static_assert(set_left<uint8_t>(0xff, 0, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 1, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 2, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 3, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 4, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 5, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 6, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 7, true) == 0xffu, "");
static_assert(set_left<uint8_t>(0xff, 0, false) == 0x7fu, "");
static_assert(set_left<uint8_t>(0xff, 1, false) == 0xbfu, "");
static_assert(set_left<uint8_t>(0xff, 2, false) == 0xdfu, "");
static_assert(set_left<uint8_t>(0xff, 3, false) == 0xefu, "");
static_assert(set_left<uint8_t>(0xff, 4, false) == 0xf7u, "");
static_assert(set_left<uint8_t>(0xff, 5, false) == 0xfbu, "");
static_assert(set_left<uint8_t>(0xff, 6, false) == 0xfdu, "");
static_assert(set_left<uint8_t>(0xff, 7, false) == 0xfeu, "");
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
static_assert(set_right<uint8_t>(0x00, 0, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 1, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 2, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 3, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 4, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 5, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 6, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 7, false) == 0x00u, "");
static_assert(set_right<uint8_t>(0x00, 0, true) == 0x01u, "");
static_assert(set_right<uint8_t>(0x00, 1, true) == 0x02u, "");
static_assert(set_right<uint8_t>(0x00, 2, true) == 0x04u, "");
static_assert(set_right<uint8_t>(0x00, 3, true) == 0x08u, "");
static_assert(set_right<uint8_t>(0x00, 4, true) == 0x10u, "");
static_assert(set_right<uint8_t>(0x00, 5, true) == 0x20u, "");
static_assert(set_right<uint8_t>(0x00, 6, true) == 0x40u, "");
static_assert(set_right<uint8_t>(0x00, 7, true) == 0x80u, "");
static_assert(set_right<uint8_t>(0xff, 0, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 1, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 2, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 3, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 4, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 5, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 6, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 7, true) == 0xffu, "");
static_assert(set_right<uint8_t>(0xff, 0, false) == 0xfeu, "");
static_assert(set_right<uint8_t>(0xff, 1, false) == 0xfdu, "");
static_assert(set_right<uint8_t>(0xff, 2, false) == 0xfbu, "");
static_assert(set_right<uint8_t>(0xff, 3, false) == 0xf7u, "");
static_assert(set_right<uint8_t>(0xff, 4, false) == 0xefu, "");
static_assert(set_right<uint8_t>(0xff, 5, false) == 0xdfu, "");
static_assert(set_right<uint8_t>(0xff, 6, false) == 0xbfu, "");
static_assert(set_right<uint8_t>(0xff, 7, false) == 0x7fu, "");
static_assert(std::is_same<decltype(set_right<uint8_t>(0, 0, false)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool mask_left_test(Type target, size_t bits, Type expected)
////{
////    return (mask_left_into(target, bits) == expected) && (target == expected);
////}

// mask_left
static_assert(mask_left<uint8_t>(0) == 0xffu, "");
static_assert(mask_left<uint8_t>(1) == 0x7fu, "");
static_assert(mask_left<uint8_t>(2) == 0x3fu, "");
static_assert(mask_left<uint8_t>(3) == 0x1fu, "");
static_assert(mask_left<uint8_t>(4) == 0x0fu, "");
static_assert(mask_left<uint8_t>(5) == 0x07u, "");
static_assert(mask_left<uint8_t>(6) == 0x03u, "");
static_assert(mask_left<uint8_t>(7) == 0x01u, "");
static_assert(mask_left<uint8_t>(8) == 0x00u, "");
static_assert(mask_left<uint16_t>(8) == 0x00ffu, "");
static_assert(mask_left<uint32_t>(8) == 0x00ffffffu, "");
static_assert(mask_left<uint64_t>(8) == 0x00ffffffffffffffu, "");
static_assert(mask_left<uint8_t>(0xff, 8) == 0x00u, "");
static_assert(mask_left<uint16_t>(0xffff, 8) == 0x00ffu, "");
static_assert(mask_left<uint32_t>(0xffffffff, 8) == 0x00ffffffu, "");
static_assert(mask_left<uint64_t>(0xffffffffffffffff, 8) == 0x00ffffffffffffffu, "");
static_assert(std::is_same<decltype(mask_left<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool mask_right_test(Type target, size_t bits, Type expected)
////{
////    return (mask_right_into(target, bits) == expected) && (target == expected);
////}

// mask_right
static_assert(mask_right<uint8_t>(0) == 0xffu, "");
static_assert(mask_right<uint8_t>(1) == 0xfeu, "");
static_assert(mask_right<uint8_t>(2) == 0xfcu, "");
static_assert(mask_right<uint8_t>(3) == 0xf8u, "");
static_assert(mask_right<uint8_t>(4) == 0xf0u, "");
static_assert(mask_right<uint8_t>(5) == 0xe0u, "");
static_assert(mask_right<uint8_t>(6) == 0xc0u, "");
static_assert(mask_right<uint8_t>(7) == 0x80u, "");
static_assert(mask_right<uint8_t>(8) == 0x00u, "");
static_assert(mask_right<uint16_t>(8) == 0xff00u, "");
static_assert(mask_right<uint32_t>(8) == 0xffffff00u, "");
static_assert(mask_right<uint64_t>(8) == 0xffffffffffffff00u, "");
static_assert(mask_right<uint8_t>(0xff, 8) == 0x00u, "");
static_assert(mask_right<uint16_t>(0xffff, 8) == 0xff00u, "");
static_assert(mask_right<uint32_t>(0xffffffff, 8) == 0xffffff00u, "");
static_assert(mask_right<uint64_t>(0xffffffffffffffff, 8) == 0xffffffffffffff00u, "");
static_assert(std::is_same<decltype(mask_right<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool unmask_left_test(Type target, size_t bits, Type expected)
////{
////    return (unmask_left_into(target, bits) == expected) && (target == expected);
////}

// unmask_left
static_assert(unmask_left<uint8_t>(0) == 0x00u, "");
static_assert(unmask_left<uint8_t>(1) == 0x80u, "");
static_assert(unmask_left<uint8_t>(2) == 0xc0u, "");
static_assert(unmask_left<uint8_t>(3) == 0xe0u, "");
static_assert(unmask_left<uint8_t>(4) == 0xf0u, "");
static_assert(unmask_left<uint8_t>(5) == 0xf8u, "");
static_assert(unmask_left<uint8_t>(6) == 0xfcu, "");
static_assert(unmask_left<uint8_t>(7) == 0xfeu, "");
static_assert(unmask_left<uint8_t>(8) == 0xffu, "");
static_assert(unmask_left<uint16_t>(8) == 0xff00u, "");
static_assert(unmask_left<uint32_t>(8) == 0xff000000u, "");
static_assert(unmask_left<uint64_t>(8) == 0xff00000000000000u, "");
static_assert(unmask_left<uint8_t>(0x00, 8) == 0xffu, "");
static_assert(unmask_left<uint16_t>(0x0000, 8) == 0xff00u, "");
static_assert(unmask_left<uint32_t>(0x00000000, 8) == 0xff000000u, "");
static_assert(unmask_left<uint64_t>(0x0000000000000000, 8) == 0xff00000000000000u, "");
static_assert(std::is_same<decltype(unmask_left<uint8_t>(0)), uint8_t>::value, "");

template <typename Type>
constexpr bool unmask_right_test(Type target, size_t bits, Type expected)
{
    return (unmask_right_into(target, bits) == expected) && (target == expected);
}

// unmask_right
static_assert(unmask_right<uint8_t>(0) == 0u, "");
static_assert(unmask_right<uint8_t>(1) == 0x01u, "");
static_assert(unmask_right<uint8_t>(2) == 0x03u, "");
static_assert(unmask_right<uint8_t>(3) == 0x07u, "");
static_assert(unmask_right<uint8_t>(4) == 0x0fu, "");
static_assert(unmask_right<uint8_t>(5) == 0x1fu, "");
static_assert(unmask_right<uint8_t>(6) == 0x3fu, "");
static_assert(unmask_right<uint8_t>(7) == 0x7fu, "");
static_assert(unmask_right<uint8_t>(8) == 0xffu, "");
static_assert(unmask_right<uint16_t>(8) == 0x00ffu, "");
static_assert(unmask_right<uint32_t>(8) == 0x000000ffu, "");
static_assert(unmask_right<uint64_t>(8) == 0x00000000000000ffu, "");
static_assert(unmask_right<uint8_t>(0x00, 8) == 0xffu, "");
static_assert(unmask_right<uint16_t>(0x0000, 8) == 0x00ffu, "");
static_assert(unmask_right<uint32_t>(0x00000000, 8) == 0x000000ffu, "");
static_assert(unmask_right<uint64_t>(0x0000000000000000, 8) == 0x00000000000000ffu, "");
static_assert(std::is_same<decltype(unmask_right<uint8_t>(0)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool shift_left_test(Type value, size_t bits, bool overflow, Type expected)
////{
////    return (shift_left_into(value, bits) == expected) && (value == expected);
////}

// shift_left
static_assert(shift_left<uint8_t>(0, 0, true) == 0u, "");
static_assert(shift_left<uint8_t>(0, 1, true) == 0u, "");
static_assert(shift_left<uint8_t>(0, 8, true) == 0u, "");
static_assert(shift_left<uint8_t>(1, 0, true) == 1u, "");
static_assert(shift_left<uint8_t>(1, 1, true) == 2u, "");
static_assert(shift_left<uint8_t>(1, 2, true) == 4u, "");
static_assert(shift_left<uint8_t>(1, 3, true) == 8u, "");
static_assert(shift_left<uint8_t>(1, 4, true) == 16u, "");
static_assert(shift_left<uint8_t>(1, 5, true) == 32u, "");
static_assert(shift_left<uint8_t>(1, 6, true) == 64u, "");
static_assert(shift_left<uint8_t>(1, 7, true) == 128u, "");
static_assert(shift_left<uint8_t>(1, 8, true) == 0u, "");
static_assert(shift_left<uint8_t>(1, 9, true) == 0u, "");
static_assert(shift_left<uint8_t>(0, 0, false) == 0u, "");
static_assert(shift_left<uint8_t>(0, 1, false) == 0u, "");
static_assert(shift_left<uint8_t>(0, 8, false) == 0u, "");
static_assert(shift_left<uint8_t>(1, 0, false) == 1u, "");
static_assert(shift_left<uint8_t>(1, 1, false) == 2u, "");
static_assert(shift_left<uint8_t>(1, 2, false) == 4u, "");
static_assert(shift_left<uint8_t>(1, 3, false) == 8u, "");
static_assert(shift_left<uint8_t>(1, 4, false) == 16u, "");
static_assert(shift_left<uint8_t>(1, 5, false) == 32u, "");
static_assert(shift_left<uint8_t>(1, 6, false) == 64u, "");
static_assert(shift_left<uint8_t>(1, 7, false) == 128u, "");
static_assert(shift_left<uint8_t>(1, 8, false) == 1u, "");
static_assert(shift_left<uint8_t>(1, 9, false) == 2u, "");
static_assert(std::is_same<decltype(shift_left<uint8_t>(0, 0, true)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool shift_right_test(Type value, size_t bits, bool overflow, Type expected)
////{
////    return (shift_right_into(value, bits) == expected) && (value == expected);
////}

// shift_left
static_assert(shift_right<uint8_t>(0x00, 0, true) == 0u, "");
static_assert(shift_right<uint8_t>(0x00, 1, true) == 0u, "");
static_assert(shift_right<uint8_t>(0x00, 8, true) == 0u, "");
static_assert(shift_right<uint8_t>(0x80, 0, true) == 0x80u, "");
static_assert(shift_right<uint8_t>(0x80, 1, true) == 0x40u, "");
static_assert(shift_right<uint8_t>(0x80, 2, true) == 0x20u, "");
static_assert(shift_right<uint8_t>(0x80, 3, true) == 0x10u, "");
static_assert(shift_right<uint8_t>(0x80, 4, true) == 0x08u, "");
static_assert(shift_right<uint8_t>(0x80, 5, true) == 0x04u, "");
static_assert(shift_right<uint8_t>(0x80, 6, true) == 0x02u, "");
static_assert(shift_right<uint8_t>(0x80, 7, true) == 0x01u, "");
static_assert(shift_right<uint8_t>(0x80, 8, true) == 0u, "");
static_assert(shift_right<uint8_t>(0x80, 9, true) == 0u, "");
static_assert(shift_right<uint8_t>(0x00, 0, false) == 0u, "");
static_assert(shift_right<uint8_t>(0x00, 1, false) == 0u, "");
static_assert(shift_right<uint8_t>(0x00, 8, false) == 0u, "");
static_assert(shift_right<uint8_t>(0x80, 0, false) == 0x80u, "");
static_assert(shift_right<uint8_t>(0x80, 1, false) == 0x40u, "");
static_assert(shift_right<uint8_t>(0x80, 2, false) == 0x20u, "");
static_assert(shift_right<uint8_t>(0x80, 3, false) == 0x10u, "");
static_assert(shift_right<uint8_t>(0x80, 4, false) == 0x08u, "");
static_assert(shift_right<uint8_t>(0x80, 5, false) == 0x04u, "");
static_assert(shift_right<uint8_t>(0x80, 6, false) == 0x02u, "");
static_assert(shift_right<uint8_t>(0x80, 7, false) == 0x01u, "");
static_assert(shift_right<uint8_t>(0x80, 9, false) == 0x40u, "");
static_assert(std::is_same<decltype(shift_left<uint8_t>(0, 0, true)), uint8_t>::value, "");

////template <typename Type>
////constexpr bool rotate_right_test(Type value, size_t bits, Type expected)
////{
////    return (rotate_right_into(value, bits) == expected) && (value == expected);
////}

// rotate_right
static_assert(rotate_right<uint8_t>(0x00, 0) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 1) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 2) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 3) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 5) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 6) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 7) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0x00, 8) == 0x00u, "");
static_assert(rotate_right<uint8_t>(0xff, 0) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 1) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 2) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 3) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 5) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 6) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 7) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0xff, 8) == 0xffu, "");
static_assert(rotate_right<uint8_t>(0x81, 0) == 0x81u, "");
static_assert(rotate_right<uint8_t>(0x81, 1) == 0xc0u, "");
static_assert(rotate_right<uint8_t>(0x81, 2) == 0x60u, "");
static_assert(rotate_right<uint8_t>(0x81, 3) == 0x30u, "");
static_assert(rotate_right<uint8_t>(0x81, 4) == 0x18u, "");
static_assert(rotate_right<uint8_t>(0x81, 5) == 0x0cu, "");
static_assert(rotate_right<uint8_t>(0x81, 6) == 0x06u, "");
static_assert(rotate_right<uint8_t>(0x81, 7) == 0x03u, "");
static_assert(rotate_right<uint8_t>(0x81, 8) == 0x81u, "");
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
static_assert(rotate_left<uint8_t>(0x00, 0) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 1) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 2) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 3) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 5) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 6) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 7) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0x00, 8) == 0x00u, "");
static_assert(rotate_left<uint8_t>(0xff, 0) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 1) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 2) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 3) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 5) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 6) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 7) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0xff, 8) == 0xffu, "");
static_assert(rotate_left<uint8_t>(0x81, 0) == 0x81u, "");
static_assert(rotate_left<uint8_t>(0x81, 1) == 0x03u, "");
static_assert(rotate_left<uint8_t>(0x81, 2) == 0x06u, "");
static_assert(rotate_left<uint8_t>(0x81, 3) == 0x0cu, "");
static_assert(rotate_left<uint8_t>(0x81, 4) == 0x18u, "");
static_assert(rotate_left<uint8_t>(0x81, 5) == 0x30u, "");
static_assert(rotate_left<uint8_t>(0x81, 6) == 0x60u, "");
static_assert(rotate_left<uint8_t>(0x81, 7) == 0xc0u, "");
static_assert(rotate_left<uint8_t>(0x81, 8) == 0x81u, "");
static_assert(std::is_same<decltype(rotate_left<uint8_t>(0, 0)), uint8_t>::value, "");

// en.cppreference.com/w/cpp/numeric/rotl
static_assert(rotate_left<uint8_t>(0x1d, 0) == 0x1d, "");
static_assert(rotate_left<uint8_t>(0x1d, 1) == 0x3a, "");
static_assert(rotate_left<uint8_t>(0x1d, 9) == 0x3a, "");

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
