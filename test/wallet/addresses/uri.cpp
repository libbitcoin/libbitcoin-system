/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(uri_tests)

using namespace bc::system::wallet;

BOOST_AUTO_TEST_CASE(uri__parse__slash_foo_dot_bar__path)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("/foo.bar"));
    BOOST_REQUIRE_EQUAL(parsed.path(), "/foo.bar");
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(!parsed.has_query());
}

BOOST_AUTO_TEST_CASE(uri__parse__foo_colon_bar__scheme_and_bar)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("foo:bar?baz=boo"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "bar");
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BOOST_AUTO_TEST_CASE(uri__parse__foo_colon_slash_bar__scheme_and_path)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("foo:/bar?baz=boo"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/bar");
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BOOST_AUTO_TEST_CASE(uri__parse__foo_colon_double_slash_bar__scheme_and_authority)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("foo://bar/bee?baz=boo"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "bar");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/bee");
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BOOST_AUTO_TEST_CASE(uri__parse__http_roundtrip__expected)
{
    const auto test = "http://github.com/libbitcoin?good=true#nice";
    uri parsed;
    BOOST_REQUIRE(parsed.decode(test));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE(parsed.has_fragment());

    BOOST_REQUIRE_EQUAL(parsed.scheme(), "http");
    BOOST_REQUIRE_EQUAL(parsed.authority(), "github.com");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/libbitcoin");
    BOOST_REQUIRE_EQUAL(parsed.query(), "good=true");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "nice");

    BOOST_REQUIRE_EQUAL(parsed.encoded(), test);
}

BOOST_AUTO_TEST_CASE(uri__parse__messy_roundtrip__expected)
{
    const auto test = "TEST:%78?%79#%7a";
    uri parsed;
    BOOST_REQUIRE(parsed.decode(test));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE(parsed.has_fragment());

    BOOST_REQUIRE_EQUAL(parsed.scheme(), "test");
    BOOST_REQUIRE_EQUAL(parsed.path(), "x");
    BOOST_REQUIRE_EQUAL(parsed.query(), "y");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "z");

    BOOST_REQUIRE_EQUAL(parsed.encoded(), test);
}

BOOST_AUTO_TEST_CASE(uri__parse__scheme__expected)
{
    uri parsed;
    BOOST_REQUIRE(!parsed.decode(""));
    BOOST_REQUIRE(!parsed.decode("1:"));

    // Valid scheme cases
    BOOST_REQUIRE(parsed.decode("x:"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "x");
    BOOST_REQUIRE_EQUAL(parsed.path(), "");

    BOOST_REQUIRE(parsed.decode("x::"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "x");
    BOOST_REQUIRE_EQUAL(parsed.path(), ":");

    // Empty scheme
    BOOST_REQUIRE(!parsed.decode(":"));
    BOOST_REQUIRE(!parsed.decode("%78:"));
    BOOST_REQUIRE(!parsed.decode(":/path"));
}

BOOST_AUTO_TEST_CASE(uri__parse__no_scheme__expected)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("/path"));
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());

    BOOST_REQUIRE(parsed.decode("/path?query=val"));
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE_EQUAL(parsed.query(), "query=val");
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(!parsed.has_fragment());

    BOOST_REQUIRE(parsed.decode("//host:42"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "host:42");
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());

    BOOST_REQUIRE(parsed.decode("//host#frag"));
    BOOST_REQUIRE(!parsed.decode("/path%GG"));
}

BOOST_AUTO_TEST_CASE(uri__parse__authority__expected)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("test:/"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/");

    BOOST_REQUIRE(parsed.decode("test://"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "");
    BOOST_REQUIRE_EQUAL(parsed.path(), "");

    BOOST_REQUIRE(parsed.decode("test:///"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/");

    BOOST_REQUIRE(parsed.decode("test:/x//"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/x//");

    BOOST_REQUIRE(parsed.decode("ssh://git@github.com:22/path/"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "git@github.com:22");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path/");

    // Invalid authority-form
    BOOST_REQUIRE(parsed.decode("//host"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "host");
    BOOST_REQUIRE(!parsed.has_scheme());
}

BOOST_AUTO_TEST_CASE(uri__parse__query__expected)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("test:#?"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "?");

    BOOST_REQUIRE(parsed.decode("test:?&&x=y&z"));
    BOOST_REQUIRE(parsed.has_scheme());
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

    // Scheme-less query
    BOOST_REQUIRE(parsed.decode("/path?key=value"));
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE_EQUAL(parsed.query(), "key=value");
}

BOOST_AUTO_TEST_CASE(uri__parse__fragment__expected)
{
    uri parsed;
    BOOST_REQUIRE(parsed.decode("test:?"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_fragment());

    BOOST_REQUIRE(parsed.decode("test:#"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "");

    BOOST_REQUIRE(parsed.decode("test:#?"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "?");

    // Scheme-less fragment
    BOOST_REQUIRE(parsed.decode("/path#frag"));
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "frag");
}

BOOST_AUTO_TEST_CASE(uri__encode__positive__expected)
{
    uri parsed;
    parsed.set_scheme("test");
    parsed.set_authority("user@hostname");
    parsed.set_path("/some/path/?/#");
    parsed.set_query("tacos=yummy");
    parsed.set_fragment("good evening");
    BOOST_REQUIRE_EQUAL(parsed.encoded(), "test://user@hostname/some/path/%3F/%23?tacos=yummy#good%20evening");

    parsed.remove_authority();
    parsed.remove_query();
    parsed.remove_fragment();
    BOOST_REQUIRE_EQUAL(parsed.encoded(), "test:/some/path/%3F/%23");
}

BOOST_AUTO_TEST_CASE(uri__encode__without_scheme__expected)
{
    uri parsed;
    parsed.set_path("/path");
    parsed.set_query("key=value");
    BOOST_REQUIRE_EQUAL(parsed.encoded(), "/path?key=value");
}

BOOST_AUTO_TEST_CASE(uri__parse__http1_1_targets__expected)
{
    uri parsed;
    BOOST_REQUIRE(!parsed.decode(""));

    // Origin-form
    BOOST_REQUIRE(parsed.decode("/path"));
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");

    BOOST_REQUIRE(parsed.decode("/path?query=val"));
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_authority());
    BOOST_REQUIRE(parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE_EQUAL(parsed.query(), "query=val");

    // Absolute-form
    BOOST_REQUIRE(parsed.decode("test://example.com/path"));
    BOOST_REQUIRE(parsed.has_scheme());
    BOOST_REQUIRE_EQUAL(parsed.scheme(), "test");
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "example.com");
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());
    BOOST_REQUIRE(parsed.decode("/path#frag"));
    BOOST_REQUIRE_EQUAL(parsed.path(), "/path");
    BOOST_REQUIRE_EQUAL(parsed.fragment(), "frag");

    // Authority-form
    BOOST_REQUIRE(parsed.decode("//host:42"));
    BOOST_REQUIRE(parsed.has_authority());
    BOOST_REQUIRE_EQUAL(parsed.authority(), "host:42");
    BOOST_REQUIRE(!parsed.has_scheme());
    BOOST_REQUIRE(!parsed.has_query());
    BOOST_REQUIRE(!parsed.has_fragment());

    // Invalid encoding
    BOOST_REQUIRE(!parsed.decode(":/path"));
    BOOST_REQUIRE(!parsed.decode("/path%GG"));
}

BOOST_AUTO_TEST_SUITE_END()
