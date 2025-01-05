/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(rotate_tests)

// TODO: 16/32/64 bit tests.

// rotr
static_assert(rotr<0>(0x00_u8) == 0x00_u8);
static_assert(rotr<1>(0x00_u8) == 0x00_u8);
static_assert(rotr<2>(0x00_u8) == 0x00_u8);
static_assert(rotr<3>(0x00_u8) == 0x00_u8);
static_assert(rotr<4>(0x00_u8) == 0x00_u8);
static_assert(rotr<5>(0x00_u8) == 0x00_u8);
static_assert(rotr<6>(0x00_u8) == 0x00_u8);
static_assert(rotr<7>(0x00_u8) == 0x00_u8);
static_assert(rotr<8>(0x00_u8) == 0x00_u8);
static_assert(rotr<0>(0xff_u8) == 0xff_u8);
static_assert(rotr<1>(0xff_u8) == 0xff_u8);
static_assert(rotr<2>(0xff_u8) == 0xff_u8);
static_assert(rotr<3>(0xff_u8) == 0xff_u8);
static_assert(rotr<4>(0xff_u8) == 0xff_u8);
static_assert(rotr<5>(0xff_u8) == 0xff_u8);
static_assert(rotr<6>(0xff_u8) == 0xff_u8);
static_assert(rotr<7>(0xff_u8) == 0xff_u8);
static_assert(rotr<8>(0xff_u8) == 0xff_u8);
static_assert(rotr<0>(0x81_u8) == 0b10000001_u8);
////static_assert(rotr(0b10000001_u8, 0u) == 0b10000001_u8);
static_assert(rotr(0b10000001_u8, 1u) == 0b11000000_u8);
static_assert(rotr(0b10000001_u8, 2u) == 0b01100000_u8);
static_assert(rotr(0b10000001_u8, 3u) == 0b00110000_u8);
static_assert(rotr(0b10000001_u8, 4u) == 0b00011000_u8);
static_assert(rotr(0b10000001_u8, 5u) == 0b00001100_u8);
static_assert(rotr(0b10000001_u8, 6u) == 0b00000110_u8);
static_assert(rotr(0b10000001_u8, 7u) == 0b00000011_u8);
////static_assert(rotr(0b10000001_u8, 8u) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotr(0_u8, 1u)), uint8_t>);
static_assert(is_same_type<decltype(rotr<1>(0_u8)), uint8_t>);

// en.cppreference.com/w/cpp/numeric/rotr
static_assert(rotr<0>(0x1d_u8) == 0x1d_u8);
static_assert(rotr<1>(0x1d_u8) == 0x8e_u8);
static_assert(rotr<9>(0x1d_u8) == 0x8e_u8);

BOOST_AUTO_TEST_CASE(rotate__rotr__always__expected)
{
    BOOST_REQUIRE_EQUAL(rotr<0>(0x1d_u8), 0x1d_u8);
    BOOST_REQUIRE_EQUAL(rotr<1>(0x1d_u8), 0x8e_u8);
    BOOST_REQUIRE_EQUAL(rotr<9>(0x1d_u8), 0x8e_u8);
}

// rotl
static_assert(rotl<0>(0x00_u8) == 0x00_u8);
static_assert(rotl<1>(0x00_u8) == 0x00_u8);
static_assert(rotl<2>(0x00_u8) == 0x00_u8);
static_assert(rotl<3>(0x00_u8) == 0x00_u8);
static_assert(rotl<4>(0x00_u8) == 0x00_u8);
static_assert(rotl<5>(0x00_u8) == 0x00_u8);
static_assert(rotl<6>(0x00_u8) == 0x00_u8);
static_assert(rotl<7>(0x00_u8) == 0x00_u8);
static_assert(rotl<8>(0x00_u8) == 0x00_u8);
////static_assert(rotl(0xff_u8, 0u) == 0xff_u8);
static_assert(rotl(0xff_u8, 1u) == 0xff_u8);
static_assert(rotl(0xff_u8, 2u) == 0xff_u8);
static_assert(rotl(0xff_u8, 3u) == 0xff_u8);
static_assert(rotl(0xff_u8, 5u) == 0xff_u8);
static_assert(rotl(0xff_u8, 6u) == 0xff_u8);
static_assert(rotl(0xff_u8, 7u) == 0xff_u8);
////static_assert(rotl(0xff_u8, 8u) == 0xff_u8);
////static_assert(rotl(0x81_u8, 0u) == 0b10000001_u8);
////static_assert(rotl(0b10000001_u8, 0u) == 0b10000001_u8);
static_assert(rotl(0b10000001_u8, 1u) == 0b00000011_u8);
static_assert(rotl(0b10000001_u8, 2u) == 0b00000110_u8);
static_assert(rotl(0b10000001_u8, 3u) == 0b00001100_u8);
static_assert(rotl(0b10000001_u8, 4u) == 0b00011000_u8);
static_assert(rotl(0b10000001_u8, 5u) == 0b00110000_u8);
static_assert(rotl(0b10000001_u8, 6u) == 0b01100000_u8);
static_assert(rotl(0b10000001_u8, 7u) == 0b11000000_u8);
////static_assert(rotl(0b10000001_u8, 8u) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotl(0_u8, 1u)), uint8_t>);
static_assert(is_same_type<decltype(rotl<1>(0_u8)), uint8_t>);

// en.cppreference.com/w/cpp/numeric/rotl
static_assert(rotl<0>(0x1d_u8) == 0x1d_u8);
static_assert(rotl<1>(0x1d_u8) == 0x3a_u8);
static_assert(rotl<9>(0x1d_u8) == 0x3a_u8);

BOOST_AUTO_TEST_CASE(rotate__rotl__always__expected)
{
    BOOST_REQUIRE_EQUAL(rotl<0>(0x1d_u8), 0x1d_u8);
    BOOST_REQUIRE_EQUAL(rotl<1>(0x1d_u8), 0x3a_u8);
    BOOST_REQUIRE_EQUAL(rotl<9>(0x1d_u8), 0x3a_u8);
}

BOOST_AUTO_TEST_SUITE_END()
