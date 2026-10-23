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

BOOST_AUTO_TEST_SUITE(json_error_t_tests)

BOOST_AUTO_TEST_CASE(json_error_t__code__json_success__false_expected_message)
{
    constexpr auto value = error::json_success;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json success");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_syntax__true_expected_message)
{
    constexpr auto value = error::json_syntax;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json syntax error");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_extra_data__true_expected_message)
{
    constexpr auto value = error::json_extra_data;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json extra data");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_incomplete__true_expected_message)
{
    constexpr auto value = error::json_incomplete;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json incomplete");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_too_deep__true_expected_message)
{
    constexpr auto value = error::json_too_deep;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json too deep");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_invalid_escape__true_expected_message)
{
    constexpr auto value = error::json_invalid_escape;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json invalid escape");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_expected_hex_digit__true_expected_message)
{
    constexpr auto value = error::json_expected_hex_digit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json expected hex digit");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_expected_utf16_escape__true_expected_message)
{
    constexpr auto value = error::json_expected_utf16_escape;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json expected utf16 escape");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_illegal_leading_surrogate__true_expected_message)
{
    constexpr auto value = error::json_illegal_leading_surrogate;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json illegal leading surrogate");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_illegal_trailing_surrogate__true_expected_message)
{
    constexpr auto value = error::json_illegal_trailing_surrogate;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json illegal trailing surrogate");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_invalid_utf8__true_expected_message)
{
    constexpr auto value = error::json_invalid_utf8;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json invalid utf8");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_exponent_overflow__true_expected_message)
{
    constexpr auto value = error::json_exponent_overflow;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json exponent overflow");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_number_too_large__true_expected_message)
{
    constexpr auto value = error::json_number_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json number too large");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_string_too_large__true_expected_message)
{
    constexpr auto value = error::json_string_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json string too large");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_key_too_large__true_expected_message)
{
    constexpr auto value = error::json_key_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json key too large");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_array_too_large__true_expected_message)
{
    constexpr auto value = error::json_array_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json array too large");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_object_too_large__true_expected_message)
{
    constexpr auto value = error::json_object_too_large;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json object too large");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_null__true_expected_message)
{
    constexpr auto value = error::json_not_null;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not null");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_bool__true_expected_message)
{
    constexpr auto value = error::json_not_bool;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not bool");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_number__true_expected_message)
{
    constexpr auto value = error::json_not_number;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not number");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_int64__true_expected_message)
{
    constexpr auto value = error::json_not_int64;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not int64");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_uint64__true_expected_message)
{
    constexpr auto value = error::json_not_uint64;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not uint64");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_double__true_expected_message)
{
    constexpr auto value = error::json_not_double;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not double");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_string__true_expected_message)
{
    constexpr auto value = error::json_not_string;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not string");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_array__true_expected_message)
{
    constexpr auto value = error::json_not_array;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not array");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_object__true_expected_message)
{
    constexpr auto value = error::json_not_object;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not object");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_not_exact__true_expected_message)
{
    constexpr auto value = error::json_not_exact;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json not exact");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_out_of_range__true_expected_message)
{
    constexpr auto value = error::json_out_of_range;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json out of range");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_size_mismatch__true_expected_message)
{
    constexpr auto value = error::json_size_mismatch;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json size mismatch");
}

BOOST_AUTO_TEST_CASE(json_error_t__code__json_exhausted_variants__true_expected_message)
{
    constexpr auto value = error::json_exhausted_variants;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "json exhausted variants");
}

BOOST_AUTO_TEST_SUITE_END()
