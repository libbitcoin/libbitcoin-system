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

BC_AUTO_TEST_SUITE(uri_tests)

using namespace bc::system::wallet;

BC_AUTO_TEST_CASE(uri__parse__slash_foo_dot_bar__path)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("/foo.bar"));
    BC_REQUIRE_EQUAL(parsed.path(), "/foo.bar");
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(!parsed.has_query());
}

BC_AUTO_TEST_CASE(uri__parse__foo_colon_bar__scheme_and_bar)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("foo:bar?baz=boo"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.path(), "bar");
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BC_AUTO_TEST_CASE(uri__parse__foo_colon_slash_bar__scheme_and_path)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("foo:/bar?baz=boo"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.path(), "/bar");
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BC_AUTO_TEST_CASE(uri__parse__foo_colon_double_slash_bar__scheme_and_authority)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("foo://bar/bee?baz=boo"));
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "foo");
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "bar");
    BC_REQUIRE_EQUAL(parsed.path(), "/bee");
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.query(), "baz=boo");
}

BC_AUTO_TEST_CASE(uri__parse__http_roundtrip__expected)
{
    const auto test = "http://github.com/libbitcoin?good=true#nice";
    uri parsed;
    BC_REQUIRE(parsed.decode(test));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE(parsed.has_fragment());

    BC_REQUIRE_EQUAL(parsed.scheme(), "http");
    BC_REQUIRE_EQUAL(parsed.authority(), "github.com");
    BC_REQUIRE_EQUAL(parsed.path(), "/libbitcoin");
    BC_REQUIRE_EQUAL(parsed.query(), "good=true");
    BC_REQUIRE_EQUAL(parsed.fragment(), "nice");

    BC_REQUIRE_EQUAL(parsed.encoded(), test);
}

BC_AUTO_TEST_CASE(uri__parse__messy_roundtrip__expected)
{
    const auto test = "TEST:%78?%79#%7a";
    uri parsed;
    BC_REQUIRE(parsed.decode(test));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE(parsed.has_fragment());

    BC_REQUIRE_EQUAL(parsed.scheme(), "test");
    BC_REQUIRE_EQUAL(parsed.path(), "x");
    BC_REQUIRE_EQUAL(parsed.query(), "y");
    BC_REQUIRE_EQUAL(parsed.fragment(), "z");

    BC_REQUIRE_EQUAL(parsed.encoded(), test);
}

BC_AUTO_TEST_CASE(uri__parse__scheme__expected)
{
    uri parsed;
    BC_REQUIRE(!parsed.decode(""));
    BC_REQUIRE(!parsed.decode("1:"));

    // Valid scheme cases
    BC_REQUIRE(parsed.decode("x:"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "x");
    BC_REQUIRE_EQUAL(parsed.path(), "");

    BC_REQUIRE(parsed.decode("x::"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "x");
    BC_REQUIRE_EQUAL(parsed.path(), ":");

    // Empty scheme
    BC_REQUIRE(!parsed.decode(":"));
    BC_REQUIRE(!parsed.decode("%78:"));
    BC_REQUIRE(!parsed.decode(":/path"));
}

BC_AUTO_TEST_CASE(uri__parse__no_scheme__expected)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("/path"));
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());

    BC_REQUIRE(parsed.decode("/path?query=val"));
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE_EQUAL(parsed.query(), "query=val");
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(!parsed.has_fragment());

    BC_REQUIRE(parsed.decode("//host:42"));
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "host:42");
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());

    BC_REQUIRE(parsed.decode("//host#frag"));
    BC_REQUIRE(!parsed.decode("/path%GG"));
}

