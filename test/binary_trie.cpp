/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <new>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

template<typename Value,
    typename NodeAlloc = std::allocator<trie_structure_node<Value>>,
    typename ValueNodeAlloc = std::allocator<trie_value_node<Value>>,
    typename Comparer = std::less<Value>>
static std::vector<typename binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>::pair_iterator_bool>
    insert_equal(
        binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>& tree,
        std::vector<std::pair<Value, binary_type>>& value_key_pairs)
{
    std::vector<typename binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>::pair_iterator_bool> result;

    // insert
    for (auto query : value_key_pairs)
    {
        auto insert_result = tree.insert_equal(query.second, query.first);

        BOOST_REQUIRE(insert_result.second);
        BOOST_REQUIRE(insert_result.first != tree.end());

        result.push_back(insert_result);
    }

    BOOST_REQUIRE(tree.begin() != tree.end());

    return result;
}

template<typename Value,
    typename NodeAlloc = std::allocator<trie_structure_node<Value>>,
    typename ValueNodeAlloc = std::allocator<trie_value_node<Value>>,
    typename Comparer = std::less<Value>>
static void test_insert_equal_with_unique_keys_succeeds(
    binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>& tree,
    std::vector<std::pair<Value, binary_type>>& value_key_pairs)
{
    insert_equal(tree, value_key_pairs);

    // find
    for (auto query : value_key_pairs)
    {
        auto find_result = tree.find_equal(query.second);

        BOOST_REQUIRE(find_result.first != find_result.second);
        BOOST_REQUIRE(*(find_result.first) == query.first);
        BOOST_REQUIRE(find_result.first.get_key() == query.second);
        BOOST_REQUIRE(++find_result.first == find_result.second);
    }
}

template<typename Value,
    typename NodeAlloc = std::allocator<trie_structure_node<Value>>,
    typename ValueNodeAlloc = std::allocator<trie_value_node<Value>>,
    typename Comparer = std::less<Value>>
static void test_insert_unique_matches_expectation(
    binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>& tree,
    std::vector<std::tuple<bool, Value, binary_type>>& query_tuples)
{
    // insert
    for (auto query : query_tuples)
    {
        auto insert_result = tree.insert_unique(std::get<2>(query), std::get<1>(query));

        BOOST_REQUIRE(insert_result.second == std::get<0>(query));

        if (insert_result.second)
        {
            BOOST_REQUIRE(insert_result.first != tree.end());
            BOOST_REQUIRE(*(insert_result.first) == std::get<1>(query));
        }
    }

    BOOST_REQUIRE(tree.begin() != tree.end());

    // find
    for (auto query : query_tuples)
    {
        auto find_result = tree.find_equal(std::get<2>(query));

        if (std::get<0>(query))
        {
            BOOST_REQUIRE(find_result.first != find_result.second);
            BOOST_REQUIRE(*(find_result.first) == std::get<1>(query));
            BOOST_REQUIRE(find_result.first.get_key() == std::get<2>(query));
            BOOST_REQUIRE(++find_result.first == find_result.second);
        }
        else
        {
            if (std::get<2>(query).size() != 0)
            {
                BOOST_REQUIRE(find_result.first != find_result.second);
            }
            else
            {
                BOOST_REQUIRE(find_result.first == find_result.second);
            }
        }

    }
}

template<typename Value,
    typename NodeAlloc = std::allocator<trie_structure_node<Value>>,
    typename ValueNodeAlloc = std::allocator<trie_value_node<Value>>,
    typename Comparer = std::less<Value>>
static void verify_iterator_range(
    std::vector<Value>& expected,
    typename binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>::iterator_range& range)
{
    auto expected_it = expected.begin();
    auto actual_it = range.first;

    while ((expected_it != expected.end()) && (actual_it != range.second))
    {
        BOOST_REQUIRE(*actual_it == *expected_it);

        ++expected_it;
        ++actual_it;
    }

    BOOST_REQUIRE(expected_it == expected.end());
    BOOST_REQUIRE(actual_it == range.second);
}

