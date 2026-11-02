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

BC_AUTO_TEST_SUITE(error_t_tests)

// These test std::error_code equality operator overrides.

BC_AUTO_TEST_CASE(error_t__code__success__false_expected_message)
{
    constexpr auto value = error::success;
    const auto ec = code(value);
    BC_REQUIRE(!ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "success");
}

BC_AUTO_TEST_CASE(error_t__code__not_found__true_expected_message)
{
    constexpr auto value = error::not_found;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "object does not exist");
}

BC_AUTO_TEST_CASE(error_t__code__not_implemented__true_expected_message)
{
    constexpr auto value = error::not_implemented;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "feature not implemented");
}

BC_AUTO_TEST_SUITE_END()