BC_AUTO_TEST_CASE(uri__parse__authority__expected)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("test:/"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.path(), "/");

    BC_REQUIRE(parsed.decode("test://"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "");
    BC_REQUIRE_EQUAL(parsed.path(), "");

    BC_REQUIRE(parsed.decode("test:///"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "");
    BC_REQUIRE_EQUAL(parsed.path(), "/");

    BC_REQUIRE(parsed.decode("test:/x//"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.path(), "/x//");

    BC_REQUIRE(parsed.decode("ssh://git@github.com:22/path/"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "git@github.com:22");
    BC_REQUIRE_EQUAL(parsed.path(), "/path/");

    // Invalid authority-form
    BC_REQUIRE(parsed.decode("//host"));
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "host");
    BC_REQUIRE(!parsed.has_scheme());
}

BC_AUTO_TEST_CASE(uri__parse__query__expected)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("test:#?"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.fragment(), "?");

    BC_REQUIRE(parsed.decode("test:?&&x=y&z"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.query(), "&&x=y&z");

    auto map = parsed.decode_query();
    BC_REQUIRE(map.cend() != map.find(""));
    BC_REQUIRE(map.cend() != map.find("x"));
    BC_REQUIRE(map.cend() != map.find("z"));
    BC_REQUIRE(map.cend() == map.find("y"));

    BC_REQUIRE_EQUAL(map[""], "");
    BC_REQUIRE_EQUAL(map["x"], "y");
    BC_REQUIRE_EQUAL(map["z"], "");

    // Scheme-less query
    BC_REQUIRE(parsed.decode("/path?key=value"));
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE_EQUAL(parsed.query(), "key=value");
}

BC_AUTO_TEST_CASE(uri__parse__fragment__expected)
{
    uri parsed;
    BC_REQUIRE(parsed.decode("test:?"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(!parsed.has_fragment());

    BC_REQUIRE(parsed.decode("test:#"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.fragment(), "");

    BC_REQUIRE(parsed.decode("test:#?"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE(parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.fragment(), "?");

    // Scheme-less fragment
    BC_REQUIRE(parsed.decode("/path#frag"));
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE_EQUAL(parsed.fragment(), "frag");
}

BC_AUTO_TEST_CASE(uri__encode__positive__expected)
{
    uri parsed;
    parsed.set_scheme("test");
    parsed.set_authority("user@hostname");
    parsed.set_path("/some/path/?/#");
    parsed.set_query("tacos=yummy");
    parsed.set_fragment("good evening");
    BC_REQUIRE_EQUAL(parsed.encoded(), "test://user@hostname/some/path/%3F/%23?tacos=yummy#good%20evening");

    parsed.remove_authority();
    parsed.remove_query();
    parsed.remove_fragment();
    BC_REQUIRE_EQUAL(parsed.encoded(), "test:/some/path/%3F/%23");
}

BC_AUTO_TEST_CASE(uri__encode__without_scheme__expected)
{
    uri parsed;
    parsed.set_path("/path");
    parsed.set_query("key=value");
    BC_REQUIRE_EQUAL(parsed.encoded(), "/path?key=value");
}

BC_AUTO_TEST_CASE(uri__parse__http1_1_targets__expected)
{
    uri parsed;
    BC_REQUIRE(!parsed.decode(""));

    // Origin-form
    BC_REQUIRE(parsed.decode("/path"));
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.path(), "/path");

    BC_REQUIRE(parsed.decode("/path?query=val"));
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_authority());
    BC_REQUIRE(parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE_EQUAL(parsed.query(), "query=val");

    // Absolute-form
    BC_REQUIRE(parsed.decode("test://example.com/path"));
    BC_REQUIRE(parsed.has_scheme());
    BC_REQUIRE_EQUAL(parsed.scheme(), "test");
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "example.com");
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());
    BC_REQUIRE(parsed.decode("/path#frag"));
    BC_REQUIRE_EQUAL(parsed.path(), "/path");
    BC_REQUIRE_EQUAL(parsed.fragment(), "frag");

    // Authority-form
    BC_REQUIRE(parsed.decode("//host:42"));
    BC_REQUIRE(parsed.has_authority());
    BC_REQUIRE_EQUAL(parsed.authority(), "host:42");
    BC_REQUIRE(!parsed.has_scheme());
    BC_REQUIRE(!parsed.has_query());
    BC_REQUIRE(!parsed.has_fragment());

    // Invalid encoding
    BC_REQUIRE(!parsed.decode(":/path"));
    BC_REQUIRE(!parsed.decode("/path%GG"));
}


// setter validation

BC_AUTO_TEST_CASE(uri__set_scheme__invalid__false)
{
    uri instance;
    BC_REQUIRE(instance.decode("bitcoin:test"));
    BC_REQUIRE(!instance.set_scheme("1bitcoin"));
    BC_REQUIRE_EQUAL(instance.scheme(), "bitcoin");
}

BC_AUTO_TEST_CASE(uri__set_scheme__valid__expected)
{
    uri instance;
    BC_REQUIRE(instance.decode("bitcoin:test"));
    BC_REQUIRE(instance.set_scheme("http"));
    BC_REQUIRE_EQUAL(instance.scheme(), "http");
}

BC_AUTO_TEST_CASE(uri__set_authority__invalid__false)
{
    uri instance;
    BC_REQUIRE(instance.decode("bitcoin://host/test"));
    BC_REQUIRE(!instance.set_authority("%zz"));
}

BC_AUTO_TEST_CASE(uri__set_authority__valid__expected)
{
    uri instance;
    BC_REQUIRE(instance.decode("bitcoin://host/test"));
    BC_REQUIRE(instance.set_authority("other"));
    BC_REQUIRE_EQUAL(instance.authority(), "other");
}

BC_AUTO_TEST_CASE(uri__decode__empty__false)
{
    uri instance;
    BC_REQUIRE(!instance.decode(""));
}

BC_AUTO_TEST_CASE(uri__decode__ip_literal_authority__expected)
{
    uri instance;
    BC_REQUIRE(instance.decode("http://[::1]:8080/path"));
    BC_REQUIRE_EQUAL(instance.authority(), "[::1]:8080");
    BC_REQUIRE(instance.decode("http://[1:2:3:4:5:6:7:8]"));
    BC_REQUIRE(instance.decode("http://[1:2:3:4:5:6:7::]"));
    BC_REQUIRE(instance.decode("http://[::ffff:192.168.0.1]"));
    BC_REQUIRE(instance.decode("http://[v1.x:y]"));
    BC_REQUIRE(!instance.decode("http://[1:2:3:4:5:6:7:8:9]"));
    BC_REQUIRE(!instance.decode("http://[1::2::3]"));
    BC_REQUIRE(!instance.decode("http://[::ffff:256.1.1.1]"));
    BC_REQUIRE(!instance.decode("http://[::ffff:01.1.1.1]"));
    BC_REQUIRE(!instance.decode("http://[::1]x"));
    BC_REQUIRE(!instance.decode("http://[v.x]"));
}

BC_AUTO_TEST_CASE(uri__decode__port__expected)
{
    uri instance;
    BC_REQUIRE(instance.decode("http://host:"));
    BC_REQUIRE(instance.decode("http://user:pass@host:42"));
    BC_REQUIRE(!instance.decode("http://host:4x"));
    BC_REQUIRE(!instance.decode("http://a@b@host"));
}

BC_AUTO_TEST_CASE(uri__decode_query__plus__literal)
{
    uri instance;
    BC_REQUIRE(instance.decode("bitcoin:?message=hello+bitcoin&a%2Bb=c%2Bd"));
    BC_REQUIRE_EQUAL(instance.query(), "message=hello+bitcoin&a+b=c+d");

    const auto map = instance.decode_query();
    BC_REQUIRE_EQUAL(map.at("message"), "hello+bitcoin");
    BC_REQUIRE_EQUAL(map.at("a+b"), "c+d");
}

BC_AUTO_TEST_CASE(uri__encode_query__reserved__escaped)
{
    uri instance;
    instance.encode_query({ { "a+b", "c d" }, { "e&f", "g=h" } });
    BC_REQUIRE_EQUAL(instance.encoded(), "?a%2Bb=c%20d&e%26f=g%3Dh");

    instance.encode_query({});
    BC_REQUIRE(!instance.has_query());
}

BC_AUTO_TEST_CASE(uri__encoded__ambiguous_path__disambiguated)
{
    uri instance;
    instance.set_path("a:b/c:d");
    BC_REQUIRE_EQUAL(instance.encoded(), "a%3Ab/c:d");
    BC_REQUIRE(instance.set_scheme("x"));
    BC_REQUIRE_EQUAL(instance.encoded(), "x:a:b/c:d");
    BC_REQUIRE(instance.set_authority("host"));
    BC_REQUIRE_EQUAL(instance.encoded(), "x://host/a:b/c:d");

    instance.remove_authority();
    instance.set_path("//p");
    BC_REQUIRE_EQUAL(instance.encoded(), "x:/.//p");
}

BC_AUTO_TEST_SUITE_END()
