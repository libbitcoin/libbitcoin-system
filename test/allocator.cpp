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
#include "test.hpp"

BC_AUTO_TEST_SUITE(allocator_tests)

// Doing some low level stuff we shouldn't normally do.
BC_PUSH_WARNING(NO_NEW_OR_DELETE)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_DELETE_RAW_POINTER)
BC_PUSH_WARNING(NO_UNCLEARED_OWNER_POINTER)

struct simple
{
    simple(size_t arg={}) NOEXCEPT
      : value(arg)
    {
    }

    size_t value;
};

struct aggregate
{
    aggregate(size_t arg1={}, size_t arg2={}) NOEXCEPT
      : value1(arg1), value2(arg2)
    {
    }

    size_t value1;
    size_t value2;
};

struct compound
{
    static constexpr data_array<3> null_array{ 0x00, 0x00, 0x00 };
    compound(size_t arg1={}, const data_array<3>& arg2={}) NOEXCEPT
      : value(arg1), bytes(arg2)
    {
    }

    size_t value;
    data_array<3> bytes;
};

struct hierarchy
{
    hierarchy(const compound& arg1={}, const data_chunk& arg2={}) NOEXCEPT
      : contained(arg1), chunk(arg2)
    {
    }

    compound contained;
    data_chunk chunk;
};

struct hierarchy_arena
{
    using chunky = std::vector<uint8_t, allocator<uint8_t>>;

    hierarchy_arena(arena* alloc, const compound& arg1={},
        const chunky& arg2={}) NOEXCEPT
      : contained(arg1), chunk(arg2, alloc)
    {
    }

    compound contained;
    chunky chunk;
};

// std_vector and data_chunk are automatically allocator-injected.
// A std_vector of emplaced shared_ptr will be allocator allocated, with
// constructing pointer, deleter copied to allocation.
static_assert(!std::uses_allocator_v<hierarchy, arena>);
static_assert(!std::uses_allocator_v<std::vector<uint8_t>, allocator<>>);
static_assert(!std::uses_allocator_v<std::array<uint8_t, 42>, allocator<>>);
static_assert(!std::uses_allocator_v<std::shared_ptr<uint8_t>, allocator<>>);
////static_assert(std::uses_allocator_v<std_vector<uint8_t>, allocator<>>);
////static_assert(std::uses_allocator_v<data_chunk, allocator<>>);

///////////////////////////////////////////////////////////////////////////////

// constructor

BC_AUTO_TEST_CASE(allocator__allocator1__default__default_arena)
{
    const allocator instance1{};
    BC_REQUIRE_EQUAL(instance1.resource(), default_arena::get());
}

BC_AUTO_TEST_CASE(allocator__allocator1__resources__same_instance)
{
    const allocator instance1{};
    const allocator instance2{};
    BC_REQUIRE_EQUAL(instance1.resource(), instance2.resource());
}

BC_AUTO_TEST_CASE(allocator__allocator2__always__expected_resource)
{
    default_arena arena1{};
    const allocator instance1{ &arena1 };
    BC_REQUIRE_EQUAL(instance1.resource(), &arena1);
}

// Templated copy construction.
BC_AUTO_TEST_CASE(allocator__allocator3__always__expected_resource)
{
    default_arena arena1{};
    const allocator<data_array<1>> instance1{ &arena1 };
    const allocator<data_array<2>> instance2{ instance1 };
    BC_REQUIRE_EQUAL(instance2.resource(), &arena1);
}

// Default copy construction.
BC_AUTO_TEST_CASE(allocator__allocator4__always__expected_resource)
{
    default_arena arena1{};
    const allocator instance1{ &arena1 };
    const allocator instance2{ instance1 };
    BC_REQUIRE_EQUAL(instance2.resource(), &arena1);
}

// allocate
// test::mock_arena::allocate always returns null.

BC_AUTO_TEST_CASE(allocator__allocate__overflow__throws_bad_array_new_length)
{
    test::mock_arena arena{};
    allocator<int> instance1{ &arena };

    // max_size_t * sizeof(int) overflows.
    BC_REQUIRE_THROW(is_null(instance1.allocate(max_size_t)), bad_array_new_length);
}

BC_AUTO_TEST_CASE(allocator__allocate__max_size__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };
    constexpr auto count = max_size_t;
    BC_REQUIRE_EQUAL((void*)instance1.allocate(count), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, count * sizeof(uint8_t));
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(uint8_t));
}

BC_AUTO_TEST_CASE(allocator__allocate__values__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto count = 42_size;
    BC_REQUIRE_EQUAL((void*)instance1.allocate(count), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, count * sizeof(double));
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(double));
}

// deallocate

