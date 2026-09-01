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
#include "accessor.hpp"

BOOST_AUTO_TEST_SUITE(program_tests)

using namespace system::chain;
using namespace system::machine;

// initialize

BOOST_AUTO_TEST_CASE(program__initialize__empty_input_script__script_success)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE(accessor.initialize() == error::script_success);
}

BOOST_AUTO_TEST_CASE(program__initialize__underflow_input_script__invalid_script)
{
    const script underflow(base16_chunk("4c"), false);
    BOOST_REQUIRE(underflow.is_underflow());

    const signatures capture{};
    const auto tx = accessor_transaction(underflow);
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE(accessor.initialize() == error::invalid_script);
}

// primary stack typing

BOOST_AUTO_TEST_CASE(program__push_signed64__pop_signed32__expected)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_signed64(42);
    BOOST_REQUIRE_EQUAL(accessor.stack_size(), 1u);

    int32_t value{};
    BOOST_REQUIRE(accessor.pop_signed32(value));
    BOOST_REQUIRE_EQUAL(value, 42);
    BOOST_REQUIRE(accessor.is_stack_empty());
}

BOOST_AUTO_TEST_CASE(program__push_bool__peek_bool___expected)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_bool(true);
    BOOST_REQUIRE(accessor.peek_bool_());
}

BOOST_AUTO_TEST_CASE(program__push_chunk__peek_size__expected)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_chunk(data_chunk{ 0x01, 0x02, 0x03 });
    BOOST_REQUIRE_EQUAL(accessor.peek_size(), 3u);
}

BOOST_AUTO_TEST_CASE(program__drop___pushed__empty)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_signed64(1);
    BOOST_REQUIRE(!accessor.is_stack_empty());

    accessor.drop_();
    BOOST_REQUIRE(accessor.is_stack_empty());
}

// alternate stack

BOOST_AUTO_TEST_CASE(program__push_alternate__pop_alternate___expected)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE(accessor.is_alternate_empty());

    accessor.push_alternate(stack_variant{ true });
    BOOST_REQUIRE(!accessor.is_alternate_empty());

    const auto variant = accessor.pop_alternate_();
    BOOST_REQUIRE(variant == stack_variant{ true });
    BOOST_REQUIRE(accessor.is_alternate_empty());
}

// conditional stack

BOOST_AUTO_TEST_CASE(program__begin_if__negative__not_success)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE(accessor.is_balanced());
    BOOST_REQUIRE(accessor.is_success());

    accessor.begin_if(false);
    BOOST_REQUIRE(!accessor.is_balanced());
    BOOST_REQUIRE(!accessor.is_success());

    accessor.else_if_();
    BOOST_REQUIRE(!accessor.is_balanced());
    BOOST_REQUIRE(accessor.is_success());

    accessor.end_if_();
    BOOST_REQUIRE(accessor.is_balanced());
    BOOST_REQUIRE(accessor.is_success());
}

BOOST_AUTO_TEST_SUITE_END()
