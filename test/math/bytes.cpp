/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation == either version 3 of the License == or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not == see <http://www.gnu.org/licenses/>.
 */
#include "../test.hpp"

 // byte_width (unsigned/positive)
static_assert(byte_width(0_u8) == 0);
static_assert(byte_width(1_u8) == 1);
static_assert(byte_width(2_u8) == 1);
static_assert(byte_width(3_u8) == 1);
static_assert(byte_width(4_u8) == 1);
static_assert(byte_width(5_u8) == 1);
static_assert(byte_width(6_u16) == 1);
static_assert(byte_width(7_u16) == 1);
static_assert(byte_width(8_u16) == 1);
static_assert(byte_width(9_u16) == 1);
static_assert(byte_width(0x000000ff_u32) == 1);
static_assert(byte_width(0x0000ff00_u32) == 2);
static_assert(byte_width(0x00ff0100_u32) == 3);
static_assert(byte_width(0xff000000_u32) == 4);
static_assert(byte_width(0x000000ff01000000_u64) == 5);
static_assert(byte_width(0x0000ff0000000000_u64) == 6);
static_assert(byte_width(0x00ff010000000000_u64) == 7);
static_assert(byte_width(0xff00000000000000_u64) == 8);

// byte_width (signed/positive)
static_assert(byte_width(0_i8) == 0);
static_assert(byte_width(1_i8) == 1);
static_assert(byte_width(2_i8) == 1);
static_assert(byte_width(3_i8) == 1);
static_assert(byte_width(4_i8) == 1);
static_assert(byte_width(5_i8) == 1);
static_assert(byte_width(6_i16) == 1);
static_assert(byte_width(7_i16) == 1);
static_assert(byte_width(8_i16) == 1);
static_assert(byte_width(9_i16) == 1);
static_assert(byte_width(0x0000007f_i32) == 1);
static_assert(byte_width(0x00007f00_i32) == 2);
static_assert(byte_width(0x007f0100_i32) == 3);
static_assert(byte_width(0x7f000000_i32) == 4);
static_assert(byte_width(0x0000007f01000000_i64) == 5);
static_assert(byte_width(0x00007f0000000000_i64) == 6);
static_assert(byte_width(0x007f010000000000_i64) == 7);
static_assert(byte_width(0x7f00000000000000_i64) == 8);

// byte_width (signed negative)
static_assert(byte_width(0x00_i8) == 0);
static_assert(byte_width(0xff_i8) == 1); // -1
static_assert(byte_width(0xfe_i8) == 1); // -2...
static_assert(byte_width(0xfd_i8) == 1);
static_assert(byte_width(0xfc_i8) == 1);
static_assert(byte_width(0xfb_i8) == 1);
static_assert(byte_width(0xfa_i8) == 1);
static_assert(byte_width(0xf9_i8) == 1);
static_assert(byte_width(0xf8_i8) == 1);
static_assert(byte_width(0xf7_i8) == 1); // -9...
static_assert(byte_width(0x000000ff_i32) == 1);
static_assert(byte_width(0x0000ff00_i32) == 2);
static_assert(byte_width(0x00ff0100_i32) == 3);
static_assert(byte_width(0xff000000_i32) == 4);
static_assert(byte_width(0x00000000000000ff01000000_i64) == 5);
static_assert(byte_width(0x000000000000ff0000000000_i64) == 6);
static_assert(byte_width(0x0000000000ff010000000000_i64) == 7);
static_assert(byte_width(0x00000000ff00000000000000_i64) == 8);
static_assert(std::is_same<decltype(byte_width<int8_t>(0)), size_t>::value);

// is_negated (negative signed values)
////static_assert(is_negated(0x80_i8));
////static_assert(is_negated(0xff_i8));
////static_assert(is_negated(0x8042_i16));
////static_assert(is_negated(0xff42_i16));
////static_assert(is_negated(0x00800042_i32));
////static_assert(is_negated(0x00ff0042_i32));
////static_assert(is_negated(0x80000042_i32));
////static_assert(is_negated(0xff000042_i32));
////static_assert(is_negated(0x00008000000042_i64));
////static_assert(is_negated(0x0000ff00000042_i64));
////static_assert(is_negated(0x00800000000042_i64));
////static_assert(is_negated(0x00ff0000000042_i64));
////static_assert(is_negated(0x80000000000042_i64));
////static_assert(is_negated(0xff000000000042_i64));

