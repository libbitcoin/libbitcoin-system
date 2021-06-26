/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "../../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(stream_tests)

// TODO: test all methods against std::istringstream
// TODO: test against libbitcoin sources.

#define BIT_READER_IS_EXHAUSTED

#ifdef BIT_READER_IS_EXHAUSTED

BOOST_AUTO_TEST_CASE(bit_reader__is_exhausted__empty__true)
{
    std::istringstream stream;
    read::bits::istream reader(stream);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(bit_reader__is_exhausted__not_exhausted__false)
{
    std::istringstream stream{ "a" };
    read::bits::istream reader(stream);
    BOOST_REQUIRE(!reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(bit_reader__is_exhausted__exhausted__true)
{
    std::istringstream stream{ "a" };
    read::bits::istream reader(stream);
    BOOST_REQUIRE_EQUAL(reader.read_byte(), 'a');
    BOOST_REQUIRE(reader.is_exhausted());
}

#endif // BIT_READER_IS_EXHAUSTED

BOOST_AUTO_TEST_SUITE_END()
