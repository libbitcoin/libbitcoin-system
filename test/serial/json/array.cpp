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

BOOST_AUTO_TEST_SUITE(json_array_tests)

using namespace system::json;

BOOST_AUTO_TEST_CASE(json_array__construct__default__empty)
{
    const array instance{};
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE_EQUAL(instance.size(), 0u);
    BOOST_REQUIRE(instance.begin() == instance.end());
}

BOOST_AUTO_TEST_CASE(json_array__construct__count__nulls)
{
    const array instance(3);
    BOOST_REQUIRE_EQUAL(instance.size(), 3u);
    BOOST_REQUIRE(instance.front().is_null());
    BOOST_REQUIRE(instance.back().is_null());
}

BOOST_AUTO_TEST_CASE(json_array__construct__count_fill__filled)
{
    const array instance(2, value{ "a" });
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"(["a","a"])");
}

BOOST_AUTO_TEST_CASE(json_array__construct__initializer__elements)
{
    const array instance{ 1, "a", { 2, 3 }, { { "b", true } } };
    BOOST_REQUIRE_EQUAL(instance.size(), 4u);
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([1,"a",[2,3],{"b":true}])");
}

BOOST_AUTO_TEST_CASE(json_array__construct__initializer_key_value_pair__nested_array)
{
    const array instance{ { "a", 1 } };
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([["a",1]])");
}

BOOST_AUTO_TEST_CASE(json_array__copy_move__expected)
{
    array original{ 1, 2 };
    const array copy(original);
    const array moved(std::move(original));
    BOOST_REQUIRE(copy == moved);
    BOOST_REQUIRE_EQUAL(copy.size(), 2u);
}

BOOST_AUTO_TEST_CASE(json_array__assign__initializer__replaced)
{
    array instance{ 1 };
    instance = { 2, 3 };
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), "[2,3]");
}

BOOST_AUTO_TEST_CASE(json_array__at__in_range__expected)
{
    const array instance{ 1, 2 };
    BOOST_REQUIRE_EQUAL(instance.at(1).as_int64(), 2);
    BOOST_REQUIRE_EQUAL(instance[0].as_int64(), 1);
}

BOOST_AUTO_TEST_CASE(json_array__at__out_of_range__throws)
{
    const array instance{ 1, 2 };
    BOOST_REQUIRE_THROW(instance.at(2), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_array__if_contains__expected)
{
    const array instance{ 1 };
    BOOST_REQUIRE(!is_null(instance.if_contains(0)));
    BOOST_REQUIRE(is_null(instance.if_contains(1)));
}

BOOST_AUTO_TEST_CASE(json_array__modifiers__expected)
{
    array instance{};
    instance.reserve(10);
    BOOST_REQUIRE_GE(instance.capacity(), 10u);

    const value one_{ 1 };
    instance.push_back(one_);
    instance.push_back(value{ 2 });
    instance.emplace_back("three");
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([1,2,"three"])");

    instance.insert(instance.cbegin(), value{ 0 });
    instance.emplace(instance.cend(), nullptr);
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([0,1,2,"three",null])");

    instance.erase(std::next(instance.cbegin()));
    instance.pop_back();
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"([0,2,"three"])");

    instance.erase(instance.cbegin(), std::next(instance.cbegin(), 2));
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"(["three"])");

    instance.resize(2);
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"(["three",null])");

    instance.resize(3, value{ false });
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"(["three",null,false])");

    instance.clear();
    BOOST_REQUIRE(instance.empty());
}

BOOST_AUTO_TEST_CASE(json_array__insert__returns_inserted_position)
{
    array instance{ 1, 3 };
    const auto it = instance.insert(std::next(instance.cbegin()), value{ 2 });
    BOOST_REQUIRE_EQUAL(it->as_int64(), 2);
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), "[1,2,3]");
}

BOOST_AUTO_TEST_CASE(json_array__reverse_iteration__expected)
{
    const array instance{ 1, 2, 3 };
    BOOST_REQUIRE_EQUAL(instance.crbegin()->as_int64(), 3);
    BOOST_REQUIRE_EQUAL(std::prev(instance.crend())->as_int64(), 1);
    BOOST_REQUIRE_EQUAL(std::distance(instance.rbegin(), instance.rend()), 3);
}

BOOST_AUTO_TEST_CASE(json_array__equality__expected)
{
    BOOST_REQUIRE((array{ 1, 2 }) == (array{ 1, 2 }));
    BOOST_REQUIRE((array{ 1, 2 }) != (array{ 1 }));
}

BOOST_AUTO_TEST_CASE(json_array__swap__swapped)
{
    array left{ 1 };
    array right{ 1, 2 };
    swap(left, right);
    BOOST_REQUIRE_EQUAL(left.size(), 2u);
    BOOST_REQUIRE_EQUAL(right.size(), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
