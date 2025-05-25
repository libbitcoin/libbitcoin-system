/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <deque>

BOOST_AUTO_TEST_SUITE(collection_tests)

// TODO:
// contains
// deep_equal
// find_pair_position
// find_position
// insert_sorted
// binary_search:
//  Collection must implement [].
//  Either list members or the search key must implement lexical compare (<, >).

// binary_search (native byte comparison operators)

BOOST_AUTO_TEST_CASE(limits__binary_search_native__empty__not_found)
{
    const std::string empty = "";
    constexpr auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(empty, value), negative_one);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__one_element_unmatched__not_found)
{
    const std::string single = "a";
    const auto value = 'b';
    BOOST_REQUIRE_EQUAL(binary_search(single, value), negative_one);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__one_element_match__0)
{
    const std::string single = "a";
    const auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(single, value), 0);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__two_elements_match_first__0)
{
    const std::string sorted = "ab";
    const auto value = 'a';
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 0);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__two_elements_match_second__1)
{
    const std::string sorted = "ab";
    const auto value = 'b';
    auto result = binary_search(sorted, value);
    BOOST_REQUIRE_EQUAL(result, 1);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__three_elements_match_second__1)
{
    const std::string sorted = "abc";
    const auto value = 'b';
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 1);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__sorted_contained__expected)
{
    const std::string sorted = "abcdefjkxyz";
    const auto value = 'x';
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 8);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_native__reverse_sorted_contained__unlucky)
{
    const std::string unsorted = "zyxwvutscba";
    constexpr auto value = 'x';
    BOOST_REQUIRE_EQUAL(binary_search(unsorted, value), -1);
}

// binary_search (list element comparison operators)

