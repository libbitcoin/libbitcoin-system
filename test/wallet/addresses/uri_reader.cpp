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

#include <optional>

BC_AUTO_TEST_SUITE(uri_reader_tests)

using namespace bc::system::wallet;

// Test helper that relies on bitcoin_uri.
static bitcoin_uri parse(const std::string& uri)
{
    return uri_reader::parse<bitcoin_uri>(uri);
}

// Demonstrate custom uri_reader.
struct custom_reader
  : public uri_reader
{
    custom_reader() NOEXCEPT
      : authority_(false)
    {
    }

    bool is_valid() const NOEXCEPT
    {
        return !myscheme.empty() && !authority_;
    }

    virtual bool set_scheme(const std::string& scheme) NOEXCEPT
    {
        myscheme = scheme;
        return true;
    }

    virtual bool set_authority(const std::string& /* authority */) NOEXCEPT
    {
        // This URI doesn't support an authority component.
        authority_ = true;
        return false;
    }

    virtual bool set_path(const std::string& path) NOEXCEPT
    {
        mypath = path;
        return true;
    }

    virtual bool set_fragment(const std::string& fragment) NOEXCEPT
    {
        myfragment = std::optional<std::string>(std::in_place, fragment);
        return true;
    }

    virtual bool set_parameter(const std::string& key,
        const std::string& value) NOEXCEPT
    {
        if (key == "myparam1")
            myparam1 = std::optional<std::string>(std::in_place, value);
        else if (key == "myparam2")
            myparam2 = std::optional<std::string>(std::in_place, value);

        return true;
    }

    std::string myscheme;
    std::string myauthority;
    std::string mypath;

    // Use optionals when there is a semantic distinction between no value and default value.
    std::optional<std::string> myfragment;
    std::optional<std::string> myparam1;
    std::optional<std::string> myparam2;

private:
    bool authority_;
};

