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

BOOST_AUTO_TEST_SUITE(json_parser_tests)

using namespace system::json;

struct accepted
{
    std::string text;
    std::string expected;
};

struct rejected
{
    std::string text;
    error::json_error_t expected;
};

// Valid texts and their compact serialization.
static const std::vector<accepted> valid
{
    // literals
    { "null", "null" },
    { "true", "true" },
    { "false", "false" },
    { " \t\r\n[ \t\r\n] \t\r\n", "[]" },

    // numbers
    { "[0]", "[0]" },
    { "[-0]", "[0]" },
    { "[-0.0]", "[-0E0]" },
    { "[1]", "[1]" },
    { "[-1]", "[-1]" },
    { "[123]", "[123]" },
    { "[9223372036854775807]", "[9223372036854775807]" },
    { "[9223372036854775808]", "[9223372036854775808]" },
    { "[18446744073709551615]", "[18446744073709551615]" },
    { "[18446744073709551616]", "[1.8446744073709552E19]" },
    { "[-9223372036854775808]", "[-9223372036854775808]" },
    { "[-9223372036854775809]", "[-9.223372036854776E18]" },
    { "[1.5]", "[1.5E0]" },
    { "[1e2]", "[1E2]" },
    { "[1E+2]", "[1E2]" },
    { "[1e-2]", "[1E-2]" },
    { "[0e1]", "[0E0]" },
    { "[0.1]", "[1E-1]" },
    { "[123.456e78]", "[1.23456E80]" },
    { "[1.7976931348623157e308]", "[1.7976931348623157E308]" },
    { "[4.9e-324]", "[5E-324]" },
    { "[0.30000000000000004]", "[3.0000000000000004E-1]" },
    { "[1e-400]", "[0E0]" },
    { "[-1e-400]", "[-0E0]" },
    { "[0.0000001e-330]", "[0E0]" },
    { "[-0.000000000000000000000000000001]", "[-1E-30]" },
    { "[1.0,2.0]", "[1E0,2E0]" },

    // strings
    { R"("")", R"("")" },
    { R"("a")", R"("a")" },
    { R"(["\"\\\/\b\f\n\r\t"])", R"(["\"\\/\b\f\n\r\t"])" },
    { R"(["\u0000"])", R"(["\u0000"])" },
    { R"(["\u001f\u007f"])", "[\"\\u001f\x7f\"]" },
    { R"(["\u0041\u00e9"])", "[\"A\xc3\xa9\"]" },
    { R"(["\u20AC"])", "[\"\xe2\x82\xac\"]" },
    { R"(["\uD834\uDD1E"])", "[\"\xf0\x9d\x84\x9e\"]" },
    { R"(["\udbff\udfff"])", "[\"\xf4\x8f\xbf\xbf\"]" },
    { R"(["\uFFFF"])", "[\"\xef\xbf\xbf\"]" },
    { "[\"\xc2\x80\xdf\xbf\"]", "[\"\xc2\x80\xdf\xbf\"]" },
    { "[\"\xe0\xa0\x80\xed\x9f\xbf\xee\x80\x80\"]", "[\"\xe0\xa0\x80\xed\x9f\xbf\xee\x80\x80\"]" },
    { "[\"\xf0\x90\x80\x80\xf4\x8f\xbf\xbf\"]", "[\"\xf0\x90\x80\x80\xf4\x8f\xbf\xbf\"]" },
    { "[\"\x7f\"]", "[\"\x7f\"]" },

    // structures
    { "{}", "{}" },
    { R"({"":0})", R"({"":0})" },
    { R"({ "a" : 1 , "b" : [ ] })", R"({"a":1,"b":[]})" },
    { R"({"a":"b","a":"c"})", R"({"a":"c"})" },
    { R"({"a":1,"b":2,"a":3})", R"({"a":3,"b":2})" },
    { R"({"a":{"b":{"c":[1,{"d":true}]}}})", R"({"a":{"b":{"c":[1,{"d":true}]}}})" },
    { R"([[],[[]],{}])", R"([[],[[]],{}])" },
    { "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]", "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]" }
};

