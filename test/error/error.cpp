/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <system_error>

// These test general std::error behaviors, see _t tests for codes.
BOOST_AUTO_TEST_SUITE(error_tests)

static const auto system_category_name = "system";
static const auto bitcoin_category_name = "bc";
static const auto default_message = "system code";
static const auto success_message = "success";
static const auto not_found_message = "object does not exist";

// Overflow of message map, within message code range, gives default message.
BOOST_AUTO_TEST_CASE(error_t__code__message_overflow__default_message)
{
    const auto ec = code(error::error_last);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE_EQUAL(ec.message(), default_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::error_last));
}

// Overflows can be cast in, and value retained, and yield default message.
BOOST_AUTO_TEST_CASE(error_t__code__value_overflow__default_message)
{
    const auto overflow = static_cast<int>(error::error_last);
    const auto ec = code(static_cast<error::error_t>(overflow));
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE_EQUAL(ec.message(), default_message);
    BOOST_REQUIRE_EQUAL(ec.value(), overflow);
}

BOOST_AUTO_TEST_CASE(error_t__code__success__negative_error)
{
    const auto ec = code(error::success);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE_EQUAL(ec.message(), success_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::success));
}

BOOST_AUTO_TEST_CASE(error_t__code__not_found__positive_error)
{
    const auto ec = code(error::not_found);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE_EQUAL(ec.message(), not_found_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::not_found));
}

BOOST_AUTO_TEST_CASE(error_t__code__clear__false_but_resets_to_system)
{
    auto ec = code(error::not_found);
    BOOST_REQUIRE(ec);
    ec.clear();
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE_NE(ec.message(), success_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::success));
    BOOST_REQUIRE_EQUAL(ec.category().name(), system_category_name);
}

BOOST_AUTO_TEST_CASE(error_t__code__assign__expected)
{
    auto ec = code(error::not_found);
    BOOST_REQUIRE(ec);
    ec.assign(static_cast<int>(error::not_found), error::error_category::singleton);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE_EQUAL(ec.message(), not_found_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::not_found));
    BOOST_REQUIRE_EQUAL(ec.category().name(), bitcoin_category_name);
}

BOOST_AUTO_TEST_CASE(error_t__code__assignment__expected)
{
    auto ec = code(error::success);
    BOOST_REQUIRE(!ec);
    ec = std::error_code(error::not_found);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE_EQUAL(ec.message(), not_found_message);
    BOOST_REQUIRE_EQUAL(ec.value(), static_cast<int>(error::not_found));
    BOOST_REQUIRE_EQUAL(ec.category().name(), bitcoin_category_name);
}

BOOST_AUTO_TEST_CASE(error_t__code__category__expected)
{
    BOOST_REQUIRE(code().category() == code().category());
    BOOST_REQUIRE(code().category() != code(error::success).category());
    BOOST_REQUIRE(code(error::success).category() == code(error::success).category());
    BOOST_REQUIRE(code(error::success).category() == code(error::not_found).category());
}

BOOST_AUTO_TEST_CASE(error_t__code__category_name__expected)
{
    BOOST_REQUIRE_EQUAL(code().category().name(), system_category_name);
    BOOST_REQUIRE_EQUAL(code(error::success).category().name(), bitcoin_category_name);
}

BOOST_AUTO_TEST_CASE(error_t__code__default_error_condition_category_name__expected)
{
    // This varies by platform ("generic" on macOS/clang, "system" elsewhere).
    ////BOOST_REQUIRE_EQUAL(code().default_error_condition().category().name(), system_category_name);
    BOOST_REQUIRE_EQUAL(code(error::not_found).default_error_condition().category().name(), bitcoin_category_name);
}

BOOST_AUTO_TEST_SUITE_END()
