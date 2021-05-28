/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"
#include <cstdint>
#include <vector>

BOOST_AUTO_TEST_SUITE(collection_tests)

// binary_search

const auto not_found = -1;

// Collection must be iterable and members implement lexical compare (<, >).

BOOST_AUTO_TEST_CASE(limits__binary_search__empty__not_found)
{
    const std::string empty = "";
    const auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(empty, value), not_found);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__one_element_unmatched__not_found)
{
    const std::string single = "a";
    const auto value = 'b';
    BOOST_REQUIRE_EQUAL(binary_search(single, value), not_found);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__one_element_match__0)
{
    const std::string single = "a";
    const auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(single, value), 0);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__two_elements_match_first__0)
{
    const std::string sorted = "ab";
    const auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 0);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__two_elements_match_second__1)
{
    const std::string sorted = "ab";
    const auto value = 'b';
    auto result = binary_search(sorted, value);
    BOOST_REQUIRE_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__three_elements_match_second__1)
{
    const std::string sorted = "abc";
    const auto value = 'b';
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 1);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__three_various_elements__unmatched__not_found)
{
    const string_list sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const std::string value = "bcd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), not_found);
}

BOOST_AUTO_TEST_CASE(limits__binary_search__three_various_elements__matched__found)
{
    const string_list sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const std::string value = "cfdd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 2);
}

// These generate aborts because of the sort assertion in the implementation.

////BOOST_AUTO_TEST_CASE(limits__binary_search__unsorted__contained__unlucky)
////{
////    const string_list unsorted{ { "z", "y", "x" } };
////    const std::string value = "z";
////    BOOST_REQUIRE_EQUAL(binary_search(unsorted, value), not_found);
////}
////
////BOOST_AUTO_TEST_CASE(limits__binary_search__unsorted__contained__lucky)
////{
////    const data_chunk unsorted{ { 'z', 'a', 'x' } };
////    const auto value = 'x';
////    BOOST_REQUIRE_EQUAL(binary_search(unsorted, value), 2);
////}

// cast

