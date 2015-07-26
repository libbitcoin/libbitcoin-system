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

BOOST_AUTO_TEST_SUITE(endpoint_tests)

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(endpoint__construct)

BOOST_AUTO_TEST_CASE(endpoint__construct__empty__throws_invalid_option)
{
    // TODO: elimiante default constructor to prevent invalid instance.
    BOOST_REQUIRE_THROW(endpoint url(""), invalid_option_value);
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(endpoint__host)

BOOST_AUTO_TEST_CASE(endpoint__host__default__empty)
{
    endpoint url;
    BOOST_REQUIRE_EQUAL(url.host(), "");
}

BOOST_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //

BOOST_AUTO_TEST_SUITE(endpoint__port)

BOOST_AUTO_TEST_CASE(endpoint__port__default__zero)
{
    endpoint url;
    BOOST_REQUIRE_EQUAL(url.port(), 0u);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
