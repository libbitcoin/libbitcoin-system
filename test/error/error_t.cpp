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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(error_t_tests)

// These test std::error_code equality operator overrides.

BOOST_AUTO_TEST_CASE(error_t__code__success__false_exected_message)
{
    constexpr auto value = error::success;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "success");
}

BOOST_AUTO_TEST_CASE(error_t__code__unknown__true_exected_message)
{
    const auto ec = code(error::unknown);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == error::unknown);
    BOOST_REQUIRE_EQUAL(ec.message(), "unknown error");
}

BOOST_AUTO_TEST_CASE(error_t__code__not_found__true_exected_message)
{
    const auto ec = code(error::not_found);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == error::not_found);
    BOOST_REQUIRE_EQUAL(ec.message(), "object does not exist");
}

BOOST_AUTO_TEST_CASE(error_t__code__not_implemented__true_exected_message)
{
    const auto ec = code(error::not_implemented);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == error::not_implemented);
    BOOST_REQUIRE_EQUAL(ec.message(), "feature not implemented");
}

BOOST_AUTO_TEST_SUITE_END()