BOOST_AUTO_TEST_CASE(limits__binary_search_element__three_various_elements_unmatched__not_found)
{
    const string_list sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const auto value = "bcd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), negative_one);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_element__three_various_elements_matched__found)
{
    const string_list sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const auto value = "cfdd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 2);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_element__unsorted_contained__unlucky)
{
    const string_list unsorted{ { "z", "y", "x" } };
    const auto value = "z";
    BOOST_REQUIRE_EQUAL(binary_search(unsorted, value), negative_one);
}

// binary_search (key comparison operators)

BOOST_AUTO_TEST_CASE(limits__binary_search_key__three_various_elements_unmatched__not_found)
{
    const std_vector<const char*> sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const std::string value = "bcd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), negative_one);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_key__three_various_elements_matched__found)
{
    const std_vector<const char*> sorted{ "afgdjdfj", "btffghfg", "cfdd" };
    const std::string value = "cfdd";
    BOOST_REQUIRE_EQUAL(binary_search(sorted, value), 2);
}

BOOST_AUTO_TEST_CASE(limits__binary_search_key__unsorted_contained__unlucky)
{
    const std_vector<const char*> unsorted{ "z", "y", "x" };
    const std::string value = "z";
    BOOST_REQUIRE_EQUAL(binary_search(unsorted, value), negative_one);
}

// projection

BOOST_AUTO_TEST_CASE(collection__projection_vector__empty__empty)
{
    BOOST_REQUIRE(projection<std_vector<uint16_t>>(data_chunk{}).empty());
}

BOOST_AUTO_TEST_CASE(collection__projection_vector__move__same)
{
    const auto result = projection<std_vector<uint32_t>>(data_chunk{ 42u });
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result.front(), 42u);
}

BOOST_AUTO_TEST_CASE(collection__projection_vector__copy__same)
{
    const data_chunk value{ 42u, 24u };
    const auto result = projection<std_vector<size_t>>(value);
    BOOST_REQUIRE_EQUAL(value.size(), result.size());
    BOOST_REQUIRE_EQUAL(result[0], value[0]);
    BOOST_REQUIRE_EQUAL(result[1], value[1]);
}

// pop

BOOST_AUTO_TEST_CASE(collection__pop__empty__empty_default)
{
    data_chunk stack;
    const auto value = pop(stack);
    BOOST_REQUIRE(stack.empty());
    BOOST_REQUIRE_EQUAL(value, 0u);
}

BOOST_AUTO_TEST_CASE(collection__pop__single__empty_expected)
{
    constexpr uint8_t expected = 42u;
    data_chunk stack{ expected };
    const auto value = pop(stack);
    BOOST_REQUIRE(stack.empty());
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(collection__pop__multiple__popped_expected)
{
    const uint8_t expected_value = 42u;
    data_chunk stack{ 0, 1, 2, 3, expected_value };
    const data_chunk expected_stack{ 0, 1, 2, 3 };
    const auto value = pop(stack);
    BOOST_REQUIRE_EQUAL(value, expected_value);
    BOOST_REQUIRE_EQUAL(stack, expected_stack);
}

// pop_front
using data_queue = std::deque<uint8_t>;

BOOST_AUTO_TEST_CASE(collection__pop_front__empty__empty_default)
{
    data_queue queue{};
    const auto value = pop_front(queue);
    BOOST_REQUIRE(queue.empty());
    BOOST_REQUIRE_EQUAL(value, 0u);
}

BOOST_AUTO_TEST_CASE(collection__pop_front__single__empty_expected)
{
    constexpr uint8_t expected = 42u;
    data_queue queue{ expected };
    const auto value = pop_front(queue);
    BOOST_REQUIRE(queue.empty());
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_CASE(collection__pop_front__multiple__popped_expected)
{
    constexpr uint8_t expected_value = 42u;
    data_queue queue{ expected_value, 0, 1, 2, 3 };
    const data_queue expected_queue{ 0, 1, 2, 3 };
    const auto value = pop_front(queue);
    BOOST_REQUIRE_EQUAL(value, expected_value);
    BOOST_REQUIRE(queue == expected_queue);
}

// is_distinct

BOOST_AUTO_TEST_CASE(collection__is_distinct__empty__true)
{
    BOOST_REQUIRE(is_distinct(data_chunk{}));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__single__true)
{
    BOOST_REQUIRE(is_distinct(data_chunk{ 42 }));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__distinct__true)
{
    BOOST_REQUIRE(is_distinct(data_chunk{ 0, 2, 4, 6, 8 }));
}

BOOST_AUTO_TEST_CASE(collection__is_distinct__const_nondistinct__false)
{
    const data_chunk set{ 0, 2, 4, 2, 8 };
    BOOST_REQUIRE(!is_distinct(set));
}

// is_sorted

BOOST_AUTO_TEST_CASE(collection__is_sorted__empty__true)
{
    BOOST_REQUIRE(is_sorted(data_chunk{}));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__single__true)
{
    BOOST_REQUIRE(is_sorted(data_chunk{ 42 }));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__sorted_distinct__true)
{
    BOOST_REQUIRE(is_sorted(data_chunk{ 0, 2, 4, 6, 8 }));
}

BOOST_AUTO_TEST_CASE(collection__is_sorted__sorted_non_distinct__true)
{
    BOOST_REQUIRE(is_sorted(data_chunk{ 0, 2, 2, 6, 8 }));
}


BOOST_AUTO_TEST_CASE(collection__is_sorted__unsorted__false)
{
    BOOST_REQUIRE(!is_sorted(data_chunk{ 0, 2, 4, 2, 8 }));
}

// distinct

BOOST_AUTO_TEST_CASE(collection__distinct_move__empty__empty)
{
    BOOST_REQUIRE(distinct(data_chunk{}).empty());
}

BOOST_AUTO_TEST_CASE(collection__distinct_move__single__unchanged)
{
    const uint8_t expected = 42;
    BOOST_REQUIRE_EQUAL(distinct(data_chunk{ expected }).front(), expected);
}

BOOST_AUTO_TEST_CASE(collection__distinct_move__distinct_sorted__unchanged)
{
    const data_chunk expected{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE_EQUAL(distinct(data_chunk{ 0, 2, 4, 6, 8 }), expected);
}

BOOST_AUTO_TEST_CASE(collection__distinct_move__distinct_unsorted__sorted)
{
    const data_chunk expected{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE_EQUAL(distinct(data_chunk{ 2, 0, 8, 6, 4 }), expected);
}

BOOST_AUTO_TEST_CASE(collection__distinct__distinct_unsorted_duplicates__sorted_distinct)
{
    data_chunk set{ 2, 0, 0, 8, 6, 4 };
    const data_chunk expected{ 0, 2, 4, 6, 8 };
    distinct(set);
    BOOST_REQUIRE_EQUAL(set, expected);
}

BOOST_AUTO_TEST_CASE(collection__distinct_copy__distinct_unsorted_duplicates__sorted_distinct)
{
    const data_chunk set{ 2, 0, 0, 8, 6, 4 };
    const data_chunk expected{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE_EQUAL(distinct_copy(set), expected);
}

// is_intersecting

BOOST_AUTO_TEST_CASE(collection__is_intersecting__empty_sets__false)
{
    const data_chunk left;
    const data_chunk right;
    BOOST_REQUIRE(!is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__left_set_empty__false)
{
    const data_chunk left;
    const data_chunk right{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(!is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__right_set_empty__false)
{
    const data_chunk left{ 2, 0, 0, 8, 6, 4 };
    const data_chunk right;
    BOOST_REQUIRE(!is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__distinct_sets__false)
{
    const data_chunk left{ 0, 2, 4, 6, 8 };
    const data_chunk right{ 1, 3, 5, 7, 9 };
    BOOST_REQUIRE(!is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__same_sets__true)
{
    const data_chunk left{ 0, 2, 4, 6, 8 };
    const data_chunk right{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__consistent_sets__true)
{
    const data_chunk left{ 2, 0, 0, 8, 6, 4 };
    const data_chunk right{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__left_subset__true)
{
    const data_chunk left{ 8, 6 };
    const data_chunk right{ 0, 2, 4, 6, 8 };
    BOOST_REQUIRE(is_intersecting(left, right));
}

BOOST_AUTO_TEST_CASE(collection__is_intersecting__right_subset__true)
{
    const data_chunk left{ 2, 0, 0, 8, 6, 4 };
    const data_chunk right{ 0, 8 };
    BOOST_REQUIRE(is_intersecting(left, right));
}

// difference

BOOST_AUTO_TEST_CASE(collection__difference__empty_sets__empty)
{
    const data_chunk minuend;
    const data_chunk subtrahend;
    const data_chunk expected{ minuend };
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__idifference__minuend_empty__empty)
{
    const data_chunk minuend;
    const data_chunk subtrahend{ 0, 2, 4, 6, 8 };
    const data_chunk expected{ minuend };
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__subtrahend_empty__minuend)
{
    const data_chunk minuend{ 2, 0, 0, 8, 6, 4 };
    const data_chunk subtrahend;
    const data_chunk expected{ minuend };
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__distinct_sets__minuend)
{
    const data_chunk minuend{ 0, 2, 4, 6, 8 };
    const data_chunk subtrahend{ 1, 3, 5, 7, 9 };
    const data_chunk expected{ minuend };
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__same_sets__empty)
{
    const data_chunk minuend{ 0, 2, 4, 6, 8 };
    const data_chunk subtrahend{ 0, 2, 4, 6, 8 };
    const data_chunk expected;
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__consistent_sets__empty)
{
    const data_chunk minuend{ 2, 0, 0, 8, 6, 4 };
    const data_chunk subtrahend{ 0, 2, 4, 6, 8 };
    const data_chunk expected;
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__minuend_subset__empty)
{
    const data_chunk minuend{ 8, 6 };
    const data_chunk subtrahend{ 0, 2, 4, 6, 8 };
    const data_chunk expected;
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

BOOST_AUTO_TEST_CASE(collection__difference__subtrahend_subset__expected)
{
    const data_chunk minuend{ 2, 0, 0, 8, 6, 4 };
    const data_chunk subtrahend{ 0, 8 };
    const data_chunk expected{ 2, 6, 4 };
    BOOST_REQUIRE_EQUAL(difference(minuend, subtrahend), expected);
}

// part

BOOST_AUTO_TEST_CASE(collection__part__vector__true)
{
    std::vector<int> from = { 1, 2, 4, 7, 8, 3, 1, 5, 3 };
    std::vector<int> to{};
    BOOST_REQUIRE(part(from, to, 3));
    BOOST_REQUIRE_EQUAL(from, (std::vector<int>{1, 2, 4, 7, 8}));
    BOOST_REQUIRE_EQUAL(to, (std::vector<int>{3, 1, 5, 3}));
}

BOOST_AUTO_TEST_CASE(collection__part__list__true)
{
    std::list<int> from = { 1, 2, 4, 7, 8, 3, 1, 5, 3 };
    std::list<int> to{};
    BOOST_REQUIRE(part(from, to, 3));
    BOOST_REQUIRE(from == (std::list<int>{1, 2, 4, 7, 8}));
    BOOST_REQUIRE(to == (std::list<int>{3, 1, 5, 3}));
}

BOOST_AUTO_TEST_CASE(collection__part__deque__true)
{
    std::deque<int> from = { 1, 2, 4, 7, 8, 3, 1, 5, 3 };
    std::deque<int> to{};
    BOOST_REQUIRE(part(from, to, 3));
    BOOST_REQUIRE(from == (std::deque<int>{1, 2, 4, 7, 8}));
    BOOST_REQUIRE(to == (std::deque<int>{3, 1, 5, 3}));
}

BOOST_AUTO_TEST_CASE(collection__part__vector_empty__false)
{
    std::vector<int> from{};
    std::vector<int> to{};
    BOOST_REQUIRE(!part(from, to, 3));
    BOOST_REQUIRE_EQUAL(from, (std::vector<int>{}));
    BOOST_REQUIRE_EQUAL(to, (std::vector<int>{}));
}

BOOST_AUTO_TEST_CASE(collection__part__vector_no_equal__false)
{
    std::vector<int> from = { 1, 2, 4, 5 };
    std::vector<int> to{};
    BOOST_REQUIRE(!part(from, to, 3));
    BOOST_REQUIRE_EQUAL(from, (std::vector<int>{1, 2, 4, 5}));
    BOOST_REQUIRE_EQUAL(to, (std::vector<int>{}));
}

BOOST_AUTO_TEST_CASE(collection__part__list_first_equal__true)
{
    std::list<int> from = { 3, 6, 7, 8 };
    std::list<int> to{};
    BOOST_REQUIRE(part(from, to, 3));
    BOOST_REQUIRE(from == (std::list<int>{}));
    BOOST_REQUIRE(to == (std::list<int>{3, 6, 7, 8}));
}

BOOST_AUTO_TEST_CASE(collection__part__vector_non_empty_to__true)
{
    std::vector<int> from = { 1, 3, 4, 5 };
    std::vector<int> to = { 10, 11 };
    BOOST_REQUIRE(part(from, to, 3));
    BOOST_REQUIRE_EQUAL(from, (std::vector<int>{1}));
    BOOST_REQUIRE_EQUAL(to, (std::vector<int>{10, 11, 3, 4, 5}));
}

BOOST_AUTO_TEST_CASE(collection__part__string_list__true)
{
    string_list from = { "apple", "date", "banana", "cherry" };
    string_list to{};
    BOOST_REQUIRE(part(from, to, std::string("date")));
    BOOST_REQUIRE_EQUAL(from, (string_list{ "apple" }));
    BOOST_REQUIRE_EQUAL(to, (string_list{ "date", "banana", "cherry" }));
}

BOOST_AUTO_TEST_CASE(collection__part__string_list_first_equal__true)
{
    string_list from = { "apple", "banana", "cherry", "date" };
    string_list to{};
    BOOST_REQUIRE(part(from, to, std::string("apple")));
    BOOST_REQUIRE_EQUAL(from, (string_list{}));
    BOOST_REQUIRE_EQUAL(to, (string_list{ "apple", "banana", "cherry", "date" }));
}

BOOST_AUTO_TEST_CASE(collection__part__string_list_last_equal__true)
{
    string_list from = { "banana", "cherry", "date", "apple" };
    string_list to{};
    BOOST_REQUIRE(part(from, to, std::string("apple")));
    BOOST_REQUIRE_EQUAL(from, (string_list{ "banana", "cherry", "date" }));
    BOOST_REQUIRE_EQUAL(to, (string_list{ "apple" }));
}

// reverse

BOOST_AUTO_TEST_CASE(collection__reverse_move__empty__empty)
{
    BOOST_REQUIRE(reverse(data_chunk{}).empty());
}

BOOST_AUTO_TEST_CASE(collection__reverse_move__single__unchanged)
{
    constexpr uint8_t expected = 42;
    BOOST_REQUIRE_EQUAL(reverse(data_chunk{ expected }).front(), expected);
}

BOOST_AUTO_TEST_CASE(collection__reverse__unsorted_duplicates__reversed)
{
    data_chunk set{ 2, 0, 0, 8, 6, 4 };
    const data_chunk expected{ 4, 6, 8, 0, 0, 2 };
    reverse(set);
    BOOST_REQUIRE_EQUAL(set, expected);
}

BOOST_AUTO_TEST_CASE(collection__reverse_copy__unsorted_duplicates__reversed)
{
    const data_chunk set{ 2, 0, 0, 8, 6, 4 };
    const data_chunk expected{ 4, 6, 8, 0, 0, 2 };
    BOOST_REQUIRE_EQUAL(reverse_copy(set), expected);
}

// sort

BOOST_AUTO_TEST_CASE(collection__sort_move__empty__empty)
{
    BOOST_REQUIRE(sort(data_chunk{}).empty());
}

BOOST_AUTO_TEST_CASE(collection__sort_move__single__unchanged)
{
    constexpr uint8_t expected = 42;
    BOOST_REQUIRE_EQUAL(sort(data_chunk{ expected }).front(), expected);
}

BOOST_AUTO_TEST_CASE(collection__sort_move__unsorted_duplicates__sorted)
{
    data_chunk set{ 2, 0, 0, 8, 6, 4 };
    const data_chunk expected{ 0, 0, 2, 4, 6, 8 };
    sort(set);
    BOOST_REQUIRE_EQUAL(set, expected);
}

BOOST_AUTO_TEST_CASE(collection__sort_copy__unsorted_duplicates__sorted)
{
    const data_chunk set{ 2, 0, 0, 8, 6, 4 };
    data_chunk expected{ 0, 0, 2, 4, 6, 8 };
    BOOST_REQUIRE_EQUAL(sort_copy(set), expected);
}

// starts_with

BOOST_AUTO_TEST_CASE(collection__starts_with__empty_empty__true)
{
    static const data_chunk buffer;
    static const data_chunk sequence;
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(collection__starts_with__not_empty_empty__false)
{
    static const data_chunk buffer;
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(!starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(collection__starts_with__same_vectors__true)
{
    static const data_chunk buffer{ 42 };
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(collection__starts_with__matched_vectors__true)
{
    static const data_chunk buffer{ 42, 24, 15 };
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(collection__starts_with__mismatched_arrays__false)
{
    // Must be the same length when using arrays.
    static const data_array<2> buffer{ { 24, 0 } };
    static const data_array<2> sequence{ { 24, 42 } };
    const auto result = starts_with(buffer.begin(), buffer.end(), sequence);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(collection__starts_with__matched_arrays__true)
{
    // Must be the same length when using arrays.
    static const data_array<2> buffer{ { 24, 42 } };
    static const data_array<2> sequence{ { 24, 42 } };
    const auto result = starts_with(buffer.begin(), buffer.end(), sequence);
    BOOST_REQUIRE(result);
}

BOOST_AUTO_TEST_SUITE_END()
