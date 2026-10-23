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

BOOST_AUTO_TEST_SUITE(json_value_ref_tests)

using namespace system::json;

BOOST_AUTO_TEST_CASE(json_value_ref__is_key_value_pair__string_first_pair__true)
{
    BOOST_REQUIRE((value_ref{ "a", 1 }.is_key_value_pair()));
    BOOST_REQUIRE_EQUAL((value_ref{ "a", 1 }.make_value().as_array().size()), 2u);
}

BOOST_AUTO_TEST_CASE(json_value_ref__is_key_value_pair__not_pair__false)
{
    BOOST_REQUIRE(!value_ref("a").is_key_value_pair());
    BOOST_REQUIRE(!(value_ref{ 1, "a" }.is_key_value_pair()));
    BOOST_REQUIRE(!(value_ref{ "a", 1, 2 }.is_key_value_pair()));
}

BOOST_AUTO_TEST_CASE(json_value_ref__maybe_object__expected)
{
    BOOST_REQUIRE(value_ref::maybe_object({}));
    BOOST_REQUIRE(value_ref::maybe_object({ { "a", 1 }, { "b", 2 } }));
    BOOST_REQUIRE(!value_ref::maybe_object({ { "a", 1 }, 2 }));
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_value__scalars__expected)
{
    BOOST_REQUIRE(value_ref(nullptr).make_value().is_null());
    BOOST_REQUIRE(value_ref(true).make_value().as_bool());
    BOOST_REQUIRE_EQUAL(value_ref(-1).make_value().as_int64(), -1);
    BOOST_REQUIRE_EQUAL(value_ref(1u).make_value().as_uint64(), 1u);
    BOOST_REQUIRE_EQUAL(value_ref(1.5).make_value().as_double(), 1.5);
    BOOST_REQUIRE_EQUAL(value_ref("a").make_value().as_string(), "a");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_value__strings__expected)
{
    const std::string text{ "abc" };
    BOOST_REQUIRE_EQUAL(value_ref(text).make_value().as_string(), "abc");
    BOOST_REQUIRE_EQUAL(value_ref(std::string_view{ text }).make_value().as_string(), "abc");
    BOOST_REQUIRE_EQUAL(value_ref(std::string{ text }).make_value().as_string(), "abc");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_value__moved_string_key__object)
{
    const value instance{ { std::string{ "key" }, 1 } };
    BOOST_REQUIRE_EQUAL(serialize(instance), R"({"key":1})");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_value__values__copied_and_moved)
{
    const value copied{ "a" };
    value moved{ "b" };
    const value instance{ copied, std::move(moved) };
    BOOST_REQUIRE_EQUAL(serialize(instance), R"(["a","b"])");
    BOOST_REQUIRE_EQUAL(copied.as_string(), "a");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_value__converted__expected)
{
    const std::vector<int> numbers{ 1, 2 };
    const std::optional<int> empty{};
    const value instance{ { "numbers", numbers }, { "empty", empty }, { "moved", std::vector<std::string>{ "x" } } };
    BOOST_REQUIRE_EQUAL(serialize(instance), R"({"numbers":[1,2],"empty":null,"moved":["x"]})");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_array__key_value_pairs__array)
{
    const auto instance = value_ref::make_array({ { "a", 1 } });
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([["a",1]])");
}

BOOST_AUTO_TEST_CASE(json_value_ref__make_object__pairs__object)
{
    const auto instance = value_ref::make_object({ { "a", 1 }, { "b", { 2 } } });
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"({"a":1,"b":[2]})");
}

BOOST_AUTO_TEST_SUITE_END()