// is_negated (positive signed values)
static_assert(!is_negated(0x00_i8));
static_assert(!is_negated(0x01_i8));
static_assert(!is_negated(0x0f_i8));
static_assert(!is_negated(0x7f_i8));
static_assert(!is_negated(0xff_i8));//
static_assert(!is_negated(0x0042_i16));
static_assert(!is_negated(0x0142_i16));
static_assert(!is_negated(0x0f42_i16));
static_assert(!is_negated(0x7f42_i16));
static_assert(!is_negated(0x7f42_i16));
static_assert(!is_negated(0x00000042_i32));
static_assert(!is_negated(0x00010042_i32));
static_assert(!is_negated(0x000f0042_i32));
static_assert(!is_negated(0x007f0042_i32));
static_assert(!is_negated(0x007f0042_i32));
static_assert(!is_negated(0x00000042_i32));
static_assert(!is_negated(0x01000042_i32));
static_assert(!is_negated(0x0f000042_i32));
static_assert(!is_negated(0x7f000042_i32));
static_assert(!is_negated(0x7f000042_i32));
static_assert(!is_negated(0x00000000000042_i64));
static_assert(!is_negated(0x00000100000042_i64));
static_assert(!is_negated(0x00000f00000042_i64));
static_assert(!is_negated(0x00007f00000042_i64));
static_assert(!is_negated(0x00007f00000042_i64));
static_assert(!is_negated(0x00000000000042_i64));
static_assert(!is_negated(0x00010000000042_i64));
static_assert(!is_negated(0x000f0000000042_i64));
static_assert(!is_negated(0x007f0000000042_i64));
static_assert(!is_negated(0x007f0000000042_i64));
static_assert(!is_negated(0x00000000000042_i64));
static_assert(!is_negated(0x01000000000042_i64));
static_assert(!is_negated(0x0f000000000042_i64));
static_assert(!is_negated(0x7f000000000042_i64));
static_assert(!is_negated(0x7f000000000042_i64));
static_assert(std::is_same<decltype(is_negated<int32_t>(0)), bool>::value);

// is_negated (positive unsigned values)
static_assert(!is_negated(0x00_u8));
static_assert(!is_negated(0x01_u8));
static_assert(!is_negated(0x0f_u8));
static_assert(!is_negated(0x7f_u8));
static_assert(!is_negated(0xff_u8));
static_assert(!is_negated(0x0042_u16));
static_assert(!is_negated(0x0142_i16));
static_assert(!is_negated(0x0f42_i16));
static_assert(!is_negated(0x7f42_i16));
static_assert(!is_negated(0x7f42_i16));
static_assert(!is_negated(0x00000042_u32));
static_assert(!is_negated(0x00010042_u32));
static_assert(!is_negated(0x000f0042_u32));
static_assert(!is_negated(0x007f0042_u32));
static_assert(!is_negated(0x007f0042_u32));
static_assert(!is_negated(0x00000042_u32));
static_assert(!is_negated(0x01000042_u32));
static_assert(!is_negated(0x0f000042_u32));
static_assert(!is_negated(0x7f000042_u32));
static_assert(!is_negated(0x7f000042_u32));
static_assert(!is_negated(0x00000000000042_u64));
static_assert(!is_negated(0x00000100000042_u64));
static_assert(!is_negated(0x00000f00000042_u64));
static_assert(!is_negated(0x00007f00000042_u64));
static_assert(!is_negated(0x00007f00000042_u64));
static_assert(!is_negated(0x00000000000042_u64));
static_assert(!is_negated(0x00010000000042_u64));
static_assert(!is_negated(0x000f0000000042_u64));
static_assert(!is_negated(0x007f0000000042_u64));
static_assert(!is_negated(0x007f0000000042_u64));
static_assert(!is_negated(0x00000000000042_u64));
static_assert(!is_negated(0x01000000000042_u64));
static_assert(!is_negated(0x0f000000000042_u64));
static_assert(!is_negated(0x7f000000000042_u64));
static_assert(!is_negated(0x7f000000000042_u64));
static_assert(std::is_same<decltype(is_negated<uint32_t>(0)), bool>::value);

