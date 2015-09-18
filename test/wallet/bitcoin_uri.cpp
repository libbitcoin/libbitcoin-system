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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(bitcoin_uri_tests)

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__failure_reuse_hazard__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?amount=bob"));

    // Failed parse results in partial data in the parse result.
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");

    // Reused parse result here carries address from previous failed parse.
    BOOST_REQUIRE(uri_parse(result, "bitcoin:"));
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__success_reuse_hazard__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));

    // Reused parse result here carries address from previous successful parse.
    BOOST_REQUIRE(uri_parse(result, "bitcoin:"));
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__typical_uri__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?amount=0.1"));
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE(result.amount);
    BOOST_REQUIRE_EQUAL(result.amount.get(), 10000000u);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result___positive_scheme__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:"));
    BOOST_REQUIRE(uri_parse(result, "Bitcoin:"));
    BOOST_REQUIRE(uri_parse(result, "bitcOin:"));
    BOOST_REQUIRE(uri_parse(result, "BITCOIN:"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__negative_scheme__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcorn:"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result___positive_empty_name_parameter__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?"));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?&"));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?=y"));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?="));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result___positive_unknown_optional_parameter__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?x=y"));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?x="));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?x"));
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?ignore=true"));
    BOOST_REQUIRE(!result.address);
    BOOST_REQUIRE(!result.amount);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__negative_unknown_required_parameter__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:?req-ignore=false"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__address__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE(!result.amount);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__uri_encoded_address__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:%3113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD"));
    BOOST_REQUIRE(result.address);
    BOOST_REQUIRE_EQUAL(result.address.get().to_string(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__negative_address__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:&"));
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:19l88"));
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:19z88"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__amount_only__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?amount=4.2"));
    BOOST_REQUIRE(!result.address);
    BOOST_REQUIRE(result.amount);
    BOOST_REQUIRE_EQUAL(result.amount.get(), 420000000u);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__minimal_amount__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?amount=."));
    BOOST_REQUIRE(result.amount);
    BOOST_REQUIRE_EQUAL(result.amount.get(), 0);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__invalid_amount__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:amount=4.2.1"));
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:amount=bob"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__label_only__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?label=test"));
    BOOST_REQUIRE(!result.address);
    BOOST_REQUIRE(!result.amount);
    BOOST_REQUIRE(result.label);
    BOOST_REQUIRE_EQUAL(result.label.get(), "test");
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__reserved_symbol_with_lowercase_percent__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?label=%26%3d%6b"));
    BOOST_REQUIRE(result.label);
    BOOST_REQUIRE_EQUAL(result.label.get(), "&=k");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__negative_percent_encoding__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:label=%3"));
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:label=%3G"));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__encoded_multibyte_utf8__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?label=%E3%83%95"));
    BOOST_REQUIRE(result.label);
    BOOST_REQUIRE_EQUAL(result.label.get(), "フ");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__non_strict_encoded_multibyte_utf8_with_unencoded_label_space__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?label=Some テスト", false));
    BOOST_REQUIRE(result.label);
    BOOST_REQUIRE_EQUAL(result.label.get(), "Some テスト");
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__negative_strict_encoded_multibyte_utf8_with_unencoded_label_space__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(!uri_parse(result, "bitcoin:?label=Some テスト", true));
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__message_only__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?message=Hi%20Alice"));
    BOOST_REQUIRE(!result.address);
    BOOST_REQUIRE(!result.amount);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(result.message);
    BOOST_REQUIRE_EQUAL(result.message.get(), "Hi Alice");
    BOOST_REQUIRE(!result.r);
}

BOOST_AUTO_TEST_CASE(uri_parse__uri_parse_result__payment_protocol_only__test)
{
    uri_parse_result result;
    BOOST_REQUIRE(uri_parse(result, "bitcoin:?r=http://www.example.com?purchase%3Dshoes"));
    BOOST_REQUIRE(!result.address);
    BOOST_REQUIRE(!result.amount);
    BOOST_REQUIRE(!result.label);
    BOOST_REQUIRE(!result.message);
    BOOST_REQUIRE(result.r);
    BOOST_REQUIRE_EQUAL(result.r.get(), "http://www.example.com?purchase=shoes");
}

// Example class to demonstrate handling custom URI parameters.
struct custom_result
  : public uri_parse_result
{
    optional_string myparam;

protected:
    virtual bool set_param(const std::string& key, const std::string& value)
    {
        if ("myparam" == key)
            myparam.reset(value);

        return uri_parse_result::set_param(key, value);
    }
};

BOOST_AUTO_TEST_CASE(uri_parse__custom_result__custom_parameter_type__test)
{
    custom_result custom;
    BOOST_REQUIRE(uri_parse(custom, "bitcoin:?myparam=here"));
    BOOST_REQUIRE(!custom.address);
    BOOST_REQUIRE(!custom.amount);
    BOOST_REQUIRE(!custom.label);
    BOOST_REQUIRE(!custom.message);
    BOOST_REQUIRE(!custom.r);
    BOOST_REQUIRE(custom.myparam);
    BOOST_REQUIRE_EQUAL(custom.myparam.get(), "here");
}

BOOST_AUTO_TEST_CASE(uri_writer__encoded__unparameterized_uri__test)
{
    uri_writer writer;
    writer.write_address("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BOOST_REQUIRE_EQUAL(writer.encoded(), "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BOOST_AUTO_TEST_CASE(uri_writer__encoded__typical_uri__test)
{
    uri_writer writer;
    writer.write_address("113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    writer.write_amount(10000000000);
    writer.write_amount(120000);
    writer.write_label("&=\n");
    writer.write_message("hello bitcoin");
    writer.write_r("http://example.com?purchase=shoes&user=bob");

    BOOST_REQUIRE_EQUAL(writer.encoded(),
        "bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?"
        "amount=0.0012&"
        "label=%26%3D%0A&"
        "message=hello%20bitcoin&"
        "r=http://example.com?purchase%3Dshoes%26user%3Dbob");
}

BOOST_AUTO_TEST_SUITE_END()