BC_AUTO_TEST_CASE(allocator__deallocate__overflow__does_not_throw)
{
    test::mock_arena arena{};
    allocator<int> instance1{ &arena };
    int memory{};

    // max_size_t * sizeof(int) overflows.
    BC_REQUIRE_NO_THROW(instance1.deallocate(&memory, max_size_t));
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, max_size_t * sizeof(int));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(int));
}

BC_AUTO_TEST_CASE(allocator__deallocate__values__do_deallocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto count = 42_size;
    double memory{};
    instance1.deallocate(&memory, count);
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, count * sizeof(double));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(double));
}

// allocate_bytes
// test::mock_arena::allocate returns self if do_allocate is invoked.

BC_AUTO_TEST_CASE(allocator__allocate_bytes1__values__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto bytes = 42_size;
    BC_REQUIRE_EQUAL((void*)instance1.allocate_bytes(bytes), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, bytes);
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(max_align_t));
}

BC_AUTO_TEST_CASE(allocator__allocate_bytes2__values__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto bytes = 42_size;
    constexpr auto align = 32_size;
    BC_REQUIRE_EQUAL((void*)instance1.allocate_bytes(bytes, align), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, bytes);
    BC_REQUIRE_EQUAL(arena.do_allocate_align, align);
}

// deallocate_bytes

BC_AUTO_TEST_CASE(allocator__deallocate_bytes__values__do_deallocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto bytes = 42_size;
    double memory{};
    instance1.deallocate_bytes(&memory, bytes);
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, bytes);
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(max_align_t));
}

BC_AUTO_TEST_CASE(allocator__deallocate_bytes2__values__do_deallocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto bytes = 42_size;
    constexpr auto align = 32_size;
    double memory{};
    instance1.deallocate_bytes(&memory, bytes, align);
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, bytes);
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, align);
}

// allocate_object

BC_AUTO_TEST_CASE(allocator__allocate_object__overflow__throws_bad_array_new_length)
{
    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };

    // max_size_t * sizeof(int) overflows.
    BC_REQUIRE_THROW(is_null(instance1.allocate_object<int>(max_size_t)), bad_array_new_length);
}

BC_AUTO_TEST_CASE(allocator__allocate_object1__values__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto size = 42u;
    using other_type = data_array<size>;
    BC_REQUIRE_EQUAL((void*)instance1.allocate_object<other_type>(), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(other_type));
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, size);
}

BC_AUTO_TEST_CASE(allocator__allocate_object2__values__do_allocate_expected_values)
{
    test::mock_arena arena{};
    allocator<double> instance1{ &arena };
    constexpr auto count = 2u;
    constexpr auto size = 42u;
    using other_type = data_array<size>;
    BC_REQUIRE_EQUAL((void*)instance1.allocate_object<other_type>(count), &arena);
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(other_type));
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, count * size);
}

// deallocate_object

BC_AUTO_TEST_CASE(allocator__deallocate_object__overflow__does_not_throw)
{
    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };
    int memory{};

    // max_size_t * sizeof(int) overflows.
    BC_REQUIRE_NO_THROW(instance1.deallocate_object<int>(&memory, max_size_t));
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, max_size_t * sizeof(int));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(int));
}

BC_AUTO_TEST_CASE(allocator__deallocate_object1__values__do_deallocate_expected_values)
{
    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };
    double memory{};
    instance1.deallocate_object<double>(&memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, sizeof(double));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(double));
}

BC_AUTO_TEST_CASE(allocator__deallocate_object2__values__do_deallocate_expected_values)
{
    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };
    constexpr auto count = 42_size;
    double memory{};
    instance1.deallocate_object<double>(&memory, count);
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &memory);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, count * sizeof(double));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(double));
}

// new_object

BC_AUTO_TEST_CASE(allocator__new_object__contruct_throws__calls_deallocate_and_propagates_exception)
{
    struct thrower
    {
        thrower(const char* text) THROWS
        {
            if (!is_null(text))
                throw overflow_exception(text);
        }
    };

    test::mock_arena arena{};
    allocator<uint8_t> instance1{ &arena };

    // max_size_t * sizeof(int) overflows.
    BC_REQUIRE_THROW(is_null(instance1.new_object<thrower>("overflow")), overflow_exception);

    // Allocated and deallocated despite construction throw.
    BC_REQUIRE_EQUAL(arena.do_allocate_bytes, sizeof(thrower));
    BC_REQUIRE_EQUAL(arena.do_allocate_align, alignof(thrower));

    // Deallocate is invoked by the construct_guard.
    BC_REQUIRE_EQUAL(arena.do_deallocate_ptr, &arena);
    BC_REQUIRE_EQUAL(arena.do_deallocate_bytes, sizeof(thrower));
    BC_REQUIRE_EQUAL(arena.do_deallocate_align, alignof(thrower));
}

// select_on_container_copy_construction

