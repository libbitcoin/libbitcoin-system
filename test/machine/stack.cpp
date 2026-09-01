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
#include "../test.hpp"

using namespace system::machine;

static_assert(std::variant_size<stack_variant>::value == 3u);
static_assert(is_same_type<std::variant_alternative_t<stack_type::bool_, stack_variant>, bool>);
static_assert(is_same_type<std::variant_alternative_t<stack_type::int64_, stack_variant>, int64_t>);
static_assert(is_same_type<std::variant_alternative_t<stack_type::pchunk_, stack_variant>, chunk_xptr>);

BOOST_AUTO_TEST_SUITE(stack_tests)

// pop/push

BOOST_AUTO_TEST_CASE(stack__pop__contiguous_pushed_bool__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE(instance.pop() == stack_variant{ true });
}

BOOST_AUTO_TEST_CASE(stack__pop__linked_pushed_bool__expected)
{
    stack<linked_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE(instance.pop() == stack_variant{ true });
}

BOOST_AUTO_TEST_CASE(stack__pop__contiguous_pushed_int64__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(42);
    BOOST_REQUIRE(instance.pop() == stack_variant{ 42 });
}

BOOST_AUTO_TEST_CASE(stack__pop__linked_pushed_int64__expected)
{
    stack<linked_stack> instance{};
    instance.push(42);
    BOOST_REQUIRE(instance.pop() == stack_variant{ 42 });
}

BOOST_AUTO_TEST_CASE(stack__pop__contiguous_pushed_chunk__expected)
{
    const auto expected = data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    const chunk_xptr ptr{ expected };
    stack<contiguous_stack> instance{};
    instance.push(data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 });
    BOOST_REQUIRE(instance.pop() == stack_variant{ ptr });
}

BOOST_AUTO_TEST_CASE(stack__pop__linked_pushed_chunk__expected)
{
    const auto expected = data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    const chunk_xptr ptr{ expected };
    stack<linked_stack> instance{};
    instance.push(data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 });
    BOOST_REQUIRE(instance.pop() == stack_variant{ ptr });
}

// top/drop/empty/size

BOOST_AUTO_TEST_CASE(stack__top__contiguous_two_pushed__last)
{
    stack<contiguous_stack> instance{};
    instance.push(1);
    instance.push(2);
    BOOST_REQUIRE(instance.top() == stack_variant{ 2 });
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
}

BOOST_AUTO_TEST_CASE(stack__top__linked_two_pushed__last)
{
    stack<linked_stack> instance{};
    instance.push(1);
    instance.push(2);
    BOOST_REQUIRE(instance.top() == stack_variant{ 2 });
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
}

BOOST_AUTO_TEST_CASE(stack__drop__contiguous_pushed__removed)
{
    stack<contiguous_stack> instance{};
    BOOST_REQUIRE(instance.empty());

    instance.push(1);
    BOOST_REQUIRE(!instance.empty());

    instance.drop();
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE_EQUAL(instance.size(), 0u);
}

BOOST_AUTO_TEST_CASE(stack__drop__linked_pushed__removed)
{
    stack<linked_stack> instance{};
    BOOST_REQUIRE(instance.empty());

    instance.push(1);
    BOOST_REQUIRE(!instance.empty());

    instance.drop();
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE_EQUAL(instance.size(), 0u);
}

// emplace

BOOST_AUTO_TEST_CASE(stack__emplace_boolean__contiguous__expected)
{
    stack<contiguous_stack> instance{};
    instance.emplace_boolean(true);
    BOOST_REQUIRE(instance.pop() == stack_variant{ true });
}

BOOST_AUTO_TEST_CASE(stack__emplace_boolean__linked__expected)
{
    stack<linked_stack> instance{};
    instance.emplace_boolean(true);
    BOOST_REQUIRE(instance.pop() == stack_variant{ true });
}

BOOST_AUTO_TEST_CASE(stack__emplace_integer__contiguous__expected)
{
    stack<contiguous_stack> instance{};
    instance.emplace_integer(42);
    BOOST_REQUIRE(instance.pop() == stack_variant{ 42 });
}

BOOST_AUTO_TEST_CASE(stack__emplace_integer__linked__expected)
{
    stack<linked_stack> instance{};
    instance.emplace_integer(42);
    BOOST_REQUIRE(instance.pop() == stack_variant{ 42 });
}

BOOST_AUTO_TEST_CASE(stack__emplace_chunk__contiguous__expected)
{
    const auto chunk = data_chunk{ 0x2a };
    const chunk_xptr ptr{ chunk };
    stack<contiguous_stack> instance{};
    instance.emplace_chunk(ptr);
    BOOST_REQUIRE(instance.pop() == stack_variant{ ptr });
}

