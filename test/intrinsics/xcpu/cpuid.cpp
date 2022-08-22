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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(intrinsics_xcpu_cpuid_tests)

// cpuid includes a local get_bit<> utility.
static_assert(!get_bit<0>(0));
static_assert( get_bit<0>(1));
static_assert(!get_bit<0>(2));
static_assert(!get_bit<0>(0b00010000_u8));
static_assert( get_bit<1>(0b00010011_u8));
static_assert( get_bit<2>(0b00010100_u8));
static_assert( get_bit<3>(0b00011000_u8));
static_assert( get_bit<4>(0b00010000_u8));
static_assert( get_bit<5>(0b00110000_u8));
static_assert( get_bit<6>(0b01010000_u8));
static_assert( get_bit<7>(0b10010000_u8));
static_assert( get_bit<31>(0x80000000_u32));
static_assert( get_bit<15>(0x8000_u16));
static_assert( get_bit< 7>(0x80_u8));

// get_cpu/get_xcr tested in intrinsics.cpp.

BOOST_AUTO_TEST_SUITE_END()
