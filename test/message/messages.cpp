/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(messages_tests)

BOOST_AUTO_TEST_CASE(messages__variable_uint_size__one_byte__expected)
{
    static const uint64_t value = 1;
    BOOST_REQUIRE_EQUAL(message::variable_uint_size(value), 1u);
}

BOOST_AUTO_TEST_CASE(messages__variable_uint_size__two_byte__expected)
{
    static const uint64_t value = 0xfe;
    BOOST_REQUIRE_EQUAL(message::variable_uint_size(value), 3u);
}

BOOST_AUTO_TEST_CASE(messages__variable_uint_size__four_byte__expected)
{
    static const uint64_t value = 0x10000;
    BOOST_REQUIRE_EQUAL(message::variable_uint_size(value), 5u);
}

BOOST_AUTO_TEST_CASE(messages__variable_uint_size__eight_byte__expected)
{
    static const uint64_t value = 0x100000000;
    BOOST_REQUIRE_EQUAL(message::variable_uint_size(value), 9u);
}

BOOST_AUTO_TEST_SUITE_END()
