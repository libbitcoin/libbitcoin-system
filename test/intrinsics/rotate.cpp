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

// rotl_nominal1
static_assert(rotl_nominal<uint8_t>(0x00, 0) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 1) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 2) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 3) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 4) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 5) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 6) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 7) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0x00, 8) == 0x00_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 0) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 1) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 2) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 3) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 4) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 5) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 6) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 7) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0xff, 8) == 0xff_u8);
static_assert(rotl_nominal<uint8_t>(0x81, 0) == 0b10000001_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 0) == 0b10000001_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 1) == 0b00000011_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 2) == 0b00000110_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 3) == 0b00001100_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 4) == 0b00011000_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 5) == 0b00110000_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 6) == 0b01100000_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 7) == 0b11000000_u8);
static_assert(rotl_nominal<uint8_t>(0b10000001, 8) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotl_nominal<uint8_t>(0, 0)), uint8_t>);

// rotr_nominal1
static_assert(rotr_nominal<uint8_t>(0x00, 0) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 1) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 2) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 3) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 4) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 5) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 6) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 7) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0x00, 8) == 0x00_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 0) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 1) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 2) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 3) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 4) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 5) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 6) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 7) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0xff, 8) == 0xff_u8);
static_assert(rotr_nominal<uint8_t>(0x81, 0) == 0b10000001_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 0) == 0b10000001_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 1) == 0b11000000_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 2) == 0b01100000_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 3) == 0b00110000_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 4) == 0b00011000_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 5) == 0b00001100_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 6) == 0b00000110_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 7) == 0b00000011_u8);
static_assert(rotr_nominal<uint8_t>(0b10000001, 8) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotr_nominal<uint8_t>(0, 0)), uint8_t>);

// rotl_nominal2
static_assert(rotl_nominal<0, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<1, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<2, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<3, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<4, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<5, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<6, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<7, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<8, uint8_t>(0x00) == 0x00_u8);
static_assert(rotl_nominal<0, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<1, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<2, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<3, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<4, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<5, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<6, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<7, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<8, uint8_t>(0xff) == 0xff_u8);
static_assert(rotl_nominal<0, uint8_t>(0x81) == 0b10000001_u8);
static_assert(rotl_nominal<0, uint8_t>(0b10000001) == 0b10000001_u8);
static_assert(rotl_nominal<1, uint8_t>(0b10000001) == 0b00000011_u8);
static_assert(rotl_nominal<2, uint8_t>(0b10000001) == 0b00000110_u8);
static_assert(rotl_nominal<3, uint8_t>(0b10000001) == 0b00001100_u8);
static_assert(rotl_nominal<4, uint8_t>(0b10000001) == 0b00011000_u8);
static_assert(rotl_nominal<5, uint8_t>(0b10000001) == 0b00110000_u8);
static_assert(rotl_nominal<6, uint8_t>(0b10000001) == 0b01100000_u8);
static_assert(rotl_nominal<7, uint8_t>(0b10000001) == 0b11000000_u8);
static_assert(rotl_nominal<8, uint8_t>(0b10000001) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotl_nominal<0, uint8_t>(0)), uint8_t>);

// rotr_nominal2
static_assert(rotr_nominal<0, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<1, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<2, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<3, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<4, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<5, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<6, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<7, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<8, uint8_t>(0x00) == 0x00_u8);
static_assert(rotr_nominal<0, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<1, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<2, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<3, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<4, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<5, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<6, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<7, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<8, uint8_t>(0xff) == 0xff_u8);
static_assert(rotr_nominal<0, uint8_t>(0x81) == 0b10000001_u8);
static_assert(rotr_nominal<0, uint8_t>(0b10000001) == 0b10000001_u8);
static_assert(rotr_nominal<1, uint8_t>(0b10000001) == 0b11000000_u8);
static_assert(rotr_nominal<2, uint8_t>(0b10000001) == 0b01100000_u8);
static_assert(rotr_nominal<3, uint8_t>(0b10000001) == 0b00110000_u8);
static_assert(rotr_nominal<4, uint8_t>(0b10000001) == 0b00011000_u8);
static_assert(rotr_nominal<5, uint8_t>(0b10000001) == 0b00001100_u8);
static_assert(rotr_nominal<6, uint8_t>(0b10000001) == 0b00000110_u8);
static_assert(rotr_nominal<7, uint8_t>(0b10000001) == 0b00000011_u8);
static_assert(rotr_nominal<8, uint8_t>(0b10000001) == 0b10000001_u8);
static_assert(is_same_type<decltype(rotr_nominal<0, uint8_t>(0)), uint8_t>);