// Invalid texts (complete) and their parse error.
static const std::vector<rejected> invalid
{
    // structure
    { "", error::json_incomplete },
    { " ", error::json_incomplete },
    { "[", error::json_incomplete },
    { "{", error::json_incomplete },
    { R"({"a")", error::json_incomplete },
    { R"({"a":)", error::json_incomplete },
    { R"(["a)", error::json_incomplete },
    { "[1,", error::json_incomplete },
    { "nul", error::json_incomplete },
    { "]", error::json_syntax },
    { "}", error::json_syntax },
    { "[1,]", error::json_syntax },
    { "[,1]", error::json_syntax },
    { "[1,,2]", error::json_syntax },
    { "[1 2]", error::json_syntax },
    { "[1:2]", error::json_syntax },
    { R"({"a":1,})", error::json_syntax },
    { R"({"a" 1})", error::json_syntax },
    { R"({"a"::1})", error::json_syntax },
    { R"({a:1})", error::json_syntax },
    { R"({'a':1})", error::json_syntax },
    { R"({1:1})", error::json_syntax },
    { R"({"a":1]})", error::json_syntax },
    { R"([1})", error::json_syntax },
    { "[1]]", error::json_extra_data },
    { "[1] x", error::json_extra_data },
    { "1 2", error::json_extra_data },
    { "true false", error::json_extra_data },
    { R"({"a":1}/**/)", error::json_extra_data },
    { R"({"a":/*c*/1})", error::json_syntax },
    { "\xef\xbb\xbf{}", error::json_syntax },

    // literals
    { "tru", error::json_incomplete },
    { "nulll", error::json_extra_data },
    { "TRUE", error::json_syntax },
    { "Null", error::json_syntax },
    { "[fals]", error::json_syntax },
    { "[NaN]", error::json_syntax },
    { "[Infinity]", error::json_syntax },
    { "[-Infinity]", error::json_syntax },

    // numbers
    { "[+1]", error::json_syntax },
    { "[01]", error::json_syntax },
    { "[-01]", error::json_syntax },
    { "[-]", error::json_syntax },
    { "[- 1]", error::json_syntax },
    { "[.1]", error::json_syntax },
    { "[1.]", error::json_syntax },
    { "[1.e1]", error::json_syntax },
    { "[1e]", error::json_syntax },
    { "[1e+]", error::json_syntax },
    { "[1eE2]", error::json_syntax },
    { "[0x1]", error::json_syntax },
    { "[1.2.3]", error::json_syntax },
    { "[1a]", error::json_syntax },
    { "[1e400]", error::json_exponent_overflow },
    { "[-1e400]", error::json_exponent_overflow },
    { "1e99999", error::json_exponent_overflow },

    // strings
    { "[\"\x01\"]", error::json_syntax },
    { "[\"\n\"]", error::json_syntax },
    { "[\"\t\"]", error::json_syntax },
    { R"(["\a"])", error::json_invalid_escape },
    { R"(["\x00"])", error::json_invalid_escape },
    { R"(["\U0041"])", error::json_invalid_escape },
    { R"(["\u00G0"])", error::json_expected_hex_digit },
    { R"(["\u00"])", error::json_expected_hex_digit },
    { R"(["\uDC00"])", error::json_illegal_leading_surrogate },
    { R"(["\uDFFF\uD800"])", error::json_illegal_leading_surrogate },
    { R"(["\uD800"])", error::json_expected_utf16_escape },
    { R"(["\uD800x"])", error::json_expected_utf16_escape },
    { R"(["\uD800\n"])", error::json_expected_utf16_escape },
    { R"(["\uD800\u0041"])", error::json_illegal_trailing_surrogate },
    { R"(["\uD800\uD800"])", error::json_illegal_trailing_surrogate },
    { R"(["\uD800\uDC0"])", error::json_expected_hex_digit },

    // utf8
    { "[\"\x80\"]", error::json_invalid_utf8 },
    { "[\"\xbf\"]", error::json_invalid_utf8 },
    { "[\"\xc0\x80\"]", error::json_invalid_utf8 },
    { "[\"\xc1\xbf\"]", error::json_invalid_utf8 },
    { "[\"\xc3\"]", error::json_invalid_utf8 },
    { "[\"\xc3\x28\"]", error::json_invalid_utf8 },
    { "[\"\xe0\x80\x80\"]", error::json_invalid_utf8 },
    { "[\"\xe0\x9f\xbf\"]", error::json_invalid_utf8 },
    { "[\"\xe2\x82\"]", error::json_invalid_utf8 },
    { "[\"\xed\xa0\x80\"]", error::json_invalid_utf8 },
    { "[\"\xed\xbf\xbf\"]", error::json_invalid_utf8 },
    { "[\"\xf0\x80\x80\x80\"]", error::json_invalid_utf8 },
    { "[\"\xf0\x8f\xbf\xbf\"]", error::json_invalid_utf8 },
    { "[\"\xf4\x90\x80\x80\"]", error::json_invalid_utf8 },
    { "[\"\xf5\x80\x80\x80\"]", error::json_invalid_utf8 },
    { "[\"\xf8\x88\x80\x80\x80\"]", error::json_invalid_utf8 },
    { "[\"\xfe\"]", error::json_invalid_utf8 },
    { "[\"\xff\"]", error::json_invalid_utf8 },
    { "[\xc3\xa9]", error::json_syntax },
    { "\"\xe2\x82\xac", error::json_incomplete },

    // depth
    { "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]", error::json_too_deep },
    { R"([{"a":[{"b":[{"c":[{"d":[{"e":[{"f":[{"g":[{"h":[{"i":[{"j":[{"k":[{"l":[{"m":[{"n":[{"o":[{"p":[{}]}]}]}]}]}]}]}]}]}]}]}]}]}]}]}])", error::json_too_deep }
};

