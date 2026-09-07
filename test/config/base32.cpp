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

BOOST_AUTO_TEST_SUITE(base32_tests)

using namespace bc::system::config;

BOOST_AUTO_TEST_CASE(base32__construct__string__expected)
{
    const base32 instance("MZXW6YTBOI======");
    BOOST_REQUIRE(data_chunk(instance) == to_chunk("foobar"));
}

BOOST_AUTO_TEST_CASE(base32__construct__unpadded_lower_case__expected)
{
    const base32 instance("mzxw6ytboi");
    BOOST_REQUIRE(data_chunk(instance) == to_chunk("foobar"));
}

BOOST_AUTO_TEST_CASE(base32__construct__invalid__throws)
{
    BOOST_REQUIRE_THROW(base32("MZXW6YTB0I"), istream_exception);
}

BOOST_AUTO_TEST_CASE(base32__stream__data__canonical)
{
    std::stringstream stream{};
    stream << base32(to_chunk("foobar"));
    BOOST_REQUIRE_EQUAL(stream.str(), "MZXW6YTBOI======");
}

BOOST_AUTO_TEST_SUITE_END()
