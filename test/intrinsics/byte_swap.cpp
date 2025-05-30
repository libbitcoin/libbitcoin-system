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

constexpr auto value16 = 0x0102_u16;
constexpr auto value32 = 0x01020304_u32;
constexpr auto value64 = 0x0102030405060708_u64;

constexpr auto swap16 = 0x0201_u16;
constexpr auto swap32 = 0x04030201_u32;
constexpr auto swap64 = 0x0807060504030201_u64;

static_assert(byte_swap8_nominal(0x01_u8) == 0x01_u8);
static_assert(byte_swap16_nominal(value16) == swap16);
static_assert(byte_swap32_nominal(value32) == swap32);
static_assert(byte_swap64_nominal(value64) == swap64);

// platform intrinsic, falls back to nominal
BOOST_AUTO_TEST_CASE(byte_swap__intrinsic__always__swapped)
{
    BOOST_REQUIRE_EQUAL(byte_swap16(value16), swap16);
    BOOST_REQUIRE_EQUAL(byte_swap32(value32), swap32);
    BOOST_REQUIRE_EQUAL(byte_swap64(value64), swap64);
}
