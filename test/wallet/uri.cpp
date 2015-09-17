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

BOOST_AUTO_TEST_SUITE(uri_tests)

BOOST_AUTO_TEST_CASE(uri_basic_test)
{
    const std::string test = "http://github.com/libbitcoin?good=true#nice";
    wallet::uri parsed;
    BOOST_REQUIRE(parsed.decode(test));

    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE(parsed.has_fragment());

    BOOST_REQUIRE_EQUAL(parsed.scheme(), "http");
    BOOST_REQUIRE_EQUAL(parsed.authority(), "github.com");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/libbitcoin");
    BOOST_REQUIRE_EQUAL(parsed.query(), "good=true");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "nice");

    BOOST_REQUIRE_EQUAL(parsed.encode(), test);
}

BOOST_AUTO_TEST_CASE(uri_messy_roundtrip_test)
{
    const std::string test = "TEST:%78?%79#%7a";
    wallet::uri parsed;
    BOOST_REQUIRE(parsed.decode(test));

    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE(parsed.has_fragment());

    BOOST_REQUIRE_EQUAL(parsed.scheme(), "test");
    BOOST_REQUIRE_EQUAL(parsed.path(), "x");
    BOOST_REQUIRE_EQUAL(parsed.query(), "y");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "z");

    BOOST_REQUIRE_EQUAL(parsed.encode(), test);
}

BOOST_AUTO_TEST_CASE(uri_scheme_test)
{
    wallet::uri parsed;
    BOOST_REQUIRE(!parsed.decode(""));
    BOOST_REQUIRE(!parsed.decode(":"));
    BOOST_REQUIRE(!parsed.decode("1:"));
    BOOST_REQUIRE(!parsed.decode("%78:"));

    BOOST_REQUIRE(parsed.decode("x:"));
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "x");

    BOOST_REQUIRE(parsed.decode("x::"));
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "x");
    BOOST_REQUIRE_EQUAL(parsed.path(), ":");
}

BOOST_AUTO_TEST_CASE(uri_non_strict_test)
{
    wallet::uri parsed;
    BOOST_REQUIRE(!parsed.decode("test:?テスト"));

    BOOST_REQUIRE(parsed.decode("test:テスト", false));
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "test");
    BOOST_REQUIRE_EQUAL(parsed.path(), "テスト");
}

BOOST_AUTO_TEST_CASE(uri_authority_test)
{
    wallet::uri parsed;

    BOOST_REQUIRE(parsed.decode("test:/"));
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/");

    BOOST_REQUIRE(parsed.decode("test://"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "");
    BOOST_REQUIRE_EQUAL(parsed.path(), "");

    BOOST_REQUIRE(parsed.decode("test:///"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/");

    BOOST_REQUIRE(parsed.decode("test:/x//"));
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/x//");

    BOOST_REQUIRE(parsed.decode("ssh://git@github.com:22/path/"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "git@github.com:22");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path/");
}

BOOST_AUTO_TEST_CASE(uri_query_test)
{
    wallet::uri parsed;

    BOOST_REQUIRE(parsed.decode("test:#?"));
    BOOST_REQUIRE(!parsed.has_query());

    BOOST_REQUIRE(parsed.decode("test:?&&x=y&z"));
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.query(), "&&x=y&z");

    auto map = parsed.decode_query();
    BOOST_REQUIRE(map.end() != map.find(""));
    BOOST_REQUIRE(map.end() != map.find("x"));
    BOOST_REQUIRE(map.end() != map.find("z"));
    BOOST_REQUIRE(map.end() == map.find("y"));

    BOOST_REQUIRE_EQUAL(map[""], "");
    BOOST_REQUIRE_EQUAL(map["x"], "y");
    BOOST_REQUIRE_EQUAL(map["z"], "");
}

BOOST_AUTO_TEST_CASE(uri_fragment_test)
{
    wallet::uri parsed;

    BOOST_REQUIRE(parsed.decode("test:?"));
    BOOST_REQUIRE(!parsed.has_fragment());

    BOOST_REQUIRE(parsed.decode("test:#"));
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "");

    BOOST_REQUIRE(parsed.decode("test:#?"));
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "?");
}

BOOST_AUTO_TEST_CASE(uri_encode_test)
{
    wallet::uri out;
    out.set_scheme("test");
    out.set_authority("user@hostname");
    out.set_path("/some/path/?/#");
    out.set_query("tacos=yummy");
    out.set_fragment("good evening");

    BOOST_REQUIRE_EQUAL(out.encode(),
        "test://user@hostname/some/path/%3F/%23?tacos=yummy#good%20evening");

    out.remove_authority();
    out.remove_query();
    out.remove_fragment();

    BOOST_REQUIRE_EQUAL(out.encode(),
        "test:/some/path/%3F/%23");
}

BOOST_AUTO_TEST_SUITE_END()
