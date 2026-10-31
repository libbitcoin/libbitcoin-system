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
#ifndef LIBBITCOIN_SYSTEM_UNIT_TEST_ALIASES_HPP
#define LIBBITCOIN_SYSTEM_UNIT_TEST_ALIASES_HPP

#include <bitcoin/system/unit_test/macros.hpp>

/// Boost.Test names of the unit test macros.
#define BOOST_AUTO_TEST_SUITE BC_AUTO_TEST_SUITE
#define BOOST_FIXTURE_TEST_SUITE BC_FIXTURE_TEST_SUITE
#define BOOST_AUTO_TEST_SUITE_END BC_AUTO_TEST_SUITE_END
#define BOOST_FIXTURE_TEST_CASE BC_FIXTURE_TEST_CASE
#define BOOST_AUTO_TEST_CASE BC_AUTO_TEST_CASE
#define BOOST_WARN BC_WARN
#define BOOST_CHECK BC_CHECK
#define BOOST_REQUIRE BC_REQUIRE
#define BOOST_WARN_MESSAGE BC_WARN_MESSAGE
#define BOOST_CHECK_MESSAGE BC_CHECK_MESSAGE
#define BOOST_REQUIRE_MESSAGE BC_REQUIRE_MESSAGE
#define BOOST_WARN_EQUAL BC_WARN_EQUAL
#define BOOST_CHECK_EQUAL BC_CHECK_EQUAL
#define BOOST_REQUIRE_EQUAL BC_REQUIRE_EQUAL
#define BOOST_CHECK_NE BC_CHECK_NE
#define BOOST_REQUIRE_NE BC_REQUIRE_NE
#define BOOST_CHECK_LT BC_CHECK_LT
#define BOOST_REQUIRE_LT BC_REQUIRE_LT
#define BOOST_REQUIRE_LE BC_REQUIRE_LE
#define BOOST_REQUIRE_GT BC_REQUIRE_GT
#define BOOST_REQUIRE_GE BC_REQUIRE_GE
#define BOOST_REQUIRE_CLOSE BC_REQUIRE_CLOSE
#define BOOST_REQUIRE_EQUAL_COLLECTIONS BC_REQUIRE_EQUAL_COLLECTIONS
#define BOOST_REQUIRE_THROW BC_REQUIRE_THROW
#define BOOST_REQUIRE_NO_THROW BC_REQUIRE_NO_THROW
#define BOOST_REQUIRE_EXCEPTION BC_REQUIRE_EXCEPTION
#define BOOST_FAIL BC_FAIL
#define BOOST_TEST_MESSAGE BC_TEST_MESSAGE
#define BOOST_TEST_CONTEXT BC_TEST_CONTEXT

#endif
