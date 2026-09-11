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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(bitcoin_uri_tests)

using namespace bc::system::wallet;

// Constructors
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__uninitialized__false)
{
    BOOST_REQUIRE(!bitcoin_uri());
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__initialized__true)
{
    BOOST_REQUIRE(bitcoin_uri("bitcoin:"));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__scheme_mixed_case__normalized)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcOin:").encoded(), "bitcoin:");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__invalid_scheme__false)
{
    BOOST_REQUIRE(!bitcoin_uri("fedcoin:"));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__payment_address_only__false)
{
    BOOST_REQUIRE(!bitcoin_uri("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__bare_address__false)
{
    BOOST_REQUIRE(!bitcoin_uri("hfFGUXFPKkQ5M6LC6aEUKMsURdhw93bUdYdacEtBA8XttLv7evZkira2i"));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__fragment__false)
{
    BOOST_REQUIRE(!bitcoin_uri("bitcoin:#satoshi"));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__strict__test)
{
    BOOST_REQUIRE(!bitcoin_uri("bitcoin:?label=Some テスト"));
}

// Setters
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(bitcoin_uri__set_path__payment_address__expected_encoding)
{
    const auto expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    const auto expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    BOOST_REQUIRE(uri.set_path(expected_payment));
    BOOST_REQUIRE_EQUAL(uri.encoded(), expected_uri);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__set_address__reset_payment__latter_payment)
{
    const auto expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    const auto expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    uri.set_address(payment_address("1BoatSLRHtKNngkdXEeobR76b53LETtpyT"));
    uri.set_address(payment_address(expected_payment));
    BOOST_REQUIRE_EQUAL(uri.encoded(), expected_uri);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__set_path__reset_path__false)
{
    const auto expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    const auto expected_uri = std::string("bitcoin:") + expected_payment;

    bitcoin_uri uri;
    uri.set_address(payment_address("1BoatSLRHtKNngkdXEeobR76b53LETtpyT"));

    // The set_path will not reset a path. This is necessary to catch failures in non-strict parsing.
    BOOST_REQUIRE(!uri.set_path(expected_payment));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__set_amount__reset_amount__latter_amount)
{
    bitcoin_uri uri;
    uri.set_amount(10000000000);
    uri.set_amount(120000);
    BOOST_REQUIRE_EQUAL(uri.encoded(), "bitcoin:?amount=0.0012");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__all_setters__complex_uri__expected_encoding)
{
    bitcoin_uri uri;
    uri.set_path("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    uri.set_amount(120000);
    uri.set_label("&=\n");
    uri.set_message("hello bitcoin");
    uri.set_r("http://example.com?purchase=shoes&user=bob");

    // % escaping changed when we switched to boost.url.
    ////BOOST_REQUIRE_EQUAL(uri.encoded(),
    ////    "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
    ////    "amount=0.0012&"
    ////    "label=%26%3D%0A&"
    ////    "message=hello%20bitcoin&"
    ////    "r=http://example.com?purchase%3Dshoes%26user%3Dbob");

    // % escaping changed again when upgrading from boost 1.87 to boost 1.91.
    ////BOOST_REQUIRE_EQUAL(uri.encoded(),
    ////    "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
    ////    "amount=0.0012&"
    ////    "label=%26=%0A&"
    ////    "message=hello%20bitcoin&"
    ////    "r=http://example.com?purchase=shoes%26user=bob");

    // TODO: enable when all on boost 1.91.
    ////BOOST_REQUIRE_EQUAL(uri.encoded(),
    ////    "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
    ////    "amount=0.0012&"
    ////    "label=%26=%0A&"
    ////    "message=hello+bitcoin&"
    ////    "r=http://example.com?purchase=shoes%26user=bob");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__set_parameter__amount_denormalized__normalized)
{
    bitcoin_uri uri;
    BOOST_REQUIRE(uri.set_parameter("amount", ".0012"));
    BOOST_REQUIRE_EQUAL(uri.encoded(), "bitcoin:?amount=0.0012");
}

// Getters
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(bitcoin_uri__amount__set__expected)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcoin:?amount=0.0012").amount(), 120000u);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__label__escaped__expected)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcoin:?label=%26%3D%0A").label(), "&=\n");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__message__escaped__expected)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcoin:?message=hello%20bitcoin").message(), "hello bitcoin");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__r__escaped__expected)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcoin:?r=http://example.com?purchase%3Dshoes%26user%3Dbob").r(), "http://example.com?purchase=shoes&user=bob");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__payment__valid__expected)
{
    const auto expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    const auto expected_uri = std::string("bitcoin:") + expected_payment;
    BOOST_REQUIRE_EQUAL(bitcoin_uri(expected_uri).payment().encoded(), expected_payment);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__address__payment__expected)
{
    const auto expected_payment = "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD";
    const auto expected_uri = std::string("bitcoin:") + expected_payment;
    BOOST_REQUIRE_EQUAL(bitcoin_uri(expected_uri).address(), expected_payment);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__parameter_amount__denormalized__normalized)
{
    BOOST_REQUIRE_EQUAL(bitcoin_uri("bitcoin:?amount=.0012").parameter("amount"), "0.0012");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__parameters_all__complex_uri__expected)
{
    bitcoin_uri uri(
        "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
        "amount=0.0012&"
        "label=%26%3D%0A&"
        "message=hello%20bitcoin&"
        "r=http://example.com?purchase%3Dshoes%26user%3Dbob");

    BOOST_REQUIRE_EQUAL(uri.address(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE_EQUAL(uri.parameter("amount"), "0.0012");
    BOOST_REQUIRE_EQUAL(uri.parameter("label"), "&=\n");
    BOOST_REQUIRE_EQUAL(uri.parameter("message"), "hello bitcoin");
    BOOST_REQUIRE_EQUAL(uri.parameter("r"), "http://example.com?purchase=shoes&user=bob");
}


// authority

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__authority_in_place_of_path__expected)
{
    const bitcoin_uri uri("bitcoin://113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE(uri);
    BOOST_REQUIRE_EQUAL(uri.address(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__authority_not_an_address__false)
{
    BOOST_REQUIRE(!bitcoin_uri("bitcoin://bogus"));
}

// amount

BOOST_AUTO_TEST_CASE(bitcoin_uri__construct__non_numeric_amount__false)
{
    BOOST_REQUIRE(!bitcoin_uri("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?amount=bogus"));
}

// operators

BOOST_AUTO_TEST_CASE(bitcoin_uri__equality__same__equal)
{
    const bitcoin_uri left("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    const bitcoin_uri right("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE(left == right);
    BOOST_REQUIRE(!(left != right));
    BOOST_REQUIRE(!(left < right));
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__equality__different_query__unequal)
{
    const bitcoin_uri left("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    const bitcoin_uri right("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?amount=1");
    BOOST_REQUIRE(left != right);
    BOOST_REQUIRE(left < right);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__stream__valid__round_trips)
{
    const std::string expected{ "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD" };
    std::istringstream in{ expected };
    bitcoin_uri instance{};
    in >> instance;
    BOOST_REQUIRE_EQUAL(instance.encoded(), expected);

    std::ostringstream out{};
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), expected);
}

BOOST_AUTO_TEST_CASE(bitcoin_uri__stream__invalid__throws)
{
    bitcoin_uri instance{};
    BOOST_REQUIRE_THROW(std::istringstream("bogus") >> instance, istream_exception);
}
BOOST_AUTO_TEST_SUITE_END()
