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

BOOST_AUTO_TEST_SUITE(json_object_tests)

using namespace system::json;

// Member keys that exceed the hash index threshold.
static const std::vector<std::string> many_keys
{
    "k00", "k01", "k02", "k03", "k04", "k05", "k06", "k07", "k08", "k09",
    "k10", "k11", "k12", "k13", "k14", "k15", "k16", "k17", "k18", "k19",
    "k20", "k21", "k22", "k23", "k24", "k25", "k26", "k27", "k28", "k29",
    "k30", "k31", "k32", "k33", "k34", "k35", "k36", "k37", "k38", "k39"
};

static object make_many()
{
    object out{};
    for (const auto& key: many_keys)
        out[key] = key;

    return out;
}

static std::string keys(const object& instance)
{
    std::string out{};
    for (const auto& member: instance)
        out.append(member.key()).append(",");

    return out;
}

BOOST_AUTO_TEST_CASE(json_object__construct__default__empty)
{
    const object instance{};
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE_EQUAL(instance.size(), 0u);
    BOOST_REQUIRE(instance.begin() == instance.end());
}

BOOST_AUTO_TEST_CASE(json_object__construct__initializer__insertion_order)
{
    const object instance{ { "b", 1 }, { "a", 2 }, { "c", 3 } };
    BOOST_REQUIRE_EQUAL(keys(instance), "b,a,c,");
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"({"b":1,"a":2,"c":3})");
}

BOOST_AUTO_TEST_CASE(json_object__construct__initializer_duplicate__first_retained)
{
    const object instance{ { "a", 1 }, { "a", 2 } };
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"({"a":1})");
}

BOOST_AUTO_TEST_CASE(json_object__lookup__present__expected)
{
    const object instance{ { "a", 1 }, { "b", "x" } };
    BOOST_REQUIRE(instance.contains("a"));
    BOOST_REQUIRE_EQUAL(instance.count("b"), 1u);
    BOOST_REQUIRE_EQUAL(instance.at("b").as_string(), "x");
    BOOST_REQUIRE_EQUAL(instance.find("a")->value().as_int64(), 1);
    BOOST_REQUIRE_EQUAL(instance.find("a")->key(), "a");
    BOOST_REQUIRE(!is_null(instance.if_contains("a")));
}

BOOST_AUTO_TEST_CASE(json_object__lookup__absent__expected)
{
    const object instance{ { "a", 1 } };
    BOOST_REQUIRE(!instance.contains("b"));
    BOOST_REQUIRE_EQUAL(instance.count("b"), 0u);
    BOOST_REQUIRE(instance.find("b") == instance.end());
    BOOST_REQUIRE(is_null(instance.if_contains("b")));
    BOOST_REQUIRE_THROW(instance.at("b"), std::system_error);
}

BOOST_AUTO_TEST_CASE(json_object__subscript__absent__inserts_null)
{
    object instance{};
    instance["a"];
    BOOST_REQUIRE(instance.at("a").is_null());

    instance["a"] = 1;
    BOOST_REQUIRE_EQUAL(instance.at("a").as_int64(), 1);
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
}

BOOST_AUTO_TEST_CASE(json_object__emplace__existing__not_replaced)
{
    object instance{ { "a", 1 } };
    const auto result = instance.emplace("a", 2);
    BOOST_REQUIRE(!result.second);
    BOOST_REQUIRE_EQUAL(result.first->value().as_int64(), 1);
}

BOOST_AUTO_TEST_CASE(json_object__emplace__absent__appended)
{
    object instance{ { "a", 1 } };
    const auto result = instance.emplace("b", "x");
    BOOST_REQUIRE(result.second);
    BOOST_REQUIRE_EQUAL(result.first->key(), "b");
    BOOST_REQUIRE_EQUAL(keys(instance), "a,b,");
}

BOOST_AUTO_TEST_CASE(json_object__insert_or_assign__existing__replaced_in_place)
{
    object instance{ { "a", 1 }, { "b", 2 } };
    const auto result = instance.insert_or_assign("a", 3);
    BOOST_REQUIRE(!result.second);
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"({"a":3,"b":2})");
}

BOOST_AUTO_TEST_CASE(json_object__insert__pair_and_list__not_replaced)
{
    object instance{ { "a", 1 } };
    instance.insert(std::pair<std::string, value>{ "a", 2 });
    instance.insert(std::pair<std::string, value>{ "b", 3 });
    instance.insert({ { "c", 4 }, { "a", 5 } });
    BOOST_REQUIRE_EQUAL(serialize(value{ instance }), R"({"a":1,"b":3,"c":4})");
}

