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

BOOST_AUTO_TEST_SUITE(memory_tests)

struct type
{
    static constexpr int expected{ 42 };

    type(int left_=0, int right_=expected) NOEXCEPT
      : left(left_), right(right_) {}

    int left;
    int right;
};

// to_shared

using test_array_shared_ptr = std::shared_ptr<const data_array<3>>;

BOOST_AUTO_TEST_CASE(memory__to_shared1__array_default__default_values)
{
    const test_array_shared_ptr ptr = to_shared<data_array<3>>();
    BOOST_REQUIRE_EQUAL(ptr->at(0), 0);
    BOOST_REQUIRE_EQUAL(ptr->at(1), 0);
    BOOST_REQUIRE_EQUAL(ptr->at(2), 0);
}

BOOST_AUTO_TEST_CASE(memory__to_shared5__array_copy__expected_values)
{
    const data_array<3> copy{ 1, 2, 3 };
    const test_array_shared_ptr ptr = to_shared<data_array<3>>(copy);
    BOOST_REQUIRE_EQUAL(ptr->at(0), 1);
    BOOST_REQUIRE_EQUAL(ptr->at(1), 2);
    BOOST_REQUIRE_EQUAL(ptr->at(2), 3);
}

BOOST_AUTO_TEST_CASE(memory__to_shared5__array_move__expected_values)
{
    const test_array_shared_ptr ptr = to_shared<data_array<3>>(data_array<3>{ 1, 2, 3 });
    BOOST_REQUIRE_EQUAL(ptr->at(0), 1);
    BOOST_REQUIRE_EQUAL(ptr->at(1), 2);
    BOOST_REQUIRE_EQUAL(ptr->at(2), 3);
}

using test_shared_ptr = std::shared_ptr<const type>;

BOOST_AUTO_TEST_CASE(memory__to_shared1__always__default)
{
    const test_shared_ptr ptr = to_shared<type>();
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_shared2__default__expected)
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    const test_shared_ptr ptr = to_shared<type>(new type{});
    BC_POP_WARNING()
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_shared2__values__expected)
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    const test_shared_ptr ptr = to_shared<type>(new type{ 1, 2 });
    BC_POP_WARNING()
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_shared3__default__expected)
{
    const test_shared_ptr ptr = to_shared<type>(type{});
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_shared3__values__expected)
{
    const test_shared_ptr ptr = to_shared<type>({ 1, 2 });
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_shared4__default__expected)
{
    const type instance{};
    const test_shared_ptr ptr = to_shared<type>(instance);
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_shared4__values__expected)
{
    const type instance{ 1, 2 };
    const test_shared_ptr ptr = to_shared<type>(instance);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_shared5__value1__expected)
{
    const test_shared_ptr ptr = to_shared<type>(1);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_shared5__values__expected)
{
    const test_shared_ptr ptr = to_shared<type>(1, 2);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__make_shared__value1__expected)
{
    // This creates a copy of the non-const into a const pointer.
    const test_shared_ptr ptr = make_shared<type>(1);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

// to_non_const_raw_ptr

BOOST_AUTO_TEST_CASE(memory__to_non_const_raw_ptr__always__equals_parameter)
{
    const auto cptr = to_shared<type>(1, 2);
    const auto ptr = to_non_const_raw_ptr(cptr);
    BOOST_REQUIRE_EQUAL(cptr.get(), ptr);
}

// to_shareds

using test_shareds_ptr = std::shared_ptr<std_vector<std::shared_ptr<const type>>>;

BOOST_AUTO_TEST_CASE(memory__to_shareds1__empty__empty)
{
    const test_shareds_ptr ptr = to_shareds<type>({});
    BOOST_REQUIRE(ptr->empty());
}

BOOST_AUTO_TEST_CASE(memory__to_shareds1__non_empty__expected)
{
    const test_shareds_ptr ptr = to_shareds<type>(std_vector<type>{ {}, { 1, 2 } });
    BOOST_REQUIRE_EQUAL(ptr->size(), 2u);
    BOOST_REQUIRE_EQUAL(ptr->at(0)->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->at(0)->right, type::expected);
    BOOST_REQUIRE_EQUAL(ptr->at(1)->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->at(1)->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_shareds2__empty__empty)
{
    const auto instance = std_vector<type>{};
    const test_shareds_ptr ptr = to_shareds<type>(instance);
    BOOST_REQUIRE(ptr->empty());
}

BOOST_AUTO_TEST_CASE(memory__to_shareds2__non_empty__expected)
{
    const auto instance = std_vector<type>{ {}, { 1, 2 } };
    const test_shareds_ptr ptr = to_shareds<type>(instance);
    BOOST_REQUIRE_EQUAL(ptr->size(), 2u);
    BOOST_REQUIRE_EQUAL(ptr->at(0)->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->at(0)->right, type::expected);
    BOOST_REQUIRE_EQUAL(ptr->at(1)->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->at(1)->right, 2);
}

// to_allocated

BOOST_AUTO_TEST_CASE(memory__to_allocated__default_resource_left__expected)
{
    const auto allocator = test::get_default_resource();
    const test_shared_ptr ptr = to_allocated<type>(allocator);
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__default_resource_right__expected)
{
    const auto allocator = test::get_default_resource();
    const test_shared_ptr ptr = to_allocated<type>(allocator, 1);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__default_resource_values__expected)
{
    const auto allocator = test::get_default_resource();
    const test_shared_ptr ptr = to_allocated<type>(allocator, 1, 2);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__test_resource_default__expected)
{
    test::reporting_arena<false> resource{};
    test_shared_ptr ptr = to_allocated<type>(&resource);
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 0u);
    BOOST_REQUIRE_EQUAL(resource.dec_bytes, 0u);
    ptr.reset();
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 1u);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__test_resource_left__expected_allocations)
{
    test::reporting_arena<false> resource{};
    test_shared_ptr ptr = to_allocated<type>(&resource, 1);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 0u);
    BOOST_REQUIRE_EQUAL(resource.dec_bytes, 0u);
    ptr.reset();
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 1u);

    BOOST_REQUIRE_EQUAL(resource.inc_count, resource.dec_count);
    BOOST_REQUIRE_EQUAL(resource.inc_bytes, resource.dec_bytes);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__test_resource_values__expected_allocations)
{
    test::reporting_arena<false> resource{};
    test_shared_ptr ptr = to_allocated<type>(&resource, 1, 2);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 0u);
    BOOST_REQUIRE_EQUAL(resource.dec_bytes, 0u);
    ptr.reset();
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.dec_count, 1u);

    BOOST_REQUIRE_EQUAL(resource.inc_count, resource.dec_count);
    BOOST_REQUIRE_EQUAL(resource.inc_bytes, resource.dec_bytes);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__test_resource_values_non_pmr_vector_push__non_cascading_allocations)
{
    using non_pmr_vector_type = std::vector<type>;
    using test_shared_vector_ptr = std::shared_ptr<const non_pmr_vector_type>;

    test::reporting_arena<false> resource{};
    const std::initializer_list<type> args{ { 1, 2 }, { 3, 4 }, { 5, 6 } };
    test_shared_vector_ptr cptr = to_allocated<non_pmr_vector_type>(&resource, args);
    auto ptr = const_cast<non_pmr_vector_type*>(cptr.get());
    BOOST_REQUIRE(ptr != nullptr);
    BOOST_REQUIRE_EQUAL(ptr->size(), 3u);
    BOOST_REQUIRE_EQUAL(ptr->at(0).left, 1);
    BOOST_REQUIRE_EQUAL(ptr->at(2).right, 6);
    ptr->push_back({ 7, 8 });
    ptr->push_back({ 9, 0 });
    cptr.reset();

    // Only the initial allocation.
    BOOST_REQUIRE_EQUAL(resource.inc_count, 1u);
    BOOST_REQUIRE_EQUAL(resource.inc_count, resource.dec_count);
    BOOST_REQUIRE_EQUAL(resource.inc_bytes, resource.dec_bytes);
}