BC_AUTO_TEST_CASE(allocator__select_on_container_copy_construction__always__contructs_default)
{
    test::mock_arena arena{};
    const allocator instance1{ &arena };
    const auto instance2 = instance1.select_on_container_copy_construction();
    BC_REQUIRE_EQUAL(instance1.resource(), &arena);
    BC_REQUIRE_EQUAL(instance2.resource(), default_arena::get());
    BC_REQUIRE_NE(instance1.resource(), default_arena::get());
    BC_REQUIRE_NE(instance2.resource(), &arena);
}

// resource

BC_AUTO_TEST_CASE(allocator__resource__default__default)
{
    BC_REQUIRE_EQUAL(allocator{}.resource(), default_arena::get());
}

BC_AUTO_TEST_CASE(allocator__resource__non_default__expected)
{
    test::mock_arena arena{};
    BC_REQUIRE_EQUAL(allocator{ &arena }.resource(), &arena);
}

// equality

BC_AUTO_TEST_CASE(allocator__equality__different_arena__false)
{
    default_arena arena1{};
    test::reporting_arena<false> arena2{};
    const allocator instance1{ &arena1 };
    const allocator instance2{ &arena2 };
    BC_REQUIRE(!(instance1 == instance2));
}

BC_AUTO_TEST_CASE(allocator__equality__same_arena__true)
{
    default_arena arena{};
    const allocator instance1{ &arena };
    const allocator instance2{ &arena };
    BC_REQUIRE(instance1 == instance2);
}

// new_object/construct
// new_object<Type>(args) and construct<Type>(ptr, args) are not mocked.

