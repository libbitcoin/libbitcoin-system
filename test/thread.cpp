/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(thread)

BOOST_AUTO_TEST_SUITE(thread_tests)

BOOST_AUTO_TEST_CASE(thread__set_thread_priority__high__no_throw)
{
    BOOST_REQUIRE_NO_THROW(set_thread_priority(thread_priority::high));
}

BOOST_AUTO_TEST_CASE(thread__set_thread_priority__normal__no_throw)
{
    BOOST_REQUIRE_NO_THROW(set_thread_priority(thread_priority::normal));
}

BOOST_AUTO_TEST_CASE(thread__set_thread_priority__low__no_throw)
{
    BOOST_REQUIRE_NO_THROW(set_thread_priority(thread_priority::low));
}

BOOST_AUTO_TEST_CASE(thread__set_thread_priority__background__no_throw)
{
    BOOST_REQUIRE_NO_THROW(set_thread_priority(thread_priority::background));
}

BOOST_AUTO_TEST_CASE(thread__set_thread_priority__invalid__throws_invalid_argument)
{
    BOOST_REQUIRE_THROW(set_thread_priority(static_cast<thread_priority>(42)), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
