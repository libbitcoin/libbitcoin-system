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

using namespace system::machine;

static_assert(std::variant_size<stack_variant>::value == 3u);
static_assert(is_same_type<std::variant_alternative_t<stack_type::bool_, stack_variant>, bool>);
static_assert(is_same_type<std::variant_alternative_t<stack_type::int64_, stack_variant>, int64_t>);
static_assert(is_same_type<std::variant_alternative_t<stack_type::pchunk_, stack_variant>, chunk_xptr>);

BOOST_AUTO_TEST_SUITE(stack_tests)

// stack_variant is not serializable, so cannot boost compare it.

BOOST_AUTO_TEST_CASE(stack__pop__pushed_bool__expected)
{
    stack<contiguous_stack> stack{};
    stack.push(true);
    BOOST_REQUIRE(stack.pop() == stack_variant{ true });
}

BOOST_AUTO_TEST_CASE(stack__pop__pushed_int64__expected)
{
    stack<contiguous_stack> stack{};
    stack.push(42);
    BOOST_REQUIRE(stack.pop() == stack_variant{ 42 });
}

BOOST_AUTO_TEST_CASE(stack__pop__pushed_chunk__expected)
{
    const auto expected = data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    const chunk_xptr ptr{ expected };
    stack<contiguous_stack> stack{};
    stack.push(data_chunk{ 0x42, 0x43, 0x44, 0x45, 0x46 });
    BOOST_REQUIRE(stack.pop() == stack_variant{ ptr });
}

BOOST_AUTO_TEST_SUITE_END()