// Parse the text split into two writes at every position, matching whole.
static bool splits_match(const std::string& text)
{
    const auto whole = serialize(parse(text));
    for (size_t split = 1; split < text.size(); ++split)
    {
        code ec{};
        parser reader{};
        const auto first = reader.write_some(text.data(), split, ec);
        if (ec || first != split)
            return false;

        const auto rest = std::next(text.data(), split);
        const auto second = reader.write(rest, text.size() - split, ec);
        if (ec || second != text.size() - split)
            return false;

        reader.finish(ec);
        if (ec || serialize(reader.release()) != whole)
            return false;
    }

    return true;
}

// Parse the text one character per write, matching whole.
static bool bytewise_match(const std::string& text)
{
    code ec{};
    parser reader{};
    for (const auto character: text)
        if (reader.write_some(&character, one, ec) != one || ec)
            return false;

    reader.finish(ec);
    return !ec && serialize(reader.release()) == serialize(parse(text));
}

// Error of the text split into two writes at every position, matching whole.
static bool split_errors_match(const std::string& text,
    error::json_error_t expected)
{
    for (size_t split = 1; split < text.size(); ++split)
    {
        code ec{};
        parser reader{};
        const auto first = reader.write_some(text.data(), split, ec);
        if (!ec)
        {
            const auto rest = std::next(text.data(), first);
            reader.write(rest, text.size() - first, ec);
        }

        if (!ec)
            reader.finish(ec);

        if (ec != expected)
            return false;
    }

    return true;
}

// parse

BOOST_AUTO_TEST_CASE(json_parser__parse__valid__expected)
{
    for (const auto& test: valid)
    {
        code ec{};
        const auto out = parse(test.text, ec);
        BOOST_REQUIRE_MESSAGE(!ec, test.text + " : " + ec.message());
        BOOST_REQUIRE_EQUAL(serialize(out), test.expected);
    }
}

BOOST_AUTO_TEST_CASE(json_parser__parse__invalid__expected_error)
{
    for (const auto& test: invalid)
    {
        code ec{};
        const auto out = parse(test.text, ec);
        BOOST_REQUIRE_MESSAGE(ec == test.expected, test.text + " : " + ec.message());
        BOOST_REQUIRE(out.is_null());
    }
}