BOOST_AUTO_TEST_CASE(collection__cast__empty__same)
{
    data_chunk parameter;
    const auto result = cast<uint8_t>(parameter);
    BOOST_REQUIRE(parameter.empty());
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(collection__cast__one_element__same)
{
    data_chunk parameter{ 42 };
    const auto result = cast<uint8_t>(parameter);
    BOOST_REQUIRE_EQUAL(parameter.size(), result.size());
    BOOST_REQUIRE_EQUAL(result[0], parameter[0]);
}

BOOST_AUTO_TEST_CASE(collection__cast__distinct_types__same)
{
    data_chunk parameter{ 42, 24 };
    const auto result = cast<uint32_t>(parameter);
    BOOST_REQUIRE_EQUAL(parameter.size(), result.size());
    BOOST_REQUIRE_EQUAL(result[0], parameter[0]);
    BOOST_REQUIRE_EQUAL(result[1], parameter[1]);
}

// distinct

BOOST_AUTO_TEST_CASE(collection__distinct__empty__same)
{
    data_chunk set{};
    set = distinct(std::move(set));
    BOOST_REQUIRE(set.empty());
}

BOOST_AUTO_TEST_CASE(collection__distinct__single__match)
{
    const uint8_t expected = 42;
    data_chunk set{ expected };
    set = distinct(std::move(set));
    BOOST_REQUIRE_EQUAL(set.size(), 1u);
    BOOST_REQUIRE_EQUAL(set[0], expected);
}

BOOST_AUTO_TEST_CASE(collection__distinct__distinct_sorted__sorted)
{
    data_chunk set{ 0, 2, 4, 6, 8 };
    set = distinct(std::move(set));
    BOOST_REQUIRE_EQUAL(set.size(), 5u);
    BOOST_REQUIRE_EQUAL(set[0], 0u);
    BOOST_REQUIRE_EQUAL(set[1], 2u);
    BOOST_REQUIRE_EQUAL(set[2], 4u);
    BOOST_REQUIRE_EQUAL(set[3], 6u);
    BOOST_REQUIRE_EQUAL(set[4], 8u);
}

BOOST_AUTO_TEST_CASE(collection__distinct__distinct_unsorted__sorted)
{
    data_chunk set{ 2, 0, 8, 6, 4 };
    set = distinct(std::move(set));
    BOOST_REQUIRE_EQUAL(set.size(), 5u);
    BOOST_REQUIRE_EQUAL(set[0], 0u);
    BOOST_REQUIRE_EQUAL(set[1], 2u);
    BOOST_REQUIRE_EQUAL(set[2], 4u);
    BOOST_REQUIRE_EQUAL(set[3], 6u);
    BOOST_REQUIRE_EQUAL(set[4], 8u);
}

BOOST_AUTO_TEST_CASE(collection__distinct__distinct_unsorted_duplicates__sorted_distinct)
{
    data_chunk set{ 2, 0, 0, 8, 6, 4 };
    set = distinct(std::move(set));
    BOOST_REQUIRE_EQUAL(set.size(), 5u);
    BOOST_REQUIRE_EQUAL(set[0], 0u);
    BOOST_REQUIRE_EQUAL(set[1], 2u);
    BOOST_REQUIRE_EQUAL(set[2], 4u);
    BOOST_REQUIRE_EQUAL(set[3], 6u);
    BOOST_REQUIRE_EQUAL(set[4], 8u);
}

// is_distinct

BOOST_AUTO_TEST_CASE(collection__is_distinct__empty__true)
{
    data_chunk set{};
    BOOST_REQUIRE(is_distinct(std::move(set)));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__single__true)
{
    data_chunk set{ 42 };
    BOOST_REQUIRE(is_distinct(std::move(set)));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__distinct__true)
{
    data_chunk set{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(is_distinct(std::move(set)));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__nondistinct__false)
{
    data_chunk set{ 0, 2, 4, 2, 8 };
    BOOST_REQUIRE(!is_distinct(std::move(set)));
}

// is_sorted

BOOST_AUTO_TEST_CASE(collection__is_sorted__empty__true)
{
    data_chunk set{};
    BOOST_REQUIRE(is_sorted(set));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__single__true)
{
    data_chunk set{ 42 };
    BOOST_REQUIRE(is_sorted(set));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__sorted_distinct__true)
{
    data_chunk set{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(is_sorted(set));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__sorted_non_distinct__true)
{
    data_chunk set{ 0, 2, 2, 6, 8 };
    BOOST_REQUIRE(is_sorted(set));
}


BOOST_AUTO_TEST_CASE(collection__is_sorted__unsorted__false)
{
    data_chunk set{ 0, 2, 4, 2, 8 };
    BOOST_REQUIRE(!is_sorted(set));
}

// move_append

BOOST_AUTO_TEST_CASE(collection__move_append__both_empty__both_empty)
{
    data_chunk source;
    data_chunk target;
    move_append(target, source);
    BOOST_REQUIRE_EQUAL(source.size(), 0u);
    BOOST_REQUIRE_EQUAL(target.size(), 0u);
}

BOOST_AUTO_TEST_CASE(collection__move_append__source_empty__both_unchanged)
{
    data_chunk source;
    data_chunk target{ 0, 2, 4, 6, 8 };
    const auto expected = target.size();
    move_append(target, source);
    BOOST_REQUIRE_EQUAL(source.size(), 0u);
    BOOST_REQUIRE_EQUAL(target.size(), expected);
    BOOST_REQUIRE_EQUAL(target[0], 0u);
    BOOST_REQUIRE_EQUAL(target[1], 2u);
    BOOST_REQUIRE_EQUAL(target[2], 4u);
    BOOST_REQUIRE_EQUAL(target[3], 6u);
    BOOST_REQUIRE_EQUAL(target[4], 8u);
}

BOOST_AUTO_TEST_CASE(collection__move_append__target_empty__swapped_values)
{
    data_chunk source{ 0, 2, 4, 6, 8 };
    data_chunk target;
    const auto expected = source.size();
    move_append(target, source);
    BOOST_REQUIRE_EQUAL(source.size(), 0u);
    BOOST_REQUIRE_EQUAL(target.size(), expected);
    BOOST_REQUIRE_EQUAL(target[0], 0u);
    BOOST_REQUIRE_EQUAL(target[1], 2u);
    BOOST_REQUIRE_EQUAL(target[2], 4u);
    BOOST_REQUIRE_EQUAL(target[3], 6u);
    BOOST_REQUIRE_EQUAL(target[4], 8u);
}

BOOST_AUTO_TEST_CASE(collection__move_append__neither_empty__moved_in_order)
{
    data_chunk source{ 10, 12, 14, 16, 18 };
    data_chunk target{ 0, 2, 4, 6, 8 };
    const auto expected = source.size() + source.size();
    move_append(target, source);
    BOOST_REQUIRE_EQUAL(source.size(), 0u);
    BOOST_REQUIRE_EQUAL(target.size(), expected);
    BOOST_REQUIRE_EQUAL(target[0], 0u);
    BOOST_REQUIRE_EQUAL(target[1], 2u);
    BOOST_REQUIRE_EQUAL(target[2], 4u);
    BOOST_REQUIRE_EQUAL(target[3], 6u);
    BOOST_REQUIRE_EQUAL(target[4], 8u);
    BOOST_REQUIRE_EQUAL(target[5], 10u);
    BOOST_REQUIRE_EQUAL(target[6], 12u);
    BOOST_REQUIRE_EQUAL(target[7], 14u);
    BOOST_REQUIRE_EQUAL(target[8], 16u);
    BOOST_REQUIRE_EQUAL(target[9], 18u);
}

BOOST_AUTO_TEST_CASE(collection__pop__single__empty_and_returns_expected)
{
    const uint8_t expected = 42u;
    data_chunk stack{ expected };
    const auto value = pop(stack);
    BOOST_REQUIRE(stack.empty());
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(collection__pop__multiple__popped_and_returns_expected)
{
    const uint8_t expected = 42u;
    data_chunk stack{ 0, 1, 2, 3, expected };
    const auto value = pop(stack);
    BOOST_REQUIRE_EQUAL(stack.size(), 4u);
    BOOST_REQUIRE_EQUAL(stack[0], 0u);
    BOOST_REQUIRE_EQUAL(stack[1], 1u);
    BOOST_REQUIRE_EQUAL(stack[2], 2u);
    BOOST_REQUIRE_EQUAL(stack[3], 3u);
    BOOST_REQUIRE_EQUAL(value, expected);
}

// reverse

BOOST_AUTO_TEST_SUITE_END()
