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

BOOST_AUTO_TEST_SUITE(x509_error_t_tests)

BOOST_AUTO_TEST_CASE(x509_error_t__code__x509_success__false_expected_message)
{
    constexpr auto value = error::x509_success;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "x509 success");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__chain_empty__true_expected_message)
{
    constexpr auto value = error::chain_empty;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate chain empty");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__chain_untrusted__true_expected_message)
{
    constexpr auto value = error::chain_untrusted;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate chain not anchored");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__certificate_not_yet_valid__true_expected_message)
{
    constexpr auto value = error::certificate_not_yet_valid;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate not yet valid");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__certificate_expired__true_expected_message)
{
    constexpr auto value = error::certificate_expired;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate expired");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__issuer_mismatch__true_expected_message)
{
    constexpr auto value = error::issuer_mismatch;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate issuer does not match");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__certificate_signature__true_expected_message)
{
    constexpr auto value = error::certificate_signature;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate signature invalid");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__issuer_not_authority__true_expected_message)
{
    constexpr auto value = error::issuer_not_authority;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate issuer not an authority");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__path_length_exceeded__true_expected_message)
{
    constexpr auto value = error::path_length_exceeded;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate path length exceeded");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__key_usage_invalid__true_expected_message)
{
    constexpr auto value = error::key_usage_invalid;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate key usage invalid");
}

BOOST_AUTO_TEST_CASE(x509_error_t__code__extended_key_usage_invalid__true_expected_message)
{
    constexpr auto value = error::extended_key_usage_invalid;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "certificate extended key usage invalid");
}

BOOST_AUTO_TEST_SUITE_END()
