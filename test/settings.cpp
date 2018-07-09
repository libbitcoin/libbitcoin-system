/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(settings_tests)

// constructors
//-----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(settings__construct__default_context__expected)
{
    settings configuration;
    BOOST_REQUIRE_EQUAL(configuration.retargeting_factor, 4);
    BOOST_REQUIRE_EQUAL(configuration.target_spacing_seconds, 600);
    BOOST_REQUIRE_EQUAL(configuration.easy_spacing_seconds, 1200);
    BOOST_REQUIRE_EQUAL(configuration.timestamp_future_seconds, 7200);
    BOOST_REQUIRE_EQUAL(configuration.target_timespan_seconds, 1209600);
    BOOST_REQUIRE_EQUAL(configuration.retarget_proof_of_work_limit, 0x1d00ffff);
    BOOST_REQUIRE_EQUAL(configuration.no_retarget_proof_of_work_limit, 0x207fffff);
    BOOST_REQUIRE_EQUAL(configuration.min_timespan, 302400);
    BOOST_REQUIRE_EQUAL(configuration.max_timespan, 4838400);
    BOOST_REQUIRE_EQUAL(configuration.retargeting_interval, 2016);
}

BOOST_AUTO_TEST_SUITE_END()
