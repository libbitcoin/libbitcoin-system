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
#include <map>
#include <optional>
#include <set>
#include <unordered_map>

namespace json_conversion_test {

struct point
{
    int32_t x;
    std::string name;
};

using namespace system::json;

void tag_invoke(value_from_tag, value& to, const point& from) THROWS
{
    to = { { "x", from.x }, { "name", from.name } };
}

point tag_invoke(value_to_tag<point>, const value& from) THROWS
{
    return
    {
        from.at("x").to_number<int32_t>(),
        from.at("name").as_string()
    };
}

} // namespace json_conversion_test

BOOST_AUTO_TEST_SUITE(json_conversion_tests)

using namespace system::json;
using namespace json_conversion_test;

static bool is_size_mismatch(const std::system_error& exception)
{
    return exception.code() == error::json_size_mismatch;
}

static bool is_exhausted_variants(const std::system_error& exception)
{
    return exception.code() == error::json_exhausted_variants;
}

static bool is_not_null(const std::system_error& exception)
{
    return exception.code() == error::json_not_null;
}

static bool is_not_exact(const std::system_error& exception)
{
    return exception.code() == error::json_not_exact;
}

// value_from

BOOST_AUTO_TEST_CASE(json_conversion__value_from__scalars__expected)
{
    BOOST_REQUIRE(value_from(nullptr).is_null());
    BOOST_REQUIRE(value_from(std::monostate{}).is_null());
    BOOST_REQUIRE(value_from(true).as_bool());
    BOOST_REQUIRE_EQUAL(value_from(-1).as_int64(), -1);
    BOOST_REQUIRE_EQUAL(value_from(1_u16).as_uint64(), 1u);
    BOOST_REQUIRE_EQUAL(value_from(0.5).as_double(), 0.5);
    BOOST_REQUIRE_EQUAL(value_from("a").as_string(), "a");
    BOOST_REQUIRE_EQUAL(value_from(std::string_view{ "b" }).as_string(), "b");
    BOOST_REQUIRE_EQUAL(value_from(std::string{ "c" }).as_string(), "c");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__json_types__copied)
{
    const value element{ 1, 2 };
    const array elements{ 3 };
    const object members{ { "a", 4 } };
    BOOST_REQUIRE(value_from(element) == element);
    BOOST_REQUIRE(value_from(elements).as_array() == elements);
    BOOST_REQUIRE(value_from(members).as_object() == members);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__user_type__tag_invoke)
{
    const point instance{ 42, "p" };
    BOOST_REQUIRE_EQUAL(serialize(value_from(instance)), R"({"x":42,"name":"p"})");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__to_target__assigned)
{
    value out{ "replaced" };
    value_from(point{ 1, "q" }, out);
    BOOST_REQUIRE_EQUAL(serialize(out), R"({"x":1,"name":"q"})");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__optional__value_or_null)
{
    BOOST_REQUIRE(value_from(std::optional<int>{}).is_null());
    BOOST_REQUIRE_EQUAL(value_from(std::optional<int>{ 1 }).as_int64(), 1);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__variant__alternative)
{
    using variant = std::variant<int, std::string, point>;
    BOOST_REQUIRE_EQUAL(value_from(variant{ 1 }).as_int64(), 1);
    BOOST_REQUIRE_EQUAL(value_from(variant{ std::string{ "a" } }).as_string(), "a");
    BOOST_REQUIRE_EQUAL(serialize(value_from(variant{ point{ 2, "b" } })), R"({"x":2,"name":"b"})");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__sequences__array)
{
    const std::vector<point> points{ { 1, "a" }, { 2, "b" } };
    const std::array<bool, 2> flags{ true, false };
    const std::set<int> numbers{ 3, 1, 2 };
    BOOST_REQUIRE_EQUAL(serialize(value_from(points)), R"([{"x":1,"name":"a"},{"x":2,"name":"b"}])");
    BOOST_REQUIRE_EQUAL(serialize(value_from(flags)), "[true,false]");
    BOOST_REQUIRE_EQUAL(serialize(value_from(numbers)), "[1,2,3]");
    BOOST_REQUIRE_EQUAL(serialize(value_from(std::vector<int>{})), "[]");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__map__object)
{
    const std::map<std::string, int> map{ { "b", 2 }, { "a", 1 } };
    BOOST_REQUIRE_EQUAL(serialize(value_from(map)), R"({"a":1,"b":2})");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__unordered_map__object)
{
    const std::unordered_map<std::string, int> map{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    const object expected{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    BOOST_REQUIRE(value_from(map).as_object() == expected);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_from__tuples__array)
{
    const std::pair<int, std::string> pair{ 1, "a" };
    const std::tuple<bool, double, std::nullptr_t> tuple{ true, 0.5, nullptr };
    BOOST_REQUIRE_EQUAL(serialize(value_from(pair)), R"([1,"a"])");
    BOOST_REQUIRE_EQUAL(serialize(value_from(tuple)), "[true,5E-1,null]");
}

// value_to

BOOST_AUTO_TEST_CASE(json_conversion__value_to__scalars__expected)
{
    BOOST_REQUIRE(value_to<bool>(value{ true }));
    BOOST_REQUIRE_EQUAL(value_to<int32_t>(value{ -1 }), -1);
    BOOST_REQUIRE_EQUAL(value_to<uint8_t>(value{ 255 }), 255u);
    BOOST_REQUIRE_EQUAL(value_to<double>(value{ 2 }), 2.0);
    BOOST_REQUIRE_EQUAL(value_to<std::string>(value{ "a" }), "a");
    BOOST_REQUIRE_EQUAL(value_to<std::string_view>(value{ "b" }), "b");
    BOOST_REQUIRE(is_null(value_to<std::nullptr_t>(value{})));
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__inexact__throws)
{
    BOOST_REQUIRE_EXCEPTION(value_to<uint8_t>(value{ 256 }), std::system_error, is_not_exact);
    BOOST_REQUIRE_THROW(value_to<std::string>(value{ 1 }), std::system_error);
    BOOST_REQUIRE_THROW(value_to<bool>(value{ 1 }), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__null_not_null__throws)
{
    BOOST_REQUIRE_EXCEPTION(value_to<std::nullptr_t>(value{ 1 }), std::system_error, is_not_null);
    BOOST_REQUIRE_EXCEPTION(value_to<std::monostate>(value{ 1 }), std::system_error, is_not_null);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__json_types__copied)
{
    const value elements{ 1, 2 };
    const value members{ { "a", 1 } };
    BOOST_REQUIRE(value_to<value>(elements) == elements);
    BOOST_REQUIRE(value_to<array>(elements) == elements.as_array());
    BOOST_REQUIRE(value_to<object>(members) == members.as_object());
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__user_type__tag_invoke)
{
    const auto instance = value_to<point>(parse(R"({"x":7,"name":"n"})"));
    BOOST_REQUIRE_EQUAL(instance.x, 7);
    BOOST_REQUIRE_EQUAL(instance.name, "n");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__optional__null_or_value)
{
    BOOST_REQUIRE(!value_to<std::optional<int>>(value{}).has_value());
    BOOST_REQUIRE_EQUAL(value_to<std::optional<int>>(value{ 3 }).value(), 3);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__variant__first_convertible)
{
    using variant = std::variant<std::nullptr_t, int64_t, std::string, point>;
    BOOST_REQUIRE_EQUAL(value_to<variant>(value{}).index(), 0u);
    BOOST_REQUIRE_EQUAL(std::get<int64_t>(value_to<variant>(value{ 1 })), 1);
    BOOST_REQUIRE_EQUAL(std::get<std::string>(value_to<variant>(value{ "a" })), "a");
    BOOST_REQUIRE_EQUAL(std::get<point>(value_to<variant>(parse(R"({"x":1,"name":"a"})"))).name, "a");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__variant_none__exhausted_variants)
{
    using variant = std::variant<int64_t, std::string>;
    BOOST_REQUIRE_EXCEPTION(value_to<variant>(value{ true }), std::system_error, is_exhausted_variants);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__sequences__expected)
{
    const auto numbers = value_to<std::vector<int>>(parse("[1,2,3]"));
    const auto unique = value_to<std::set<int>>(parse("[3,1,3]"));
    const auto points = value_to<std::vector<point>>(parse(R"([{"x":1,"name":"a"}])"));
    BOOST_REQUIRE_EQUAL(numbers.size(), 3u);
    BOOST_REQUIRE_EQUAL(numbers.back(), 3);
    BOOST_REQUIRE_EQUAL(unique.size(), 2u);
    BOOST_REQUIRE_EQUAL(points.front().name, "a");
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__std_array__exact_size)
{
    const auto flags = value_to<std::array<bool, 2>>(parse("[true,false]"));
    BOOST_REQUIRE(flags.front());
    BOOST_REQUIRE(!flags.back());
    BOOST_REQUIRE_EXCEPTION((value_to<std::array<bool, 3>>(parse("[true,false]"))), std::system_error, is_size_mismatch);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__map__expected)
{
    const auto map = value_to<std::map<std::string, int>>(parse(R"({"b":2,"a":1})"));
    const auto unordered = value_to<std::unordered_map<std::string, point>>(parse(R"({"p":{"x":5,"name":"z"}})"));
    BOOST_REQUIRE_EQUAL(map.size(), 2u);
    BOOST_REQUIRE_EQUAL(map.at("a"), 1);
    BOOST_REQUIRE_EQUAL(unordered.at("p").x, 5);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__tuples__expected)
{
    const auto pair = value_to<std::pair<int, std::string>>(parse(R"([1,"a"])"));
    const auto tuple = value_to<std::tuple<bool, double>>(parse("[true,0.5]"));
    BOOST_REQUIRE_EQUAL(pair.first, 1);
    BOOST_REQUIRE_EQUAL(pair.second, "a");
    BOOST_REQUIRE(std::get<0>(tuple));
    BOOST_REQUIRE_EQUAL(std::get<1>(tuple), 0.5);
    BOOST_REQUIRE_EXCEPTION((value_to<std::pair<int, int>>(parse("[1]"))), std::system_error, is_size_mismatch);
}

BOOST_AUTO_TEST_CASE(json_conversion__value_to__wrong_container_kind__throws)
{
    BOOST_REQUIRE_THROW(value_to<std::vector<int>>(parse(R"({"a":1})")), std::system_error);
    BOOST_REQUIRE_THROW((value_to<std::map<std::string, int>>(parse("[1]"))), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_conversion__round_trip__nested__expected)
{
    using type = std::map<std::string, std::vector<std::optional<point>>>;
    const type instance{ { "a", { point{ 1, "x" }, std::nullopt } } };
    const auto json = value_from(instance);
    BOOST_REQUIRE_EQUAL(serialize(json), R"({"a":[{"x":1,"name":"x"},null]})");
    BOOST_REQUIRE_EQUAL(value_to<type>(json).at("a").front()->name, "x");
}

BOOST_AUTO_TEST_SUITE_END()