BOOST_AUTO_TEST_CASE(json_parser__parse__valid_every_split__expected)
{
    for (const auto& test: valid)
        BOOST_REQUIRE_MESSAGE(splits_match(test.text), test.text);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__valid_bytewise__expected)
{
    for (const auto& test: valid)
        BOOST_REQUIRE_MESSAGE(bytewise_match(test.text), test.text);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__invalid_every_split__expected_error)
{
    for (const auto& test: invalid)
        BOOST_REQUIRE_MESSAGE(split_errors_match(test.text, test.expected), test.text);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__throwing_invalid__throws)
{
    BOOST_REQUIRE_THROW(parse("[1,]"), std::system_error);
    BOOST_REQUIRE_EQUAL(parse("[1]").as_array().size(), 1u);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__kinds__expected)
{
    const auto out = parse(R"([-1,1,18446744073709551615,1.0,"a",true,null,{},[]])");
    const auto& values = out.as_array();
    BOOST_REQUIRE(values.at(0).is_int64());
    BOOST_REQUIRE(values.at(1).is_int64());
    BOOST_REQUIRE(values.at(2).is_uint64());
    BOOST_REQUIRE(values.at(3).is_double());
    BOOST_REQUIRE(values.at(4).is_string());
    BOOST_REQUIRE(values.at(5).is_bool());
    BOOST_REQUIRE(values.at(6).is_null());
    BOOST_REQUIRE(values.at(7).is_object());
    BOOST_REQUIRE(values.at(8).is_array());
}

BOOST_AUTO_TEST_CASE(json_parser__parse__duplicate_key__last_value_first_position)
{
    const auto out = parse(R"({"a":1,"b":2,"a":3})");
    BOOST_REQUIRE_EQUAL(out.as_object().begin()->key(), "a");
    BOOST_REQUIRE_EQUAL(out.at("a").as_int64(), 3);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__embedded_null__retained)
{
    const std::string text{ "[\"a\\u0000b\"]" };
    const auto out = parse(text);
    BOOST_REQUIRE_EQUAL(out.at(0).as_string().size(), 3u);
    BOOST_REQUIRE_EQUAL(out.at(0).as_string(), std::string("a\0b", 3));
}

BOOST_AUTO_TEST_CASE(json_parser__parse__raw_null_byte__syntax)
{
    const std::string text{ "[\"a\0b\"]", 7 };
    code ec{};
    parse(text, ec);
    BOOST_REQUIRE(ec == error::json_syntax);
}

BOOST_AUTO_TEST_CASE(json_parser__parse__trailing_null_byte__extra_data)
{
    const std::string text{ "[1]\0", 4 };
    code ec{};
    parse(text, ec);
    BOOST_REQUIRE(ec == error::json_extra_data);
}

// write_some

