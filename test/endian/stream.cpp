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

BOOST_AUTO_TEST_SUITE(endian_stream_tests)

const auto alpha_forward = "BITCOIN!";
const auto alpha_reverse = "!NIOCTIB";
constexpr uint64_t value_alpha64 = 0x424954434f494e21;

BOOST_AUTO_TEST_CASE(endian__from_big_endian__stream_to_integer__expected)
{
    std::stringstream stream{ alpha_forward };
    BOOST_REQUIRE_EQUAL(from_big_endian<uint64_t>(stream), value_alpha64);
}

BOOST_AUTO_TEST_CASE(endian__from_little_endian__stream_to_integer__expected)
{
    std::stringstream stream{ alpha_reverse };
    BOOST_REQUIRE_EQUAL(from_little_endian<uint64_t>(stream), value_alpha64);
}

BOOST_AUTO_TEST_CASE(endian__to_big_endian__integer_to_stream__expected)
{
    std::stringstream stream;
    to_big_endian(stream, value_alpha64);
    BOOST_REQUIRE_EQUAL(stream.str(), alpha_forward);
}

BOOST_AUTO_TEST_CASE(endian__to_little_endian__integer_to_stream__expected)
{
    std::stringstream stream;
    to_little_endian(stream, value_alpha64);
    BOOST_REQUIRE_EQUAL(stream.str(), alpha_reverse);
}

BOOST_AUTO_TEST_SUITE_END()
