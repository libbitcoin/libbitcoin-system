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

BC_AUTO_TEST_SUITE(arena_tests)

BC_PUSH_WARNING(NO_NEW_OR_DELETE)
BC_PUSH_WARNING(NO_DELETE_RAW_POINTER)

// arena

BC_AUTO_TEST_CASE(arena__allocate1__non_zero__expected_bytes_max_align)
{
    test::mock_arena instance{};
    constexpr auto bytes = 42_size;
    BC_REQUIRE_EQUAL(instance.allocate(bytes), &instance);
    BC_REQUIRE_EQUAL(instance.do_allocate_bytes, bytes);
    BC_REQUIRE_EQUAL(instance.do_allocate_align, alignof(max_align_t));
}

BC_AUTO_TEST_CASE(arena__allocate2__non_zero__expected_bytes_align)
{
    test::mock_arena instance{};
    constexpr auto bytes = 42_size;
    constexpr auto align = 4_size;
    BC_REQUIRE_EQUAL(instance.allocate(bytes, align), &instance);
    BC_REQUIRE_EQUAL(instance.do_allocate_bytes, bytes);
    BC_REQUIRE_EQUAL(instance.do_allocate_align, align);
}

BC_AUTO_TEST_CASE(arena__deallocate2__nullptr_non_zero__expected_ptr_bytes_max_align)
{
    test::mock_arena instance{};
    constexpr auto ptr = nullptr;
    constexpr auto bytes = 42_size;
    instance.deallocate(ptr, bytes);
    BC_REQUIRE_EQUAL(instance.do_deallocate_ptr, ptr);
    BC_REQUIRE_EQUAL(instance.do_deallocate_bytes, bytes);
    BC_REQUIRE_EQUAL(instance.do_deallocate_align, alignof(max_align_t));
}

BC_AUTO_TEST_CASE(arena__deallocate2__nullptr_non_zero__expected_ptr_bytes_align)
{
    test::mock_arena instance{};
    constexpr auto ptr = nullptr;
    constexpr auto bytes = 42_size;
    constexpr auto align = 24_size;
    instance.deallocate(ptr, bytes, align);
    BC_REQUIRE_EQUAL(instance.do_deallocate_ptr, ptr);
    BC_REQUIRE_EQUAL(instance.do_deallocate_bytes, bytes);
    BC_REQUIRE_EQUAL(instance.do_deallocate_align, align);
}

BC_AUTO_TEST_CASE(arena__is_equal__different__expected)
{
    test::mock_arena other{};
    test::mock_arena instance{};
    BC_REQUIRE(!instance.is_equal(other));
    BC_REQUIRE_EQUAL(instance.do_is_equal_address, &other);
}

BC_AUTO_TEST_CASE(arena__is_equal__same__expected)
{
    test::mock_arena instance{};
    BC_REQUIRE(!instance.is_equal(instance));
    BC_REQUIRE_EQUAL(instance.do_is_equal_address, &instance);
}

// default_arena

BC_AUTO_TEST_CASE(default_arena__allocate1__non_zero_bytes__non_null_allocation)
{
    default_arena instance{};
    const auto ptr = instance.allocate(42);
    BC_REQUIRE(!is_null(ptr));

    // Clean up unfaked allocation.
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(default_arena__allocate2__non_zero_bytes_max_align__non_null_allocation)
{
    default_arena instance{};
    const auto ptr = instance.allocate(42, alignof(max_align_t));
    BC_REQUIRE(!is_null(ptr));

    // Clean up unfaked allocation.
    ::operator delete(ptr);
}

// "The effect of dereferencing a pointer returned as a request for zero size is undefined."
BC_AUTO_TEST_CASE(default_arena__allocate__zero_bytes__non_null)
{
    const auto ptr = default_arena{}.allocate(0);
    BC_REQUIRE(!is_null(ptr));

    // Clean up unfaked allocation (even though it was empty this would otherwise be a leak).
    ::operator delete(ptr);
}

BC_AUTO_TEST_CASE(default_arena__deallocate1__non_zero_bytes__does_not_throw)
{
    default_arena instance{};
    constexpr auto bytes = 42_size;
    auto ptr = ::operator new(bytes);
    BC_REQUIRE_NO_THROW(instance.deallocate(ptr, bytes));
    ptr = nullptr;
}

BC_AUTO_TEST_CASE(default_arena__deallocate2__non_zero_bytes_max_align__does_not_throw)
{
    default_arena instance{};
    constexpr auto bytes = 42_size;
    auto ptr = ::operator new(bytes);
    BC_REQUIRE_NO_THROW(instance.deallocate(ptr, bytes, alignof(max_align_t)));
    ptr = nullptr;
}

BC_AUTO_TEST_CASE(default_arena__is_equal__same__true)
{
    test::mock_arena instance{};
    BC_REQUIRE(!instance.is_equal(instance));
}

BC_AUTO_TEST_CASE(default_arena__is_equal__different__false)
{
    test::mock_arena other{};
    test::mock_arena instance{};
    BC_REQUIRE(!instance.is_equal(other));
}

BC_POP_WARNING()
BC_POP_WARNING()

BC_AUTO_TEST_SUITE_END()