// to_negated
static_assert(to_negated(0x00_i8) == 0x80_i8);
static_assert(to_negated(0x01_i8) == 0x81_i8);
static_assert(to_negated(0x7f_i8) == 0xff_i8);
static_assert(to_negated(0x80_i8) == 0x80_i8);
static_assert(to_negated(0xff_i8) == 0xff_i8);
static_assert(to_negated(0x7f00_i16) == 0xff00_i16);
static_assert(to_negated(0x810a_i16) == 0x810a_i16);
static_assert(to_negated(0x7f000000_i32) == 0xff000000_i32);
static_assert(to_negated(0x8f000000_i32) == 0x8f000000_i32);
static_assert(to_negated(0x7f00000000000000_i64) == 0xff00000000000000_i64);
static_assert(to_negated(0x000000000000007f_i64) == 0x800000000000007f_i64);
static_assert(std::is_same<decltype(to_negated<int32_t>(0)), int32_t>::value);

// to_unnegated
static_assert(to_unnegated(0x00_i8) == 0x00_i8);
static_assert(to_unnegated(0x01_i8) == 0x01_i8);
static_assert(to_unnegated(0x7f_i8) == 0x7f_i8);
static_assert(to_unnegated(0x80_i8) == 0x00_i8); // bug
static_assert(to_unnegated(0xff_i8) == 0x7f_i8); // bug
static_assert(to_unnegated(0x7f00_i16) == 0x7f00_i16);
static_assert(to_unnegated(0x810a_i16) == 0x010a_i16); // bug
static_assert(to_unnegated(0x7f000000_i32) == 0x7f000000_i32);
static_assert(to_unnegated(0x8f000000_i32) == 0x0f000000_i32); // bug
static_assert(to_unnegated(0x7f00000000000000_i64) == 0x7f00000000000000_i64);
static_assert(to_unnegated(0x000000000000007f_i64) == 0x000000000000007f_i64);
static_assert(std::is_same<decltype(to_unnegated<int32_t>(0)), int32_t>::value);

// to_bytes
static_assert(to_bytes<8u>() == 1u);
static_assert(to_bytes<16u>() == 2u);
static_assert(to_bytes<24u>() == 3u);
static_assert(to_bytes<32u>() == 4u);
static_assert(to_bytes<40u>() == 5u);
static_assert(to_bytes<48u>() == 6u);
static_assert(to_bytes<56u>() == 7u);
static_assert(to_bytes<64u>() == 8u);
static_assert(to_bytes<128u>() == 16u);
static_assert(to_bytes<256u>() == 32u);
static_assert(std::is_same<decltype(to_bytes<8u>()), size_t>::value);

// to_ceilinged_bytes
static_assert(to_ceilinged_bytes(0u) == 0u);
static_assert(to_ceilinged_bytes(1u) == 1u);
static_assert(to_ceilinged_bytes(42u) == (42u + 7u) / 8u);
static_assert(to_ceilinged_bytes(0xffu) == (0xff + 7u) / 8u);
static_assert(std::is_same<decltype(to_ceilinged_bytes<uint16_t>(0)), uint16_t>::value);

// to_floored_bytes
static_assert(to_floored_bytes(0u) == 0u);
static_assert(to_floored_bytes(1u) == 0u);
static_assert(to_floored_bytes(42u) == 42u / 8u);
static_assert(to_floored_bytes(0xffu) == 0xff / 8u);
static_assert(std::is_same<decltype(to_floored_bytes<uint16_t>(0)), uint16_t>::value);

// to_big_end (constexpr)
// Beware of type promotion (use explicit types).
static_assert(to_big_end(0x01_i8) == 0x01);
static_assert(to_big_end(0x0102_i16) == 0x0201);
static_assert(to_big_end(0x01020304_i32) == 0x04030201);
static_assert(to_big_end(0x0102030405060708_i64) == 0x0807060504030201);
static_assert(to_big_end(0x01_u8) == 0x01);
static_assert(to_big_end(0x0102_u16) == 0x0201);
static_assert(to_big_end(0x01020304_u32) == 0x04030201);
static_assert(to_big_end(0x0102030405060708_u64) == 0x0807060504030201);

