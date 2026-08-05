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
#include <sstream>

BOOST_AUTO_TEST_SUITE(byte_tests)

using namespace boost::program_options;

BOOST_AUTO_TEST_CASE(byte__construct__default__zero)
{
    BOOST_REQUIRE_EQUAL(config::byte(), 0x00_u8);
}

BOOST_AUTO_TEST_CASE(byte__construct__value__expected)
{
    BOOST_REQUIRE_EQUAL(config::byte(0x2a_u8), 0x2a_u8);
}

BOOST_AUTO_TEST_CASE(byte__construct__string_minimum__expected)
{
    BOOST_REQUIRE_EQUAL(config::byte("0"), 0x00_u8);
}

BOOST_AUTO_TEST_CASE(byte__construct__string_maximum__expected)
{
    BOOST_REQUIRE_EQUAL(config::byte("255"), 0xff_u8);
}

BOOST_AUTO_TEST_CASE(byte__construct__string_invalid_non_numeric__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(config::byte("a"), istream_exception);
}

BOOST_AUTO_TEST_CASE(byte__construct__string_invalid_negative__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(config::byte("-1"), istream_exception);
}

BOOST_AUTO_TEST_CASE(byte__construct__string_invalid_out_of_range__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(config::byte("256"), istream_exception);
}

BOOST_AUTO_TEST_CASE(byte__stream__round_trip__expected)
{
    config::byte instance{};
    std::istringstream("111") >> instance;
    BOOST_REQUIRE_EQUAL(instance, 111_u8);

    std::ostringstream output{};
    output << instance;
    BOOST_REQUIRE_EQUAL(output.str(), "111");
}

BOOST_AUTO_TEST_SUITE_END()
