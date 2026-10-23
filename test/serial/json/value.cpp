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

BOOST_AUTO_TEST_SUITE(json_value_tests)

using namespace system::json;

static bool is_not_object(const std::system_error& exception)
{
    return exception.code() == error::json_not_object;
}

// construct

BOOST_AUTO_TEST_CASE(json_value__construct__default__null)
{
    const value instance{};
    BOOST_REQUIRE(instance.is_null());
    BOOST_REQUIRE(instance.kind() == kind::null);
    BOOST_REQUIRE(instance.is_primitive());
    BOOST_REQUIRE(!instance.is_structured());
}

BOOST_AUTO_TEST_CASE(json_value__construct__nullptr__null)
{
    const value instance(nullptr);
    BOOST_REQUIRE(instance.is_null());
}

BOOST_AUTO_TEST_CASE(json_value__construct__bool__bool)
{
    const value instance(true);
    BOOST_REQUIRE(instance.is_bool());
    BOOST_REQUIRE(instance.kind() == kind::bool_);
    BOOST_REQUIRE(instance.as_bool());
}

BOOST_AUTO_TEST_CASE(json_value__construct__signed_integrals__int64)
{
    BOOST_REQUIRE(value(int8_t{ -1 }).is_int64());
    BOOST_REQUIRE(value(int16_t{ -1 }).is_int64());
    BOOST_REQUIRE(value(-1).is_int64());
    BOOST_REQUIRE(value(-1_i64).is_int64());
    BOOST_REQUIRE_EQUAL(value(min_int64).as_int64(), min_int64);
}

BOOST_AUTO_TEST_CASE(json_value__construct__unsigned_integrals__uint64)
{
    BOOST_REQUIRE(value(1_u8).is_uint64());
    BOOST_REQUIRE(value(1_u16).is_uint64());
    BOOST_REQUIRE(value(1_u32).is_uint64());
    BOOST_REQUIRE(value(1_size).is_uint64());
    BOOST_REQUIRE_EQUAL(value(max_uint64).as_uint64(), max_uint64);
}

BOOST_AUTO_TEST_CASE(json_value__construct__double__double)
{
    const value instance(1.5);
    BOOST_REQUIRE(instance.is_double());
    BOOST_REQUIRE(instance.kind() == kind::double_);
    BOOST_REQUIRE_EQUAL(instance.as_double(), 1.5);
}

BOOST_AUTO_TEST_CASE(json_value__construct__texts__string)
{
    const std::string text{ "abc" };
    BOOST_REQUIRE_EQUAL(value("abc").as_string(), "abc");
    BOOST_REQUIRE_EQUAL(value(std::string_view{ text }).as_string(), "abc");
    BOOST_REQUIRE_EQUAL(value(text).as_string(), "abc");
    BOOST_REQUIRE_EQUAL(value(std::string{ text }).as_string(), "abc");
    BOOST_REQUIRE(value(text).kind() == kind::string);
}

BOOST_AUTO_TEST_CASE(json_value__construct__array__array)
{
    const array elements{ 1, 2 };
    const value instance(elements);
    BOOST_REQUIRE(instance.is_array());
    BOOST_REQUIRE(instance.is_structured());
    BOOST_REQUIRE_EQUAL(instance.as_array().size(), 2u);
}

BOOST_AUTO_TEST_CASE(json_value__construct__object__object)
{
    const object members{ { "a", 1 } };
    const value instance(members);
    BOOST_REQUIRE(instance.is_object());
    BOOST_REQUIRE(instance.is_structured());
    BOOST_REQUIRE_EQUAL(instance.as_object().size(), 1u);
}

// initializer list

BOOST_AUTO_TEST_CASE(json_value__initializer__key_value_pairs__object)
{
    const value instance{ { "a", 1 }, { "b", "x" } };
    BOOST_REQUIRE(instance.is_object());
    BOOST_REQUIRE_EQUAL(serialize(instance), R"({"a":1,"b":"x"})");
}

BOOST_AUTO_TEST_CASE(json_value__initializer__mixed__array)
{
    const value instance{ 1, "a", nullptr, true };
    BOOST_REQUIRE(instance.is_array());
    BOOST_REQUIRE_EQUAL(serialize(instance), R"([1,"a",null,true])");
}

BOOST_AUTO_TEST_CASE(json_value__initializer__single_element__element)
{
    const value instance{ "a" };
    BOOST_REQUIRE(instance.is_string());
    BOOST_REQUIRE_EQUAL(instance.as_string(), "a");
}

BOOST_AUTO_TEST_CASE(json_value__initializer__pair_not_string_key__array)
{
    const value instance{ { 1, 2 }, { 3, 4 } };
    BOOST_REQUIRE(instance.is_array());
    BOOST_REQUIRE_EQUAL(serialize(instance), "[[1,2],[3,4]]");
}