BOOST_AUTO_TEST_CASE(stack__emplace_chunk__linked__expected)
{
    const auto chunk = data_chunk{ 0x2a };
    const chunk_xptr ptr{ chunk };
    stack<linked_stack> instance{};
    instance.emplace_chunk(ptr);
    BOOST_REQUIRE(instance.pop() == stack_variant{ ptr });
}

// positional

BOOST_AUTO_TEST_CASE(stack__peek__contiguous_index__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.push(3);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 3 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 2 });
    BOOST_REQUIRE(instance.peek(2) == stack_variant{ 1 });
}

BOOST_AUTO_TEST_CASE(stack__peek__linked_index__expected)
{
    stack<linked_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.push(3);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 3 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 2 });
    BOOST_REQUIRE(instance.peek(2) == stack_variant{ 1 });
}

BOOST_AUTO_TEST_CASE(stack__swap__contiguous_top_and_second__swapped)
{
    stack<contiguous_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.swap(0, 1);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 1 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 2 });
}

BOOST_AUTO_TEST_CASE(stack__swap__linked_top_and_second__swapped)
{
    stack<linked_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.swap(0, 1);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 1 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 2 });
}

BOOST_AUTO_TEST_CASE(stack__erase__contiguous_middle__removed)
{
    stack<contiguous_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.push(3);
    instance.erase(1);
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 3 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 1 });
}

BOOST_AUTO_TEST_CASE(stack__erase__linked_middle__removed)
{
    stack<linked_stack> instance{};
    instance.push(1);
    instance.push(2);
    instance.push(3);
    instance.erase(1);
    BOOST_REQUIRE_EQUAL(instance.size(), 2u);
    BOOST_REQUIRE(instance.peek(0) == stack_variant{ 3 });
    BOOST_REQUIRE(instance.peek(1) == stack_variant{ 1 });
}

// peek_signed4/peek_signed5

BOOST_AUTO_TEST_CASE(stack__peek_signed4__contiguous_max_int32__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(int64_t{ max_int32 });

    int32_t value{};
    BOOST_REQUIRE(instance.peek_signed4(value));
    BOOST_REQUIRE_EQUAL(value, max_int32);
}

BOOST_AUTO_TEST_CASE(stack__peek_signed4__linked_max_int32__expected)
{
    stack<linked_stack> instance{};
    instance.push(int64_t{ max_int32 });

    int32_t value{};
    BOOST_REQUIRE(instance.peek_signed4(value));
    BOOST_REQUIRE_EQUAL(value, max_int32);
}

BOOST_AUTO_TEST_CASE(stack__peek_signed4__contiguous_five_byte_value__false)
{
    stack<contiguous_stack> instance{};
    instance.push(add1<int64_t>(max_int32));

    int32_t value{};
    BOOST_REQUIRE(!instance.peek_signed4(value));
}

BOOST_AUTO_TEST_CASE(stack__peek_signed4__linked_five_byte_value__false)
{
    stack<linked_stack> instance{};
    instance.push(add1<int64_t>(max_int32));

    int32_t value{};
    BOOST_REQUIRE(!instance.peek_signed4(value));
}

BOOST_AUTO_TEST_CASE(stack__peek_signed5__contiguous_five_byte_value__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(add1<int64_t>(max_int32));

    int64_t value{};
    BOOST_REQUIRE(instance.peek_signed5(value));
    BOOST_REQUIRE_EQUAL(value, add1<int64_t>(max_int32));
}

BOOST_AUTO_TEST_CASE(stack__peek_signed5__linked_five_byte_value__expected)
{
    stack<linked_stack> instance{};
    instance.push(add1<int64_t>(max_int32));

    int64_t value{};
    BOOST_REQUIRE(instance.peek_signed5(value));
    BOOST_REQUIRE_EQUAL(value, add1<int64_t>(max_int32));
}

BOOST_AUTO_TEST_CASE(stack__peek_signed5__contiguous_six_byte_value__false)
{
    stack<contiguous_stack> instance{};
    instance.push(power2<int64_t>(39u));

    int64_t value{};
    BOOST_REQUIRE(!instance.peek_signed5(value));
}

BOOST_AUTO_TEST_CASE(stack__peek_signed5__linked_six_byte_value__false)
{
    stack<linked_stack> instance{};
    instance.push(power2<int64_t>(39u));

    int64_t value{};
    BOOST_REQUIRE(!instance.peek_signed5(value));
}

// variant conversions