template<typename Value,
    typename NodeAlloc = std::allocator<trie_structure_node<Value>>,
    typename ValueNodeAlloc = std::allocator<trie_value_node<Value>>,
    typename Comparer = std::less<Value>>
static void verify_iterator_range(
    std::vector<std::pair<Value, binary_type>>& expected,
    typename binary_trie<Value, NodeAlloc, ValueNodeAlloc, Comparer>::iterator_range& range)
{
    auto expected_it = expected.begin();
    auto actual_it = range.first;

    while ((expected_it != expected.end()) && (actual_it != range.second))
    {
//        if (*actual_it != (*expected_it).first)
//        {
//            std::cout << "actual: " << *actual_it << std::endl;
//            std::cout << "expect: " << (*expected_it).first << std::endl;
//        }

        BOOST_REQUIRE(*actual_it == (*expected_it).first);
        BOOST_REQUIRE(actual_it.get_key() == (*expected_it).second);

        ++expected_it;
        ++actual_it;
    }

    BOOST_REQUIRE(expected_it == expected.end());

    bool exhausted_actual_values = (actual_it == range.second);

//    int i = 0;
//    while ((actual_it != range.second) && (i < 5))
//    {
//        std::cout << "actual: " << *actual_it << std::endl;
//        ++actual_it;
//        i++;
//    }

    BOOST_REQUIRE(exhausted_actual_values);
}

template<typename Value>
static std::pair<std::allocator<trie_structure_node<Value>>, std::allocator<trie_value_node<Value>>>
    get_allocators()
{
    std::allocator<trie_structure_node<Value>> structure_allocator;
    std::allocator<trie_value_node<Value>> value_allocator;

    return std::make_pair(structure_allocator, value_allocator);
}

BOOST_AUTO_TEST_SUITE(binary_trie_tests)

//
// constructor new/delete tests
//
BOOST_AUTO_TEST_CASE(constructor_allocator_args)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int>* tree = new binary_trie<int>(
        alloc_pair.first, alloc_pair.second);

    BOOST_REQUIRE(tree->begin() == tree->end());
    BOOST_REQUIRE(tree->cbegin() == tree->cend());
    BOOST_REQUIRE(tree->rbegin() == tree->rend());
    BOOST_REQUIRE(tree->crbegin() == tree->crend());

    delete tree;

    BOOST_REQUIRE(true);
}

//
// insert_equal tests
//
BOOST_AUTO_TEST_CASE(insert_equal_zero_length_key_fails)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(0, bc::data_chunk {});
    int value = 17;

    auto insert_result = tree.insert_equal(key, value);

    BOOST_REQUIRE(!insert_result.second);
    BOOST_REQUIRE(tree.begin() == insert_result.first);
    BOOST_REQUIRE(tree.begin() == tree.end());

    auto find_result = tree.find_equal(key);

    BOOST_REQUIRE(find_result.first == find_result.second);
    BOOST_REQUIRE(find_result.first == tree.end());
    BOOST_REQUIRE(find_result.second == tree.end());
}

BOOST_AUTO_TEST_CASE(insert_equal_nonzero_length_key_succeeds)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });
    int value = 17;

    auto insert_result = tree.insert_equal(key, value);

    BOOST_REQUIRE(insert_result.second);
    BOOST_REQUIRE(insert_result.first != tree.end());
    BOOST_REQUIRE(insert_result.first.get_key() == key);
    BOOST_REQUIRE(*(insert_result.first) == value);
    BOOST_REQUIRE(tree.begin() == insert_result.first);
    BOOST_REQUIRE(tree.begin() != tree.end());

    auto find_result = tree.find_equal(key);

    BOOST_REQUIRE(find_result.first != find_result.second);
    BOOST_REQUIRE(find_result.first == tree.begin());
    BOOST_REQUIRE(find_result.second == tree.end());

    BOOST_REQUIRE(find_result.first.get_key() == key);
    BOOST_REQUIRE(*(find_result.first) == value);
}