BOOST_AUTO_TEST_CASE(json_value__initializer__duplicate_keys__first_retained)
{
    const value instance{ { "a", 1 }, { "a", 2 } };
    BOOST_REQUIRE_EQUAL(serialize(instance), R"({"a":1})");
}

BOOST_AUTO_TEST_CASE(json_value__assign__empty_braces__empty_object)
{
    value instance{ 42 };
    instance = {};
    BOOST_REQUIRE(instance.is_object());
    BOOST_REQUIRE(instance.as_object().empty());
}

BOOST_AUTO_TEST_CASE(json_value__initializer__nested__expected)
{
    const value instance{ { "a", { 1, 2 } }, { "b", { { "c", nullptr } } } };
    BOOST_REQUIRE_EQUAL(serialize(instance), R"({"a":[1,2],"b":{"c":null}})");
}

// assign

BOOST_AUTO_TEST_CASE(json_value__assign__kinds__expected)
{
    value instance{};
    instance = true;
    BOOST_REQUIRE(instance.is_bool());

    instance = 42;
    BOOST_REQUIRE(instance.is_int64());

    instance = 42u;
    BOOST_REQUIRE(instance.is_uint64());

    instance = 4.2;
    BOOST_REQUIRE(instance.is_double());

    instance = "text";
    BOOST_REQUIRE(instance.is_string());

    instance = array{};
    BOOST_REQUIRE(instance.is_array());

    instance = object{};
    BOOST_REQUIRE(instance.is_object());

    instance = nullptr;
    BOOST_REQUIRE(instance.is_null());
}

BOOST_AUTO_TEST_CASE(json_value__assign__own_element__expected)
{
    value instance{ 1, 2 };
    instance = std::move(instance.as_array().front());
    BOOST_REQUIRE_EQUAL(instance.as_int64(), 1);
}

BOOST_AUTO_TEST_CASE(json_value__copy__deep__independent)
{
    value original{ { "a", { 1 } } };
    const value copy{ original };
    original.at("a").as_array().push_back(2);
    BOOST_REQUIRE_EQUAL(serialize(copy), R"({"a":[1]})");
    BOOST_REQUIRE_EQUAL(serialize(original), R"({"a":[1,2]})");
}

// checked access

