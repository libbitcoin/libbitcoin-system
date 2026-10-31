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

BC_AUTO_TEST_SUITE(json_serializer_tests)

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
    { value{ 0.0 }, "0.0" },
    { value{ -0.0 }, "-0.0" },
    { value{ 1.0 }, "1.0" },
    { value{ -1.0 }, "-1.0" },
    { value{ 1.5 }, "1.5" },
    { value{ 0.1 }, "0.1" },
    { value{ 0.3 }, "0.3" },
    { value{ 100.0 }, "100.0" },
    { value{ 1234.5 }, "1234.5" },
    { value{ 2.0 / 3.0 }, "0.6666666666666666" },
    { value{ 1e21 }, "1000000000000000000000.0" },
    { value{ 1e22 }, "10000000000000000000000.0" },
    { value{ 1e-7 }, "0.0000001" },
    { value{ 1e100 }, "10000000000000000159028911097599180468360808563945281389781327557747838772170381060813469985856815104.0" },
    { value{ 1e-100 }, "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001" },
    { value{ 123456789012345678.0 }, "123456789012345680.0" },
    { value{ 9007199254740992.0 }, "9007199254740992.0" },
    { value{ 5e-324 }, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005" },
    { value{ 2.2250738585072014e-308 }, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022250738585072014" },
    { value{ 1.7976931348623157e308 }, "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.0" },
    { value{ 0.30000000000000004 }, "0.30000000000000004" },
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
    { value{ { "k", { { "k", { { "k", { 1.5 } } } } } } }, R"({"k":{"k":{"k":[1.5]}}})" }
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

BC_AUTO_TEST_CASE(json_serializer__serialize__models__expected)
{
    for (const auto& test: models)
        BC_REQUIRE_EQUAL(serialize(test.model), test.expected);
}

BC_AUTO_TEST_CASE(json_serializer__read__models_every_chunk__expected)
{
    for (const auto& test: models)
        BC_REQUIRE_MESSAGE(all_chunks_match(test.model, test.expected), test.expected.substr(0, 80));
}

BC_AUTO_TEST_CASE(json_serializer__read__parsed_round_trip__expected)
{
    const std::string text{ R"({"a":[1,-2,3.5,"x\u0001\"",{"b":null}],"c":true})" };
    const auto model = parse(text);
    BC_REQUIRE_EQUAL(serialize(model), text);
    BC_REQUIRE(all_chunks_match(model, text));
}

BC_AUTO_TEST_CASE(json_serializer__read__escape_split__expected)
{
    const value model{ "\x01" };
    BC_REQUIRE(chunks_match(model, 1, R"("\u0001")"));
    BC_REQUIRE(chunks_match(model, 3, R"("\u0001")"));
    BC_REQUIRE(chunks_match(model, 7, R"("\u0001")"));
}

BC_AUTO_TEST_CASE(json_serializer__read__fills_buffer__expected)
{
    const value model{ { "key", std::string(20, 'x') } };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(8, '\0');
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 8), R"({"key":")");
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 8), "xxxxxxxx");
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 8), "xxxxxxxx");
    BC_REQUIRE(!writer.done());
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 8), R"(xxxx"})");
    BC_REQUIRE(writer.done());
}

BC_AUTO_TEST_CASE(json_serializer__done__exact_fill__done_without_empty_read)
{
    const value model{ "ab" };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(4, '\0');
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 4), R"("ab")");
    BC_REQUIRE(writer.done());
}

BC_AUTO_TEST_CASE(json_serializer__read__after_done__empty)
{
    const value model{ 1 };
    serializer writer{};
    writer.reset(&model);
    std::string buffer(4, '\0');
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 4), "1");
    BC_REQUIRE(writer.done());
    BC_REQUIRE(writer.read(buffer.data(), 4).empty());
}

BC_AUTO_TEST_CASE(json_serializer__reset__null_model__done)
{
    serializer writer{};
    writer.reset(nullptr);
    BC_REQUIRE(writer.done());
}

BC_AUTO_TEST_CASE(json_serializer__default__done)
{
    const serializer writer{};
    BC_REQUIRE(writer.done());
}

BC_AUTO_TEST_CASE(json_serializer__reset__reused__expected)
{
    const value first{ 1, 2 };
    const value second{ { "a", "b" } };
    serializer writer{};
    std::string buffer(3, '\0');
    writer.reset(&first);
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 3), "[1,");

    writer.reset(&second);
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"({"a)");
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"(":")");
    BC_REQUIRE_EQUAL(writer.read(buffer.data(), 3), R"(b"})");
    BC_REQUIRE(writer.done());
}

BC_AUTO_TEST_CASE(json_serializer__stream__value__expected)
{
    std::ostringstream stream{};
    stream << value{ { "a", 1 } };
    BC_REQUIRE_EQUAL(stream.str(), R"({"a":1})");
}

BC_AUTO_TEST_CASE(json_serializer__serialize__object_erase_order__expected)
{
    value model{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    model.as_object().erase("a");
    BC_REQUIRE_EQUAL(serialize(model), R"({"c":3,"b":2})");
}

BC_AUTO_TEST_SUITE_END()