BOOST_AUTO_TEST_CASE(insert_equal_multiple_same_key_succeeds)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });
    std::vector<int> values = { 17, 112, 34, 56 };

    for (auto value : values)
    {
        auto insert_result = tree.insert_equal(key, value);

        BOOST_REQUIRE(insert_result.second);
        BOOST_REQUIRE(insert_result.first != tree.end());
        BOOST_REQUIRE(*(insert_result.first) == value);
    }

    BOOST_REQUIRE(tree.begin() != tree.end());

    auto find_result = tree.find_equal(key);

    BOOST_REQUIRE(find_result.first != find_result.second);

    std::vector<int> expected = { 17, 34, 56, 112 };
    auto vit = expected.begin();
    auto rit = find_result.first;

    for (; (rit != find_result.second) && (vit != expected.end()); ++rit, ++vit)
    {
        BOOST_REQUIRE(*vit == *rit);
    }

    BOOST_REQUIRE(vit == expected.end());
    BOOST_REQUIRE(rit == find_result.second);
}

BOOST_AUTO_TEST_CASE(insert_equal_add_leaf_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_pairs = {
        std::make_pair(97,      binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112,     binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(17,      binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds(tree, value_key_pairs);
}

BOOST_AUTO_TEST_CASE(insert_equal_add_intermediary_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_pairs = {
        std::make_pair(97,      binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(17,      binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112,     binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds(tree, value_key_pairs);
}

BOOST_AUTO_TEST_CASE(insert_equal_match_existing_intermediary_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_pairs = {
        std::make_pair(97,      binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112,     binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_pair(17,      binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,      binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_equal_with_unique_keys_succeeds(tree, value_key_pairs);
}

BOOST_AUTO_TEST_CASE(insert_equal_multiple_unique_keys)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_pairs = {
        std::make_pair(97,      binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112,     binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_pair(17,      binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,      binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(34,      binary_type(22, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(56,      binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(435,     binary_type(15, bc::data_chunk { 0x0A, 0x5B, 0x00 })),
        std::make_pair(234,     binary_type(18, bc::data_chunk { 0x00, 0x02, 0x00 }))
    };

    test_insert_equal_with_unique_keys_succeeds(tree, value_key_pairs);
}

//
// insert_unique tests
//
BOOST_AUTO_TEST_CASE(insert_unique_zero_length_key_fails)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(0, bc::data_chunk {});
    int value = 17;

    auto insert_result = tree.insert_unique(key, value);

    BOOST_REQUIRE(!insert_result.second);
    BOOST_REQUIRE(tree.begin() == insert_result.first);
    BOOST_REQUIRE(tree.begin() == tree.end());

    auto find_result = tree.find_equal(key);

    BOOST_REQUIRE(find_result.first == find_result.second);
    BOOST_REQUIRE(find_result.first == tree.end());
    BOOST_REQUIRE(find_result.second == tree.end());
}

BOOST_AUTO_TEST_CASE(insert_unique_collision_returns_error)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, int, binary_type>> value_key_tuples = {
        std::make_tuple(true, 97,   binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(false, 112,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_add_leaf_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, int, binary_type>> value_key_tuples = {
        std::make_tuple(true, 97,   binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 112,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 17,   binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_add_intermediary_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, int, binary_type>> value_key_tuples = {
        std::make_tuple(true, 97,   binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 17,   binary_type(25, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 112,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation(tree, value_key_tuples);
}

BOOST_AUTO_TEST_CASE(insert_unique_match_existing_intermediary_node)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::tuple<bool, int, binary_type>> value_key_tuples = {
        std::make_tuple(true, 97,   binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 112,  binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_tuple(true, 17,   binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_tuple(true, 26,   binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    test_insert_unique_matches_expectation(tree, value_key_tuples);
}

//
// find_prefix tests
//
BOOST_AUTO_TEST_CASE(find_prefix_matches_none)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    insert_equal(tree, value_key_tuples);

    binary_type query(8, bc::data_chunk { 0xBB });

    auto query_result = tree.find_prefix(query);

    BOOST_REQUIRE(query_result.first == query_result.second);
    BOOST_REQUIRE(query_result.first == tree.end());
}

BOOST_AUTO_TEST_CASE(find_prefix_matches_all)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xBB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xBB, 0xCC }))
    };

    binary_type query(8, bc::data_chunk { 0xAA });

    std::vector<int> expected = { 97, 26, 112, 17 };

    insert_equal(tree, value_key_tuples);

    auto query_result = tree.find_prefix(query);

    BOOST_REQUIRE(query_result.first != query_result.second);
    BOOST_REQUIRE(query_result.first == tree.begin());
    BOOST_REQUIRE(query_result.second == tree.end());

    verify_iterator_range(expected, query_result);
}

BOOST_AUTO_TEST_CASE(find_prefix_matches_returns_subset)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    binary_type query(10, bc::data_chunk { 0xAA, 0xC0 });

    std::vector<int> expected = { 26, 112 };

    insert_equal(tree, value_key_tuples);

    auto query_result = tree.find_prefix(query);

    BOOST_REQUIRE(query_result.first != query_result.second);
    BOOST_REQUIRE((query_result.first != tree.begin()) || (query_result.second != tree.end()));

    verify_iterator_range(expected, query_result);
}

//
// remove_equal tests
//
BOOST_AUTO_TEST_CASE(remove_equal_from_empty_tree)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type key(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });

    bool success = tree.remove_equal(key);

    BOOST_REQUIRE(!success);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_key_zero_length_returns_false)
{
    binary_type query(0, bc::data_chunk { 0x00 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(query);

    BOOST_REQUIRE(!success);

    auto range = std::make_pair(tree.begin(), tree.end());

    verify_iterator_range(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_key_not_present_returns_false)
{
    binary_type query(4, bc::data_chunk { 0xFF });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(query);

    BOOST_REQUIRE(!success);

    auto range = std::make_pair(tree.begin(), tree.end());

    verify_iterator_range(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_leaf_of_root_with_single_value)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected(
        ++(value_key_tuples.begin()), value_key_tuples.end());

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_leaf_with_multiple_values)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_pair(523,  binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected(
        ++(++(value_key_tuples.begin())), value_key_tuples.end());

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_unique_key)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_pair(235, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(235, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_equal_from_single_key_multiply_inserted)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(235, binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_equal(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 }))
    };

    verify_iterator_range(expected, range);
}

//
// remove_prefix tests
//
BOOST_AUTO_TEST_CASE(remove_prefix_from_empty_tree)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    binary_type prefix(20, bc::data_chunk { 0xAA, 0xBB, 0xCC });

    bool success = tree.remove_prefix(prefix);

    BOOST_REQUIRE(!success);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_key_zero_length_returns_false)
{
    binary_type query(0, bc::data_chunk { 0x00 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(!success);

    auto range = std::make_pair(tree.begin(), tree.end());

    verify_iterator_range<int, std::less<int>>(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_key_not_present_returns_false)
{
    binary_type query(4, bc::data_chunk { 0xFF });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(!success);

    auto range = std::make_pair(tree.begin(), tree.end());

    verify_iterator_range(value_key_tuples, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_single_matching_key)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_single_matching_prefix)
{
    binary_type query(2, bc::data_chunk { 0x40 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_single_node_matching_key_with_multiplicity)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_single_node_matching_prefix_with_multiplicity)
{
    binary_type query(2, bc::data_chunk { 0x40 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_key)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_prefix)
{
    binary_type query(2, bc::data_chunk { 0x40 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_key_with_multiplicity)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(27,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(66,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
    };

    verify_iterator_range(expected, range);
}

BOOST_AUTO_TEST_CASE(remove_prefix_from_subtree_matching_prefix_with_multiplicity)
{
    binary_type query(2, bc::data_chunk { 0x40 });

    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(22,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(57,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(11,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(27,  binary_type(16, bc::data_chunk { 0x55, 0x12 })),
        std::make_pair(99,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0xF0 })),
        std::make_pair(88,  binary_type(20, bc::data_chunk { 0x55, 0x12, 0x00 })),
        std::make_pair(77,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(66,  binary_type(14, bc::data_chunk { 0x50, 0xF0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    bool success = tree.remove_prefix(query);

    BOOST_REQUIRE(success);

    auto range = std::make_pair(tree.begin(), tree.end());

    std::vector<std::pair<int, binary_type>> expected = {
        std::make_pair(00,  binary_type(1, bc::data_chunk { 0x00 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB })),
        std::make_pair(56,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCC })),
        std::make_pair(44,  binary_type(24, bc::data_chunk { 0xAA, 0xCB, 0xCF }))
    };

    verify_iterator_range(expected, range);
}

//
// remove_value tests
//
BOOST_AUTO_TEST_CASE(remove_value_from_empty_tree)
{
    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto iterator = tree.remove_value(tree.begin());

    BOOST_REQUIRE(iterator == tree.begin());
}

BOOST_AUTO_TEST_CASE(remove_value_from_leaf_of_root_with_single_value)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].first);

    BOOST_REQUIRE(iterator != insert_results[0].first);

    auto find_result = tree.find_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(remove_value_from_leaf_with_multiple_values)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(17,  binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_pair(523,  binary_type(10, bc::data_chunk { 0xAB, 0xA0 })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].first);

    BOOST_REQUIRE(iterator != insert_results[0].first);

    auto find_result = tree.find_equal(value_key_tuples[0].second);

    BOOST_REQUIRE(find_result.first != find_result.second);

    while (find_result.first != find_result.second)
    {
        BOOST_REQUIRE(*(find_result.first) != value_key_tuples[0].first);
        find_result.first++;
    }
}

BOOST_AUTO_TEST_CASE(remove_value_from_unique_key)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(235, binary_type(4, bc::data_chunk { 0x50 })),
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[0].first);

    BOOST_REQUIRE(iterator != insert_results[0].first);

    auto find_result = tree.find_prefix(value_key_tuples[0].second);

    BOOST_REQUIRE(find_result.first == find_result.second);
}

BOOST_AUTO_TEST_CASE(remove_value_from_single_key_multiply_inserted)
{
    std::vector<std::pair<int, binary_type>> value_key_tuples = {
        std::make_pair(235, binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(97,  binary_type(10, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(112, binary_type(20, bc::data_chunk { 0xAA, 0xCB, 0x00 })),
        std::make_pair(17,  binary_type(24, bc::data_chunk { 0xAA, 0xBB, 0xCC })),
        std::make_pair(26,  binary_type(16, bc::data_chunk { 0xAA, 0xCB, 0xCC }))
    };

    auto alloc_pair = get_allocators<int>();

    binary_trie<int> tree(alloc_pair.first, alloc_pair.second);

    auto insert_results = insert_equal(tree, value_key_tuples);

    auto iterator = tree.remove_value(insert_results[1].first);

    BOOST_REQUIRE(iterator != insert_results[1].first);
    BOOST_REQUIRE(iterator.get_key() == value_key_tuples[0].second);

    auto find_result = tree.find_prefix(value_key_tuples[1].second);

    BOOST_REQUIRE(find_result.first != find_result.second);
    BOOST_REQUIRE(*(find_result.first) == *iterator);
}

BOOST_AUTO_TEST_SUITE_END()