BOOST_AUTO_TEST_CASE(json_parser__write_some__document_then_more__stops_after_whitespace)
{
    const std::string text{ "{\"a\":1} \r\n{\"b\":2}" };
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some(text.data(), text.size(), ec), 10u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
    BOOST_REQUIRE_EQUAL(serialize(reader.release()), R"({"a":1})");
    BOOST_REQUIRE(!reader.done());

    const auto rest = std::next(text.data(), 10);
    BOOST_REQUIRE_EQUAL(reader.write_some(rest, text.size() - 10u, ec), 7u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
    BOOST_REQUIRE_EQUAL(serialize(reader.release()), R"({"b":2})");
}

BOOST_AUTO_TEST_CASE(json_parser__write_some__done_then_whitespace__consumed)
{
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some("[]", 2, ec), 2u);
    BOOST_REQUIRE(reader.done());

    BOOST_REQUIRE_EQUAL(reader.write_some(" \n", 2, ec), 2u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
}

BOOST_AUTO_TEST_CASE(json_parser__write_some__done_then_character__not_consumed)
{
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some("[] x", 4, ec), 3u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
}

BOOST_AUTO_TEST_CASE(json_parser__write_some__top_level_number__not_done_until_delimiter)
{
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some("123", 3, ec), 3u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(!reader.done());

    BOOST_REQUIRE_EQUAL(reader.write_some("4\n", 2, ec), 2u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
    BOOST_REQUIRE_EQUAL(reader.release().as_int64(), 1234);
}

BOOST_AUTO_TEST_CASE(json_parser__finish__top_level_number__done)
{
    code ec{};
    parser reader{};
    reader.write_some("-1.5", 4, ec);
    BOOST_REQUIRE(!reader.done());

    reader.finish(ec);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
    BOOST_REQUIRE_EQUAL(reader.release().as_double(), -1.5);
}

BOOST_AUTO_TEST_CASE(json_parser__finish__incomplete__json_incomplete)
{
    code ec{};
    parser reader{};
    reader.write_some(R"({"a":)", 5, ec);
    BOOST_REQUIRE(!ec);

    reader.finish(ec);
    BOOST_REQUIRE(ec == error::json_incomplete);
}

BOOST_AUTO_TEST_CASE(json_parser__write_some__error__position_and_sticky)
{
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some("[1,]", 4, ec), 3u);
    BOOST_REQUIRE(ec == error::json_syntax);

    BOOST_REQUIRE_EQUAL(reader.write_some("1", 1, ec), 0u);
    BOOST_REQUIRE(ec == error::json_syntax);
    BOOST_REQUIRE(!reader.done());
    BOOST_REQUIRE(reader.release().is_null());
}

BOOST_AUTO_TEST_CASE(json_parser__reset__after_error__parses)
{
    code ec{};
    parser reader{};
    reader.write_some("[1,]", 4, ec);
    BOOST_REQUIRE(ec);

    reader.reset();
    reader.write_some("[1]", 3, ec);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(reader.done());
}

BOOST_AUTO_TEST_CASE(json_parser__write_some__empty__nothing)
{
    code ec{};
    parser reader{};
    BOOST_REQUIRE_EQUAL(reader.write_some("", 0, ec), 0u);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(!reader.done());
}

BOOST_AUTO_TEST_CASE(json_parser__release__not_done__null)
{
    code ec{};
    parser reader{};
    reader.write_some("[1", 2, ec);
    BOOST_REQUIRE(reader.release().is_null());
}

// limits

BOOST_AUTO_TEST_CASE(json_parser__limits__max_depth__expected)
{
    const parse_options options{ .max_depth = 2 };
    code ec{};
    parse("[[1]]", ec, options);
    BOOST_REQUIRE(!ec);

    parse("[[[1]]]", ec, options);
    BOOST_REQUIRE(ec == error::json_too_deep);
}

BOOST_AUTO_TEST_CASE(json_parser__limits__max_string__expected)
{
    const parse_options options{ .max_string = 3 };
    code ec{};
    parse(R"(["abc"])", ec, options);
    BOOST_REQUIRE(!ec);

    parse(R"(["abcd"])", ec, options);
    BOOST_REQUIRE(ec == error::json_string_too_large);

    parse(R"(["ab\u00e9"])", ec, options);
    BOOST_REQUIRE(ec == error::json_string_too_large);
}

BOOST_AUTO_TEST_CASE(json_parser__limits__max_key__expected)
{
    const parse_options options{ .max_key = 1 };
    code ec{};
    parse(R"({"a":"long value"})", ec, options);
    BOOST_REQUIRE(!ec);

    parse(R"({"ab":1})", ec, options);
    BOOST_REQUIRE(ec == error::json_key_too_large);
}

BOOST_AUTO_TEST_CASE(json_parser__limits__max_array__expected)
{
    const parse_options options{ .max_array = 2 };
    code ec{};
    parse("[1,2]", ec, options);
    BOOST_REQUIRE(!ec);

    parse("[1,2,3]", ec, options);
    BOOST_REQUIRE(ec == error::json_array_too_large);
}

BOOST_AUTO_TEST_CASE(json_parser__limits__max_object__expected)
{
    const parse_options options{ .max_object = 1 };
    code ec{};
    parse(R"({"a":1})", ec, options);
    BOOST_REQUIRE(!ec);

    parse(R"({"a":1,"b":2})", ec, options);
    BOOST_REQUIRE(ec == error::json_object_too_large);
}

BOOST_AUTO_TEST_CASE(json_parser__limits__max_number__expected)
{
    const parse_options options{ .max_number = 3 };
    code ec{};
    parse("[123]", ec, options);
    BOOST_REQUIRE(!ec);

    parse("[1234]", ec, options);
    BOOST_REQUIRE(ec == error::json_number_too_large);
}

BOOST_AUTO_TEST_SUITE_END()
