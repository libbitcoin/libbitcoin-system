/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BC_AUTO_TEST_SUITE(exclusive_slice_tests)

BC_AUTO_TEST_CASE(exclusive_slice__construct__empty__empty)
{
    BC_REQUIRE(exclusive_slice{}.empty());
}

BC_AUTO_TEST_CASE(exclusive_slice__construct__text_empty__empty)
{
    BC_REQUIRE(exclusive_slice("").empty());
}

// array precluded by design
////BC_AUTO_TEST_CASE(exclusive_slice__construct__array_empty__empty)
////{
////    BC_REQUIRE(exclusive_slice(data_array<0>{}).empty());
////}

// array precluded by design
////BC_AUTO_TEST_CASE(exclusive_slice__construct__vector_empty__empty)
////{
////    BC_REQUIRE(exclusive_slice(data_chunk{}).empty());
////}

BC_AUTO_TEST_CASE(exclusive_slice__construct__pointers_empty__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(exclusive_slice(value.data(), value.data()).empty());
}

BC_AUTO_TEST_CASE(exclusive_slice__construct__pointers_reversed__empty)
{
    const std::string value{ "foobar" };
    BC_REQUIRE(exclusive_slice(std::next(value.data(), 5u), value.data()).empty());
}

BC_AUTO_TEST_CASE(exclusive_slice__construct__iterators_empty__empty)
{
    const data_chunk value{};
    BC_REQUIRE(exclusive_slice(value.cbegin(), value.cend()).empty());
}

BC_AUTO_TEST_CASE(exclusive_slice__construct__string_empty__empty)
{
    const std::string value{};
    BC_REQUIRE(exclusive_slice(value).empty());
}

// TODO: review.
////BC_AUTO_TEST_CASE(exclusive_slice__initializer_list__ten__expected)
////{
////    const exclusive_slice slice{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
////    BC_REQUIRE(!slice.empty());
////    const auto expected = base16_chunk("00010203040506070809");
////    BC_REQUIRE_EQUAL(slice.to_chunk(), expected);
////}

BC_AUTO_TEST_SUITE_END()