BOOST_AUTO_TEST_CASE(memory__to_allocated__test_resource_empty_vector__cascading_allocations)
{
    using pmr_vector_type = std_vector<type>;
    using test_shared_vector_ptr = std::shared_ptr<const pmr_vector_type>;

    test::reporting_arena<false> resource{};
    const std::initializer_list<type> args{ { 1, 2 }, { 3, 4 }, { 5, 6 } };
    test_shared_vector_ptr cptr = to_allocated<pmr_vector_type>(&resource, args);
    auto ptr = const_cast<pmr_vector_type*>(cptr.get());
    BOOST_REQUIRE(ptr != nullptr);
    BOOST_REQUIRE_EQUAL(cptr->size(), 3u);
    BOOST_REQUIRE_EQUAL(cptr->at(0).left, 1);
    BOOST_REQUIRE_EQUAL(cptr->at(2).right, 6);
    ptr->push_back({ 7, 8 });
    ptr->push_back({ 9, 0 });
    cptr.reset();

    // The initial allocation plus at least initial vector resize.
    BOOST_REQUIRE_GE(resource.inc_count, 2u);
    BOOST_REQUIRE_EQUAL(resource.inc_count, resource.dec_count);
    BOOST_REQUIRE_EQUAL(resource.inc_bytes, resource.dec_bytes);
}

// to_unique

using test_unique_ptr = std::unique_ptr<const type>;

BOOST_AUTO_TEST_CASE(memory__to_unique1__always__default)
{
    const test_unique_ptr ptr = to_unique<type>();
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_unique2__default__expected)
{
    const test_unique_ptr ptr = to_unique<type>(type{});
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_unique2__values__expected)
{
    const test_unique_ptr ptr = to_unique<type>({ 1, 2 });
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_unique3__default__expected)
{
    const type instance{};
    const test_unique_ptr ptr = to_unique<type>(instance);
    BOOST_REQUIRE_EQUAL(ptr->left, 0);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_unique3__values__expected)
{
    const type instance{ 1, 2 };
    const test_unique_ptr ptr = to_unique<type>(instance);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_CASE(memory__to_unique4__value1__expected)
{
    const test_unique_ptr ptr = to_unique<type>(1);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, type::expected);
}

BOOST_AUTO_TEST_CASE(memory__to_unique4__values__expected)
{
    const test_unique_ptr ptr = to_unique<type>(1, 2);
    BOOST_REQUIRE_EQUAL(ptr->left, 1);
    BOOST_REQUIRE_EQUAL(ptr->right, 2);
}

BOOST_AUTO_TEST_SUITE_END()
