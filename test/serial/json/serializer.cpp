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
#include <sstream>

BOOST_AUTO_TEST_SUITE(json_serializer_tests)

using namespace system::json;

struct serialized
{
    value model;
    std::string expected;
};

static const auto nan_ = std::numeric_limits<double>::quiet_NaN();
static const auto infinity = std::numeric_limits<double>::infinity();
static const auto long_text = std::string(5000, 'x');

// Values and their compact serialization (doubles as shortest round trip).
static const std::vector<serialized> models
{
    // primitives
    { value{}, "null" },
    { value{ true }, "true" },
    { value{ false }, "false" },
    { value{ 0 }, "0" },
    { value{ -1 }, "-1" },
    { value{ min_int64 }, "-9223372036854775808" },
    { value{ max_int64 }, "9223372036854775807" },
    { value{ 0u }, "0" },
    { value{ max_uint64 }, "18446744073709551615" },

    // doubles
    { value{ 0.0 }, "0E0" },
    { value{ -0.0 }, "-0E0" },
    { value{ 1.0 }, "1E0" },
    { value{ -1.0 }, "-1E0" },
    { value{ 1.5 }, "1.5E0" },
    { value{ 0.1 }, "1E-1" },
    { value{ 0.3 }, "3E-1" },
    { value{ 100.0 }, "1E2" },
    { value{ 1234.5 }, "1.2345E3" },
    { value{ 2.0 / 3.0 }, "6.666666666666666E-1" },
    { value{ 1e21 }, "1E21" },
    { value{ 1e22 }, "1E22" },
    { value{ 1e-7 }, "1E-7" },
    { value{ 1e100 }, "1E100" },
    { value{ 1e-100 }, "1E-100" },
    { value{ 123456789012345678.0 }, "1.2345678901234568E17" },
    { value{ 9007199254740992.0 }, "9.007199254740992E15" },
    { value{ 5e-324 }, "5E-324" },
    { value{ 2.2250738585072014e-308 }, "2.2250738585072014E-308" },
    { value{ 1.7976931348623157e308 }, "1.7976931348623157E308" },
    { value{ 0.30000000000000004 }, "3.0000000000000004E-1" },
    { value{ nan_ }, "null" },
    { value{ infinity }, "1e99999" },
    { value{ -infinity }, "-1e99999" },

    // strings
    { value{ "" }, R"("")" },
    { value{ "abc" }, R"("abc")" },
    { value{ "\"\\/" }, R"("\"\\/")" },
    { value{ "\b\f\n\r\t" }, R"("\b\f\n\r\t")" },
    { value{ std::string("\x00\x01\x0b\x1f", 4) }, R"("\u0000\u0001\u000b\u001f")" },
    { value{ "\x7f" }, "\"\x7f\"" },
    { value{ "\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80" }, "\"\xc3\xa9\xe2\x82\xac\xf0\x9f\x98\x80\"" },
    { value{ "\xff" }, "\"\xff\"" },
    { value{ long_text }, "\"" + long_text + "\"" },
    { value{ "\n\n\n\n\n\n\n\n\n\n" }, R"("\n\n\n\n\n\n\n\n\n\n")" },

    // structures
    { value(array{}), "[]" },
    { value(object{}), "{}" },
    { value{ 1, "a", nullptr }, R"([1,"a",null])" },
    { value{ { "a", 1 }, { "b\n", { true, false } } }, R"({"a":1,"b\n":[true,false]})" },
    { value{ { "", value(object{}) } }, R"({"":{}})" },
    { value{ value(array{}), value(array{}) }, "[[],[]]" },
    { value{ { "k", { { "k", { { "k", { 1.5 } } } } } } }, R"({"k":{"k":{"k":[1.5E0]}}})" }
};

// Read with the chunk size, true if every read but the last is full and the
// reassembly matches the expectation.
static bool chunks_match(const value& model, size_t chunk,
    const std::string& expected)
{
    serializer writer{};
    writer.reset(&model);
    std::string buffer(chunk, '\0');
    std::string out{};
    while (!writer.done())
    {
        const auto view = writer.read(buffer.data(), chunk);
        if (view.empty() || (view.size() != chunk && !writer.done()))
            return false;

        out.append(view);
    }

    return out == expected;
}

