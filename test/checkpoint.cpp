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
#include <sstream>
#include <boost/program_options.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::config;
using namespace boost::program_options;

BOOST_AUTO_TEST_SUITE(checkpoint_tests)

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__hash)

BOOST_AUTO_TEST_CASE(checkpoint__hash__default__null_hash)
{
    const checkpoint check;
    BOOST_REQUIRE(check.hash() == null_hash);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(checkpoint__height)

BOOST_AUTO_TEST_CASE(checkpoint__height__default__zero)
{
    const checkpoint check;
    BOOST_REQUIRE_EQUAL(check.height(), 0u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