BC_AUTO_TEST_CASE(uri_reader__parse__typical_uri__test)
{
    const auto uri = parse("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD?amount=0.1");
    BC_REQUIRE(uri);
    BC_REQUIRE_EQUAL(uri.payment().encoded(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BC_REQUIRE_EQUAL(uri.amount(), 10000000u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE(uri.r().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__positive_scheme__test)
{
    BC_REQUIRE(parse("bitcoin:"));
    BC_REQUIRE(parse("Bitcoin:"));
    BC_REQUIRE(parse("bitcOin:"));
    BC_REQUIRE(parse("BITCOIN:"));
}

BC_AUTO_TEST_CASE(uri_reader__parse__negative_scheme__test)
{
    BC_REQUIRE(!parse("bitcorn:"));
}

BC_AUTO_TEST_CASE(uri_reader__parse__positive_empty_name_parameter__test)
{
    BC_REQUIRE(parse("bitcoin:?"));
    BC_REQUIRE(parse("bitcoin:?&"));
    BC_REQUIRE(parse("bitcoin:?=y"));
    BC_REQUIRE(parse("bitcoin:?="));
}

BC_AUTO_TEST_CASE(uri_reader__parse__positive_unknown_optional_parameter__test)
{
    BC_REQUIRE(parse("bitcoin:?x=y"));
    BC_REQUIRE(parse("bitcoin:?x="));
    BC_REQUIRE(parse("bitcoin:?x"));

    const auto uri = parse("bitcoin:?ignore=true");
    BC_REQUIRE(uri);
    BC_REQUIRE(uri.address().empty());
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE(uri.r().empty());
    BC_REQUIRE(uri.parameter("ignore").empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__negative_unknown_required_parameter__test)
{
    const auto uri = parse("bitcoin:?req-ignore=false");
    BC_REQUIRE(!uri);
}

BC_AUTO_TEST_CASE(uri_reader__parse__address__test)
{
    const auto uri = parse("bitcoin:113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BC_REQUIRE(uri);
    BC_REQUIRE_EQUAL(uri.payment().encoded(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE(uri.r().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__uri_encoded_address__test)
{
    const auto uri = parse("bitcoin:%3113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
    BC_REQUIRE(uri);
    BC_REQUIRE_EQUAL(uri.payment().encoded(), "113Pfw4sFqN1T5kXUnKbqZHMJHN9oyjtgD");
}

BC_AUTO_TEST_CASE(uri_reader__parse__negative_address__test)
{
    BC_REQUIRE(!parse("bitcoin:&"));
    BC_REQUIRE(!parse("bitcoin:19l88"));
    BC_REQUIRE(!parse("bitcoin:19z88"));
}

BC_AUTO_TEST_CASE(uri_reader__parse__amount_only__test)
{
    const auto uri = parse("bitcoin:?amount=4.2");
    BC_REQUIRE(uri);
    BC_REQUIRE(!uri.payment());
    BC_REQUIRE_EQUAL(uri.amount(), 420000000u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE(uri.r().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__minimal_amount__test)
{
    const auto uri = parse("bitcoin:?amount=.");
    BC_REQUIRE(uri);
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
}

BC_AUTO_TEST_CASE(uri_reader__parse__invalid_amount__test)
{
    BC_REQUIRE(!parse("bitcoin:amount=4.2.1"));
    BC_REQUIRE(!parse("bitcoin:amount=bob"));
}

BC_AUTO_TEST_CASE(uri_reader__parse__label_only__test)
{
    const auto uri = parse("bitcoin:?label=test");
    BC_REQUIRE(uri);
    BC_REQUIRE(!uri.payment());
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
    BC_REQUIRE_EQUAL(uri.label(), "test");
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE(uri.r().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__reserved_symbol_with_lowercase_percent__test)
{
    const auto uri = parse("bitcoin:?label=%26%3d%6b");
    BC_REQUIRE_EQUAL(uri.label(), "&=k");
}

BC_AUTO_TEST_CASE(uri_reader__parse__negative_percent_encoding__test)
{
    BC_REQUIRE(!parse("bitcoin:label=%3"));
    BC_REQUIRE(!parse("bitcoin:label=%3G"));
}

BC_AUTO_TEST_CASE(uri_reader__parse__encoded_multibyte_utf8__test)
{
    const auto uri = parse("bitcoin:?label=%E3%83%95");
    BC_REQUIRE_EQUAL(uri.label(), "フ");
}

BC_AUTO_TEST_CASE(uri_reader__parse__non_strict_encoded_multibyte_utf8_with_unencoded_label_space__test)
{
    const auto uri = parse("bitcoin:?label=Some テスト");
    BC_REQUIRE(uri.label().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__message_only__test)
{
    const auto uri = parse("bitcoin:?message=Hi%20Alice");
    BC_REQUIRE(!uri.payment());
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE_EQUAL(uri.message(), "Hi Alice");
    BC_REQUIRE(uri.r().empty());
}

BC_AUTO_TEST_CASE(uri_reader__parse__payment_protocol_only__test)
{
    const auto uri = parse("bitcoin:?r=http://www.example.com?purchase%3Dshoes");
    BC_REQUIRE(!uri.payment());
    BC_REQUIRE_EQUAL(uri.amount(), 0u);
    BC_REQUIRE(uri.label().empty());
    BC_REQUIRE(uri.message().empty());
    BC_REQUIRE_EQUAL(uri.r(), "http://www.example.com?purchase=shoes");
}

BC_AUTO_TEST_CASE(uri_reader__parse__custom_reader_optional_parameter_type__test)
{
    const auto custom = uri_reader::parse<custom_reader>("foo:part/abc?myparam1=1&myparam2=2#myfrag");
    BC_REQUIRE(custom.is_valid());
    BC_REQUIRE_EQUAL(custom.myscheme, "foo");
    BC_REQUIRE_EQUAL(custom.mypath, "part/abc");
    BC_REQUIRE(custom.myfragment);
    BC_REQUIRE_EQUAL(custom.myfragment.value(), "myfrag");
    BC_REQUIRE(custom.myparam1);
    BC_REQUIRE_EQUAL(custom.myparam1.value(), "1");
    BC_REQUIRE(custom.myparam2);
    BC_REQUIRE_EQUAL(custom.myparam2.value(), "2");
}

BC_AUTO_TEST_CASE(uri_reader__parse__custom_reader_unsupported_component__invalid)
{
    BC_REQUIRE(!uri_reader::parse<custom_reader>("foo://bar:42/part/abc?myparam1=1&myparam2=2#myfrag").is_valid());
}

BC_AUTO_TEST_CASE(uri_reader__parse__custom_reader__test)
{
    BC_REQUIRE(uri_reader::parse<custom_reader>("foo:?unknown=test").is_valid());
}

BC_AUTO_TEST_SUITE_END()