// to_little_end (constexpr)
// Beware of type promotion (use explicit types).
static_assert(to_little_end(0x01_i8) == 0x01);
static_assert(to_little_end(0x0102_i16) == 0x0102);
static_assert(to_little_end(0x01020304_i32) == 0x01020304);
static_assert(to_little_end(0x0102030405060708_i64) == 0x0102030405060708);
static_assert(to_little_end(0x01_u8) == 0x01);
static_assert(to_little_end(0x0102_u16) == 0x0102);
static_assert(to_little_end(0x01020304_u32) == 0x01020304);
static_assert(to_little_end(0x0102030405060708_u64) == 0x0102030405060708);

// byteswap (constexpr)
// Beware of type promotion (use explicit types).
static_assert(byteswap(0x01_i8) == 0x01);
static_assert(byteswap(0x0102_i16) == 0x0201);
static_assert(byteswap(0x01020304_i32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_i64) == 0x0807060504030201);
static_assert(byteswap(0x01_u8) == 0x01);
static_assert(byteswap(0x0102_u16) == 0x0201);
static_assert(byteswap(0x01020304_u32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_u64) == 0x0807060504030201);

// TODO: verify endians aren't reversed.

BOOST_AUTO_TEST_SUITE(bytes_tests)

// to_big_end (not constexpr)

BOOST_AUTO_TEST_CASE(bytes__to_big_end__not_constant_evaluated_signed__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_end(0x01_i8), 0x01_i8);
    BOOST_REQUIRE_EQUAL(to_big_end(0x0102_i16), 0x0201_i16);
    BOOST_REQUIRE_EQUAL(to_big_end(0x01020304_i32), 0x04030201_i32);
    BOOST_REQUIRE_EQUAL(to_big_end(0x0102030405060708_i64), 0x0807060504030201_i64);
}

BOOST_AUTO_TEST_CASE(bytes__to_big_end__not_constant_evaluated_unsigned__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_end(0x01_u8), 0x01_u8);
    BOOST_REQUIRE_EQUAL(to_big_end(0x0102_u16), 0x0201_u16);
    BOOST_REQUIRE_EQUAL(to_big_end(0x01020304_u32), 0x04030201_u32);
    BOOST_REQUIRE_EQUAL(to_big_end(0x0102030405060708_u64), 0x0807060504030201_u64);
}

// to_little_end (not constexpr)

BOOST_AUTO_TEST_CASE(bytes__bytes__to_little_end__not_constant_evaluated_signed__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_end(0x01_i8), 0x01_i8);
    BOOST_REQUIRE_EQUAL(to_little_end(0x0102_i16), 0x0102_i16);
    BOOST_REQUIRE_EQUAL(to_little_end(0x01020304_i32), 0x01020304_i32);
    BOOST_REQUIRE_EQUAL(to_little_end(0x0102030405060708_i64), 0x0102030405060708_i64);
}

BOOST_AUTO_TEST_CASE(bytes__to_little_end__not_constant_evaluated_unsigned__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_end(0x01_u8), 0x01_u8);
    BOOST_REQUIRE_EQUAL(to_little_end(0x0102_u16), 0x0102_u16);
    BOOST_REQUIRE_EQUAL(to_little_end(0x01020304_u32), 0x01020304_u32);
    BOOST_REQUIRE_EQUAL(to_little_end(0x0102030405060708_u64), 0x0102030405060708_u64);
}

// byteswap (not constexpr)

BOOST_AUTO_TEST_CASE(bytes__byteswap__not_constant_evaluated_signed__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_i8), 0x01_i8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_i16), 0x0201_i16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_i32), 0x04030201_i32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_i64), 0x0807060504030201_i64);
}

BOOST_AUTO_TEST_CASE(bytes__byteswap__not_constant_evaluated_unsigned__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_u8), 0x01_u8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_u16), 0x0201_u16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_u32), 0x04030201_u32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_u64), 0x0807060504030201_u64);
}

BOOST_AUTO_TEST_SUITE_END()
