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
#include <cstdint>
#include <iostream>
#include <limits>
////#include <sstream>
#include "number.hpp"

BOOST_AUTO_TEST_SUITE(number_tests)

using namespace bc::system::machine;

constexpr auto min64 = std::numeric_limits<int64_t>::min();
constexpr auto max64 = std::numeric_limits<int64_t>::max();
constexpr auto min32 = std::numeric_limits<int32_t>::min();
constexpr auto max32 = std::numeric_limits<int32_t>::max();

// construct

BOOST_AUTO_TEST_CASE(number__construct__default__zero)
{
    const number instance{};
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

BOOST_AUTO_TEST_CASE(number__construct__zero__zero)
{
    const number instance{ 0 };
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

// little-endian (with 0x80 negative sentinel)
static const data_chunk min64_data{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80 };
static const data_chunk max64_data{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f };

BOOST_AUTO_TEST_CASE(number__construct__minimum__minimums)
{
    const number instance{ min64 };
    BOOST_REQUIRE_EQUAL(instance.data(), min64_data );
    BOOST_REQUIRE_EQUAL(instance.int64(), min64);
}

BOOST_AUTO_TEST_CASE(number__construct__maximum__maximums)
{
    const number instance{ max64 };
    BOOST_REQUIRE_EQUAL(instance.data(), max64_data);
    BOOST_REQUIRE_EQUAL(instance.int64(), max64);
}

// set_data

BOOST_AUTO_TEST_CASE(number__set_data__empty_four__zero)
{
    number instance{};
    BOOST_REQUIRE(instance.set_data({}, 4));
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

BOOST_AUTO_TEST_CASE(number__set_data__one_four__expected)
{
    constexpr uint8_t value = 0x42;
    const data_chunk data{ value };
    number instance{};
    BOOST_REQUIRE(instance.set_data(data, 4));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int64(), value);
}

BOOST_AUTO_TEST_CASE(number__set_data__one_five__expected)
{
    constexpr uint8_t value = 0x42;
    const data_chunk data{ value };
    number instance{};
    BOOST_REQUIRE(instance.set_data(data, 5));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int64(), value);
}

BOOST_AUTO_TEST_CASE(number__set_data__four_four__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45 };
    number instance{};
    BOOST_REQUIRE(instance.set_data(data, 4));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int64(), from_little_endian<int64_t>(data));
}

BOOST_AUTO_TEST_CASE(number__set_data__five_five__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    number instance{};
    BOOST_REQUIRE(instance.set_data(data, 5));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int64(), from_little_endian<int64_t>(data));
 }

BOOST_AUTO_TEST_CASE(number__set_data__five_four__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    number instance{};
    BOOST_REQUIRE(!instance.set_data(data, 4));
}

BOOST_AUTO_TEST_CASE(number__set_data__six_five__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };
    number instance{};
    BOOST_REQUIRE(!instance.set_data(data, 5));
}

// data

BOOST_AUTO_TEST_CASE(number__data__x00__empty)
{
    const number instance{ 0x00 };
    const data_chunk expected{};
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__x01__0x01)
{
    const number instance{ 0x01 };
    const data_chunk expected{ 0x01 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__x7f__0x7f)
{
    const number instance{ 0x7f };
    const data_chunk expected{ 0x7f };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__xff__0x00ff)
{
    const number instance{ 0xff };
    const data_chunk expected{ 0xff, 0x00 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__negative_1__0x81)
{
    const number instance{ -1 };
    const data_chunk expected{ 0x81 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_SUITE_END()