BOOST_AUTO_TEST_CASE(json_value__as__wrong_kind__throws_expected)
{
    const value instance{};
    BOOST_REQUIRE_THROW(instance.as_bool(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_int64(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_uint64(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_double(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_string(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_array(), std::system_error);
    BOOST_REQUIRE_THROW(instance.as_object(), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_value__as_object__not_object__json_not_object)
{
    const value instance{ 1, 2 };
    BOOST_REQUIRE_EXCEPTION(instance.as_object(), std::system_error, is_not_object);
}

BOOST_AUTO_TEST_CASE(json_value__as__wrong_kind__throws_json_exception)
{
    BOOST_REQUIRE_THROW(value{}.as_bool(), system::json::exception);
}

BOOST_AUTO_TEST_CASE(json_value__as_int64__uint64__throws)
{
    const value instance{ 1u };
    BOOST_REQUIRE_THROW(instance.as_int64(), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_value__as__mutable__modifies)
{
    value instance{ 1 };
    instance.as_int64() = 2;
    BOOST_REQUIRE_EQUAL(instance.as_int64(), 2);
}

// optional access

BOOST_AUTO_TEST_CASE(json_value__if__matching_kind__non_null)
{
    const value instance{ "a" };
    BOOST_REQUIRE(!is_null(instance.if_string()));
    BOOST_REQUIRE(is_null(instance.if_bool()));
    BOOST_REQUIRE(is_null(instance.if_int64()));
    BOOST_REQUIRE(is_null(instance.if_uint64()));
    BOOST_REQUIRE(is_null(instance.if_double()));
    BOOST_REQUIRE(is_null(instance.if_array()));
    BOOST_REQUIRE(is_null(instance.if_object()));
}

// at

BOOST_AUTO_TEST_CASE(json_value__at__key_and_index__expected)
{
    const value instance{ { "a", { 10, 20 } } };
    BOOST_REQUIRE_EQUAL(instance.at("a").at(1).as_int64(), 20);
}

BOOST_AUTO_TEST_CASE(json_value__at__missing__throws_out_of_range)
{
    const value instance{ { "a", { 10, 20 } } };
    BOOST_REQUIRE_THROW(instance.at("b"), std::system_error);
    BOOST_REQUIRE_THROW(instance.at("a").at(2), std::system_error);
    BOOST_REQUIRE_THROW(instance.at(0), std::system_error);
}

// to_number

BOOST_AUTO_TEST_CASE(json_value__to_number__exact__expected)
{
    BOOST_REQUIRE_EQUAL(value(42).to_number<uint8_t>(), 42u);
    BOOST_REQUIRE_EQUAL(value(42u).to_number<int32_t>(), 42);
    BOOST_REQUIRE_EQUAL(value(42.0).to_number<int64_t>(), 42);
    BOOST_REQUIRE_EQUAL(value(-42.0).to_number<int16_t>(), -42);
    BOOST_REQUIRE_EQUAL(value(max_uint64).to_number<double>(), 18446744073709551616.0);
    BOOST_REQUIRE_EQUAL(value(-1).to_number<double>(), -1.0);
    BOOST_REQUIRE_EQUAL(value(0.5).to_number<double>(), 0.5);
    BOOST_REQUIRE_EQUAL(value(min_int64).to_number<int64_t>(), min_int64);
    BOOST_REQUIRE_EQUAL(value(-9223372036854775808.0).to_number<int64_t>(), min_int64);
}

BOOST_AUTO_TEST_CASE(json_value__to_number__inexact__json_not_exact)
{
    code ec{};
    BOOST_REQUIRE_EQUAL(value(256).to_number<uint8_t>(ec), 0u);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(-1).to_number<uint64_t>(ec), 0u);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(max_uint64).to_number<int64_t>(ec), 0);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(0.5).to_number<int32_t>(ec), 0);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(9223372036854775808.0).to_number<int64_t>(ec), 0);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(18446744073709551616.0).to_number<uint64_t>(ec), 0u);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(-1.0).to_number<uint32_t>(ec), 0u);
    BOOST_REQUIRE(ec == error::json_not_exact);

    BOOST_REQUIRE_EQUAL(value(std::nan("")).to_number<int32_t>(ec), 0);
    BOOST_REQUIRE(ec == error::json_not_exact);
}

BOOST_AUTO_TEST_CASE(json_value__to_number__not_number__json_not_number)
{
    code ec{};
    BOOST_REQUIRE_EQUAL(value("1").to_number<int32_t>(ec), 0);
    BOOST_REQUIRE(ec == error::json_not_number);

    BOOST_REQUIRE_EQUAL(value(true).to_number<double>(ec), 0.0);
    BOOST_REQUIRE(ec == error::json_not_number);
}

BOOST_AUTO_TEST_CASE(json_value__to_number__throwing_inexact__throws)
{
    BOOST_REQUIRE_THROW(value(1.5).to_number<int32_t>(), std::system_error);
    BOOST_REQUIRE_THROW(value(nullptr).to_number<double>(), std::system_error);
}

// emplace

BOOST_AUTO_TEST_CASE(json_value__emplace__kinds__default_values)
{
    value instance{ 42 };
    BOOST_REQUIRE(!instance.emplace_bool());
    BOOST_REQUIRE_EQUAL(instance.emplace_int64(), 0);
    BOOST_REQUIRE_EQUAL(instance.emplace_uint64(), 0u);
    BOOST_REQUIRE_EQUAL(instance.emplace_double(), 0.0);
    BOOST_REQUIRE(instance.emplace_string().empty());
    BOOST_REQUIRE(instance.emplace_array().empty());
    BOOST_REQUIRE(instance.emplace_object().empty());
    BOOST_REQUIRE(instance.is_object());

    instance.emplace_null();
    BOOST_REQUIRE(instance.is_null());
}

// equality

BOOST_AUTO_TEST_CASE(json_value__equality__integers_by_value__equal)
{
    BOOST_REQUIRE(value(1) == value(1u));
    BOOST_REQUIRE(value(1u) == value(1));
    BOOST_REQUIRE(value(-1) != value(max_uint64));
    BOOST_REQUIRE(value(max_uint64) != value(-1));
}

BOOST_AUTO_TEST_CASE(json_value__equality__integer_double__not_equal)
{
    BOOST_REQUIRE(value(1) != value(1.0));
}

BOOST_AUTO_TEST_CASE(json_value__equality__kinds__expected)
{
    BOOST_REQUIRE(value{} == value(nullptr));
    BOOST_REQUIRE(value(true) != value(false));
    BOOST_REQUIRE(value("a") == value("a"));
    BOOST_REQUIRE(value("a") != value("b"));
    BOOST_REQUIRE((value{ 1, 2 }) == (value{ 1, 2 }));
    BOOST_REQUIRE((value{ 1, 2 }) != (value{ 2, 1 }));
    BOOST_REQUIRE((value{ { "a", 1 }, { "b", 2 } }) == (value{ { "b", 2 }, { "a", 1 } }));
    BOOST_REQUIRE((value{ { "a", 1 } }) != (value{ { "a", 2 } }));
    BOOST_REQUIRE(value(1) != value("1"));
}

// swap

BOOST_AUTO_TEST_CASE(json_value__swap__values__swapped)
{
    value left{ 1 };
    value right{ "a" };
    swap(left, right);
    BOOST_REQUIRE(left.is_string());
    BOOST_REQUIRE(right.is_int64());
}

BOOST_AUTO_TEST_SUITE_END()