// Every chunk size from one byte through one beyond the text.
static bool all_chunks_match(const value& model, const std::string& expected)
{
    const auto limit = std::min(add1(expected.size()), 600_size);
    for (size_t chunk = one; chunk <= limit; ++chunk)
        if (!chunks_match(model, chunk, expected))
            return false;

    return true;
}

BOOST_AUTO_TEST_CASE(json_serializer__serialize__models__expected)
{
    for (const auto& test: models)
        BOOST_REQUIRE_EQUAL(serialize(test.model), test.expected);
}

BOOST_AUTO_TEST_CASE(json_serializer__read__models_every_chunk__expected)
{
    for (const auto& test: models)
        BOOST_REQUIRE_MESSAGE(all_chunks_match(test.model, test.expected), test.expected.substr(0, 80));
}

BOOST_AUTO_TEST_CASE(json_serializer__read__parsed_round_trip__expected)
{
    const std::string text{ R"({"a":[1,-2,3.5E0,"x\u0001\"",{"b":null}],"c":true})" };
    const auto model = parse(text);
    BOOST_REQUIRE_EQUAL(serialize(model), text);
    BOOST_REQUIRE(all_chunks_match(model, text));
}

BOOST_AUTO_TEST_CASE(json_serializer__read__escape_split__expected)
{
    const value model{ "\x01" };
    BOOST_REQUIRE(chunks_match(model, 1, R"("\u0001")"));
    BOOST_REQUIRE(chunks_match(model, 3, R"("\u0001")"));
    BOOST_REQUIRE(chunks_match(model, 7, R"("\u0001")"));
}

BOOST_AUTO_TEST_CASE(json_serializer__read__fills_buffer__expected)
{
    const value model{ { "key", std::string(20, 'x') } };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(8, '\0');
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 8), R"({"key":")");
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 8), "xxxxxxxx");
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 8), "xxxxxxxx");
    BOOST_REQUIRE(!writer.done());
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 8), R"(xxxx"})");
    BOOST_REQUIRE(writer.done());
}

BOOST_AUTO_TEST_CASE(json_serializer__done__exact_fill__done_without_empty_read)
{
    const value model{ "ab" };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(4, '\0');
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 4), R"("ab")");
    BOOST_REQUIRE(writer.done());
}

BOOST_AUTO_TEST_CASE(json_serializer__read__after_done__empty)
{
    const value model{ 1 };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(4, '\0');
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 4), "1");
    BOOST_REQUIRE(writer.done());
    BOOST_REQUIRE(writer.read(buffer.data(), 4).empty());
}

BOOST_AUTO_TEST_CASE(json_serializer__reset__null_model__done)
{
    serializer writer{};
    writer.reset(nullptr);
    BOOST_REQUIRE(writer.done());
}

BOOST_AUTO_TEST_CASE(json_serializer__default__done)
{
    const serializer writer{};
    BOOST_REQUIRE(writer.done());
}

BOOST_AUTO_TEST_CASE(json_serializer__reset__reused__expected)
{
    const value first{ 1, 2 };
    const value second{ { "a", "b" } };
    serializer writer{};
    std::string buffer(3, '\0');
    writer.reset(&first);
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 3), "[1,");

    writer.reset(&second);
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"({"a)");
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"(":")");
    BOOST_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"(b"})");
    BOOST_REQUIRE(writer.done());
}

BOOST_AUTO_TEST_CASE(json_serializer__stream__value__expected)
{
    std::ostringstream stream{};
    stream << value{ { "a", 1 } };
    BOOST_REQUIRE_EQUAL(stream.str(), R"({"a":1})");
}

BOOST_AUTO_TEST_CASE(json_serializer__serialize__object_erase_order__expected)
{
    value model{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    model.as_object().erase("a");
    BOOST_REQUIRE_EQUAL(serialize(model), R"({"c":3,"b":2})");
}

BOOST_AUTO_TEST_SUITE_END()