BC_AUTO_TEST_CASE(allocator__new_object__integral_type__expected)
{
    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    constexpr auto expected_value = 42;
    const auto ptr = instance1.new_object<int>(expected_value);
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(*ptr, expected_value);
    BC_REQUIRE_EQUAL(arena.inc_count, one);
    BC_REQUIRE_EQUAL(arena.dec_count, zero);
    BC_REQUIRE_EQUAL(arena.inc_bytes, sizeof(int));
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(allocator__new_object__simple_struct__expected)
{

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    const auto ptr = instance1.new_object<simple>(42);
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(ptr->value, 42u);
    BC_REQUIRE_EQUAL(arena.inc_count, one);
    BC_REQUIRE_EQUAL(arena.dec_count, zero);
    BC_REQUIRE_EQUAL(arena.inc_bytes, sizeof(size_t));
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(allocator__new_object__aggregate_struct__expected)
{
    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    const auto ptr = instance1.new_object<aggregate>(42, 24);
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(ptr->value1, 42u);
    BC_REQUIRE_EQUAL(ptr->value2, 24u);
    BC_REQUIRE_EQUAL(arena.inc_count, one);
    BC_REQUIRE_EQUAL(arena.dec_count, zero);
    BC_REQUIRE_EQUAL(arena.inc_bytes, 2 * sizeof(size_t));
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(allocator__new_object__compound_struct__expected)
{
    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    const auto ptr = instance1.new_object<compound>();
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(ptr->value, 0u);
    BC_REQUIRE_EQUAL(ptr->bytes, compound::null_array);
    BC_REQUIRE_EQUAL(arena.inc_count, one);
    BC_REQUIRE_EQUAL(arena.dec_count, zero);
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(allocator__new_object__hierarchy_struct__expected)
{
    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    const auto ptr = instance1.new_object<hierarchy>(42);
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(ptr->contained.value, 42u);
    BC_REQUIRE_EQUAL(ptr->contained.bytes, compound::null_array);
    BC_REQUIRE(ptr->chunk.empty());
    BC_REQUIRE_EQUAL(arena.dec_count, zero);
    BC_REQUIRE_EQUAL(arena.inc_count, one);

    // data_chunk is unlinked from instance1.resource(). 
    const auto before = arena.inc_bytes;
    ptr->chunk.push_back(0X01);
    ptr->chunk.push_back(0X02);
    ptr->chunk.push_back(0X03);
    BC_REQUIRE_EQUAL(arena.inc_bytes, before);
    BC_REQUIRE_EQUAL(arena.inc_count, one);

    // Destructor invokes dynamic deallocations (otherwise they leak).
    ptr->~hierarchy();
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(allocator__new_object__hierarchy_arena_struct__expected)
{
    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    const auto ptr = instance1.new_object<hierarchy_arena>(instance1.resource(), 42);
    BC_REQUIRE(!is_null(ptr));
    BC_REQUIRE_EQUAL(ptr->contained.value, 42u);
    BC_REQUIRE_EQUAL(ptr->contained.bytes, compound::null_array);
    BC_REQUIRE(ptr->chunk.empty());
    BC_REQUIRE_EQUAL(arena.dec_count, 0u);

    // This is 2 on msvc and 1 on gcc/clang.
    BC_REQUIRE_GT(arena.inc_count, 0u);

    // data_chunk is linked via instance1.resource() passed via construct. 
    const auto before = arena.inc_bytes;
    ptr->chunk.push_back(0X01);
    ptr->chunk.push_back(0X02);
    ptr->chunk.push_back(0X03);
    BC_REQUIRE_GT(arena.inc_bytes, before);
    BC_REQUIRE_GT(arena.inc_count, 3u);

    // Destructor invokes dynamic deallocations (otherwise they leak).
    ptr->~hierarchy_arena();
    ::operator delete(ptr);
}

// delete_object/destroy
// delete_object<Type>(ptr) and destroy<Type>(ptr) are not mocked.

BC_AUTO_TEST_CASE(allocator__delete_object__integral_type__expected)
{
    const auto ptr = ::new int{ 42 };
    BC_REQUIRE(!is_null(ptr));

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    instance1.delete_object<int>(ptr);
    BC_REQUIRE_EQUAL(arena.inc_count, zero);
    BC_REQUIRE_EQUAL(arena.dec_count, one);
    BC_REQUIRE_EQUAL(arena.dec_bytes, sizeof(int));
}

BC_AUTO_TEST_CASE(allocator__delete_object__simple_struct__expected)
{
    const auto ptr = ::new simple{ 42 };
    BC_REQUIRE(!is_null(ptr));

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    instance1.delete_object<simple>(ptr);
    BC_REQUIRE_EQUAL(arena.inc_count, zero);
    BC_REQUIRE_EQUAL(arena.dec_count, one);
    BC_REQUIRE_EQUAL(arena.dec_bytes, sizeof(size_t));
}

BC_AUTO_TEST_CASE(allocator__delete_object__aggregate_struct__expected)
{
    const auto ptr = ::new aggregate{ 42, 24 };
    BC_REQUIRE(!is_null(ptr));

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    instance1.delete_object<aggregate>(ptr);
    BC_REQUIRE_EQUAL(arena.inc_count, zero);
    BC_REQUIRE_EQUAL(arena.dec_count, one);
    BC_REQUIRE_EQUAL(arena.dec_bytes, 2 * sizeof(size_t));
}

BC_AUTO_TEST_CASE(allocator__delete_object__compound_struct__expected)
{
    const auto ptr = ::new compound{ 42, { 0x01, 0x02, 0x03 } };
    BC_REQUIRE(!is_null(ptr));

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    instance1.delete_object<compound>(ptr);
    BC_REQUIRE_EQUAL(arena.inc_count, zero);
    BC_REQUIRE_EQUAL(arena.dec_count, one);
}

BC_AUTO_TEST_CASE(allocator__delete_object__hierarchy_struct__expected)
{
    // data_chunk (vector) element allocations are independent of allocator
    // because the allocator doesn't propagate into the struct elements,
    // because the struct does not satisfy std::uses_allocator_v.
    const auto ptr = ::new hierarchy{ { 42, { 0x01, 0x02, 0x03 } }, { 0x24, 0x25 } };
    BC_REQUIRE(!is_null(ptr));

    test::reporting_arena<false> arena{};
    allocator instance1{ &arena };
    instance1.delete_object<hierarchy>(ptr);
    BC_REQUIRE_EQUAL(arena.inc_count, zero);
    BC_REQUIRE_EQUAL(arena.dec_count, one);
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

BC_AUTO_TEST_SUITE_END()

// This is where the magic happens.
////template <class Type, class Allocator, class... Types>
////constexpr auto uses_allocator_construction_args(const Allocator& alloc, Types&&... args) noexcept
////{
////    if constexpr (!std::uses_allocator_v<remove_cv_t<Type>, Allocator>)
////    {
////        static_assert(std::is_constructible_v<Type, Types...>, "If std::uses_allocator_v<std::remove_cv_t<T>, Alloc> is false, T must be constructible from Types...");
////
////        (void)alloc;
////        return std::forward_as_tuple(std::forward<Types>(args)...);
////    }
////    else if constexpr (std::is_constructible_v<Type, std::allocator_arg_t, const Allocator&, Types...>)
////    {
////        using return_type = std::tuple<std::allocator_arg_t, const Allocator&, Types&&...>;
////        return return_type{ std::allocator_arg, alloc, std::forward<Types>(args)... };
////    }
////    else if constexpr (std::is_constructible_v<Type, Types..., const Allocator&>)
////    {
////        return std::forward_as_tuple(std::forward<Types>(args)..., alloc);
////    }
////    else
////    {
////        static_assert(false, "T must be constructible from either (std::allocator_arg_t, const Alloc&, Types...) or (Types..., const Alloc&) if std::uses_allocator_v<std::remove_cv_t<T>, Alloc> is true");
////    }
////}