BOOST_AUTO_TEST_CASE(json_object__erase__first__last_moved_into_position)
{
    object instance{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    BOOST_REQUIRE_EQUAL(instance.erase("a"), 1u);
    BOOST_REQUIRE_EQUAL(keys(instance), "c,b,");
}

BOOST_AUTO_TEST_CASE(json_object__erase__last__removed)
{
    object instance{ { "a", 1 }, { "b", 2 } };
    const auto it = instance.erase(std::next(instance.cbegin()));
    BOOST_REQUIRE(it == instance.end());
    BOOST_REQUIRE_EQUAL(keys(instance), "a,");
}

BOOST_AUTO_TEST_CASE(json_object__erase__absent__zero)
{
    object instance{ { "a", 1 } };
    BOOST_REQUIRE_EQUAL(instance.erase("b"), 0u);
    BOOST_REQUIRE_EQUAL(instance.stable_erase("b"), 0u);
    BOOST_REQUIRE_EQUAL(instance.size(), 1u);
}

BOOST_AUTO_TEST_CASE(json_object__stable_erase__first__order_retained)
{
    object instance{ { "a", 1 }, { "b", 2 }, { "c", 3 } };
    BOOST_REQUIRE_EQUAL(instance.stable_erase("a"), 1u);
    BOOST_REQUIRE_EQUAL(keys(instance), "b,c,");
}

BOOST_AUTO_TEST_CASE(json_object__indexed__lookup_all__expected)
{
    const auto instance = make_many();
    BOOST_REQUIRE_EQUAL(instance.size(), many_keys.size());
    BOOST_REQUIRE_EQUAL(instance.at("k00").as_string(), "k00");
    BOOST_REQUIRE_EQUAL(instance.at("k17").as_string(), "k17");
    BOOST_REQUIRE_EQUAL(instance.at("k39").as_string(), "k39");
    BOOST_REQUIRE(!instance.contains("k40"));
}

BOOST_AUTO_TEST_CASE(json_object__indexed__every_key__found)
{
    const auto instance = make_many();
    for (const auto& key: many_keys)
        BOOST_REQUIRE_EQUAL(instance.at(key).as_string(), key);
}

BOOST_AUTO_TEST_CASE(json_object__indexed__erase_then_lookup__expected)
{
    auto instance = make_many();
    BOOST_REQUIRE_EQUAL(instance.erase("k05"), 1u);
    BOOST_REQUIRE_EQUAL(instance.stable_erase("k10"), 1u);
    BOOST_REQUIRE(!instance.contains("k05"));
    BOOST_REQUIRE(!instance.contains("k10"));
    BOOST_REQUIRE_EQUAL(instance.at("k39").as_string(), "k39");
    BOOST_REQUIRE_EQUAL(std::next(instance.begin(), 5)->key(), "k39");
    BOOST_REQUIRE_EQUAL(instance.size(), sub1(sub1(many_keys.size())));
}

BOOST_AUTO_TEST_CASE(json_object__indexed__copy__lookup_expected)
{
    const auto original = make_many();
    const object copy{ original };
    BOOST_REQUIRE(copy == original);
    BOOST_REQUIRE_EQUAL(copy.at("k33").as_string(), "k33");
}

BOOST_AUTO_TEST_CASE(json_object__indexed__duplicate_emplace__not_inserted)
{
    auto instance = make_many();
    BOOST_REQUIRE(!instance.emplace("k20", 1).second);
    BOOST_REQUIRE_EQUAL(instance.size(), many_keys.size());
}

BOOST_AUTO_TEST_CASE(json_object__clear__empty)
{
    auto instance = make_many();
    instance.clear();
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE(!instance.contains("k00"));
}

BOOST_AUTO_TEST_CASE(json_object__equality__order_independent)
{
    BOOST_REQUIRE((object{ { "a", 1 }, { "b", 2 } }) == (object{ { "b", 2 }, { "a", 1 } }));
    BOOST_REQUIRE((object{ { "a", 1 } }) != (object{ { "a", 1 }, { "b", 2 } }));
    BOOST_REQUIRE((object{ { "a", 1 } }) != (object{ { "b", 1 } }));
}

BOOST_AUTO_TEST_CASE(json_object__reverse_iteration__expected)
{
    const object instance{ { "a", 1 }, { "b", 2 } };
    BOOST_REQUIRE_EQUAL(instance.crbegin()->key(), "b");
    BOOST_REQUIRE_EQUAL(std::prev(instance.rend())->key(), "a");
}

BOOST_AUTO_TEST_CASE(json_object__key_c_str__expected)
{
    const object instance{ { "abc", 1 } };
    BOOST_REQUIRE_EQUAL(std::string{ instance.begin()->key_c_str() }, "abc");
}

BOOST_AUTO_TEST_CASE(json_object__swap__swapped)
{
    object left{ { "a", 1 } };
    auto right = make_many();
    swap(left, right);
    BOOST_REQUIRE_EQUAL(left.size(), many_keys.size());
    BOOST_REQUIRE_EQUAL(right.at("a").as_int64(), 1);
    BOOST_REQUIRE_EQUAL(left.at("k01").as_string(), "k01");
}

BOOST_AUTO_TEST_SUITE_END()
