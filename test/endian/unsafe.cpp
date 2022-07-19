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

BOOST_AUTO_TEST_SUITE(endian_unsafe_tests)

constexpr uint32_t value32 = 0x01020304;
const data_chunk data_forward{ 0x01, 0x02, 0x03, 0x04 };
const data_chunk data_reverse{ 0x04, 0x03, 0x02, 0x01 };

////BOOST_AUTO_TEST_CASE(endian__unsafe_from_big_endian__always__expected)
////{
////    BOOST_REQUIRE_EQUAL(unsafe_from_big_endian<uint32_t>(data_forward.begin()), value32);
////}
////
////BOOST_AUTO_TEST_CASE(endian__unsafe_from_little_endian__always__expected)
////{
////    BOOST_REQUIRE_EQUAL(unsafe_from_little_endian<uint32_t>(data_reverse.begin()), value32);
////}

BOOST_AUTO_TEST_SUITE_END()