BOOST_AUTO_TEST_CASE(stack__peek_bool__contiguous_types__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE(instance.peek_bool());

    instance.push(0);
    BOOST_REQUIRE(!instance.peek_bool());

    instance.push(data_chunk{ 0x02 });
    BOOST_REQUIRE(instance.peek_bool());
}

BOOST_AUTO_TEST_CASE(stack__peek_bool__linked_types__expected)
{
    stack<linked_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE(instance.peek_bool());

    instance.push(0);
    BOOST_REQUIRE(!instance.peek_bool());

    instance.push(data_chunk{ 0x02 });
    BOOST_REQUIRE(instance.peek_bool());
}

BOOST_AUTO_TEST_CASE(stack__peek_strict_bool__contiguous_negative_zero_chunk__true)
{
    stack<contiguous_stack> instance{};
    instance.push(data_chunk{ 0x80 });
    BOOST_REQUIRE(instance.peek_strict_bool());
}

BOOST_AUTO_TEST_CASE(stack__peek_strict_bool__linked_empty_chunk__false)
{
    stack<linked_stack> instance{};
    instance.push(data_chunk{});
    BOOST_REQUIRE(!instance.peek_strict_bool());
}

BOOST_AUTO_TEST_CASE(stack__peek_minimal_bool__contiguous_nonminimal_chunk__false)
{
    stack<contiguous_stack> instance{};
    instance.push(data_chunk{ 0x02 });

    bool value{};
    BOOST_REQUIRE(!instance.peek_minimal_bool(value));
}

BOOST_AUTO_TEST_CASE(stack__peek_minimal_bool__linked_minimal_true__true)
{
    stack<linked_stack> instance{};
    instance.push(data_chunk{ 0x01 });

    bool value{};
    BOOST_REQUIRE(instance.peek_minimal_bool(value));
    BOOST_REQUIRE(value);
}

BOOST_AUTO_TEST_CASE(stack__peek_size__contiguous_types__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 1u);

    instance.push(0);
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 0u);

    instance.push(data_chunk{ 0x01, 0x02, 0x03 });
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 3u);
}

BOOST_AUTO_TEST_CASE(stack__peek_size__linked_types__expected)
{
    stack<linked_stack> instance{};
    instance.push(true);
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 1u);

    instance.push(0);
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 0u);

    instance.push(data_chunk{ 0x01, 0x02, 0x03 });
    BOOST_REQUIRE_EQUAL(instance.peek_size(), 3u);
}

BOOST_AUTO_TEST_CASE(stack__peek_nonempty__contiguous_mixed__expected)
{
    stack<contiguous_stack> instance{};
    instance.push(true);
    instance.push(0);
    instance.push(data_chunk{});
    instance.push(data_chunk{ 0x01 });
    BOOST_REQUIRE_EQUAL(instance.peek_nonempty(), 2u);
}

BOOST_AUTO_TEST_CASE(stack__peek_nonempty__linked_mixed__expected)
{
    stack<linked_stack> instance{};
    instance.push(true);
    instance.push(0);
    instance.push(data_chunk{});
    instance.push(data_chunk{ 0x01 });
    BOOST_REQUIRE_EQUAL(instance.peek_nonempty(), 2u);
}

BOOST_AUTO_TEST_CASE(stack__peek_chunk__contiguous_integer__converted)
{
    stack<contiguous_stack> instance{};
    instance.push(int64_t{ 0x0100 });
    BOOST_REQUIRE_EQUAL(*instance.peek_chunk(), base16_chunk("0001"));
}

BOOST_AUTO_TEST_CASE(stack__peek_chunk__linked_integer__converted)
{
    stack<linked_stack> instance{};
    instance.push(int64_t{ 0x0100 });
    BOOST_REQUIRE_EQUAL(*instance.peek_chunk(), base16_chunk("0001"));
}

// equal_chunks

BOOST_AUTO_TEST_CASE(stack__equal_chunks__bool_and_integer_one__true)
{
    const stack_variant left{ true };
    const stack_variant right{ int64_t{ 1 } };
    BOOST_REQUIRE(stack<contiguous_stack>::equal_chunks(left, right));
}

BOOST_AUTO_TEST_CASE(stack__equal_chunks__false_and_integer_zero__true)
{
    const stack_variant left{ false };
    const stack_variant right{ int64_t{ 0 } };
    BOOST_REQUIRE(stack<contiguous_stack>::equal_chunks(left, right));
}

BOOST_AUTO_TEST_CASE(stack__equal_chunks__unequal_integers__false)
{
    const stack_variant left{ int64_t{ 1 } };
    const stack_variant right{ int64_t{ 2 } };
    BOOST_REQUIRE(!stack<contiguous_stack>::equal_chunks(left, right));
}

BOOST_AUTO_TEST_SUITE_END()
