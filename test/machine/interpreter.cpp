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

BC_AUTO_TEST_SUITE(interpreter_tests)

using namespace system::chain;
using namespace system::machine;

// Push operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_push_number__positive__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_push_number(5) }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 5);
}

BC_AUTO_TEST_CASE(interpreter__op_push_number__negative__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_push_number(-1) }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -1);
}

BC_AUTO_TEST_CASE(interpreter__op_push_size__claimed__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ data_chunk{ 0x01, 0x02, 0x03 }, false };
    BC_REQUIRE(!op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_size(op) }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("010203"));
}

BC_AUTO_TEST_CASE(interpreter__op_push_size__underclaimed__op_push_size)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("03ffff") };
    BC_REQUIRE(op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_size(op) }, error::op_push_size);
}

BC_AUTO_TEST_CASE(interpreter__op_push_one_size__claimed__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4c03010203") };
    BC_REQUIRE(!op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_one_size(op) }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("010203"));
}

BC_AUTO_TEST_CASE(interpreter__op_push_one_size__underclaimed__op_push_one_size)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4c04ffff") };
    BC_REQUIRE(op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_one_size(op) }, error::op_push_one_size);
}

BC_AUTO_TEST_CASE(interpreter__op_push_two_size__claimed__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4d0300010203") };
    BC_REQUIRE(!op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_two_size(op) }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("010203"));
}

BC_AUTO_TEST_CASE(interpreter__op_push_two_size__underclaimed__op_push_two_size)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4d0400ffff") };
    BC_REQUIRE(op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_two_size(op) }, error::op_push_two_size);
}

BC_AUTO_TEST_CASE(interpreter__op_push_four_size__claimed__pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4e03000000010203") };
    BC_REQUIRE(!op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_four_size(op) }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("010203"));
}

BC_AUTO_TEST_CASE(interpreter__op_push_four_size__underclaimed__op_push_four_size)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    const operation op{ base16_chunk("4e04000000ffff") };
    BC_REQUIRE(op.is_underclaimed());
    BC_REQUIRE_EQUAL(code{ machine->op_push_four_size(op) }, error::op_push_four_size);
}

// Reserved and disabled operations.
// ----------------------------------------------------------------------------
// op_nop retains both fork-history branches (currently both succeed).

BC_AUTO_TEST_CASE(interpreter__op_unevaluated__invalid_code__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_unevaluated(opcode::op_cat) }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_unevaluated__reserved_code__op_reserved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_unevaluated(opcode::op_ver) }, error::op_reserved);
}

BC_AUTO_TEST_CASE(interpreter__op_nop_code__always__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_nop(opcode::nop4) }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_nop__always__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_nop() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_ver__always__op_reserved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_ver() }, error::op_reserved);
}

BC_AUTO_TEST_CASE(interpreter__op_verif__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_verif() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_vernotif__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_vernotif() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_return__always__op_reserved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_return() }, error::op_reserved);
}

BC_AUTO_TEST_CASE(interpreter__op_cat__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_cat() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_substr__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_substr() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_left__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_left() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_right__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_right() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_invert__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_invert() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_and__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_and() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_or__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_or() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_xor__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_xor() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_mul2__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_mul2() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_div2__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_div2() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_mul__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_mul() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_div__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_div() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_mod__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_mod() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_lshift__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_lshift() }, error::op_invalid);
}

BC_AUTO_TEST_CASE(interpreter__op_rshift__always__op_invalid)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_rshift() }, error::op_invalid);
}

// Conditional operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_if__true__positive_scope_popped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    BC_REQUIRE_EQUAL(code{ machine->op_if() }, error::op_success);
    BC_REQUIRE(machine->is_success());
    BC_REQUIRE(!machine->is_balanced());
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_if__false__negative_scope)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(false);
    BC_REQUIRE_EQUAL(code{ machine->op_if() }, error::op_success);
    BC_REQUIRE(!machine->is_success());
    BC_REQUIRE_EQUAL(code{ machine->op_endif() }, error::op_success);
    BC_REQUIRE(machine->is_balanced());
}

BC_AUTO_TEST_CASE(interpreter__op_if__empty_stack__op_if1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_if() }, error::op_if1);
}

BC_AUTO_TEST_CASE(interpreter__op_if__negative_scope__not_popped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->begin_if(false);
    BC_REQUIRE_EQUAL(code{ machine->op_if() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
    BC_REQUIRE_EQUAL(code{ machine->op_endif() }, error::op_success);
    BC_REQUIRE_EQUAL(code{ machine->op_endif() }, error::op_success);
    BC_REQUIRE(machine->is_balanced());
}

BC_AUTO_TEST_CASE(interpreter__op_notif__true__negative_scope)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    BC_REQUIRE_EQUAL(code{ machine->op_notif() }, error::op_success);
    BC_REQUIRE(!machine->is_success());
}

BC_AUTO_TEST_CASE(interpreter__op_notif__false__positive_scope)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(false);
    BC_REQUIRE_EQUAL(code{ machine->op_notif() }, error::op_success);
    BC_REQUIRE(machine->is_success());
}

BC_AUTO_TEST_CASE(interpreter__op_notif__empty_stack__op_notif1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_notif() }, error::op_notif1);
}

BC_AUTO_TEST_CASE(interpreter__op_else__no_scope__op_else)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_else() }, error::op_else);
}

BC_AUTO_TEST_CASE(interpreter__op_else__negative_scope__toggled)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->begin_if(false);
    BC_REQUIRE(!machine->is_success());
    BC_REQUIRE_EQUAL(code{ machine->op_else() }, error::op_success);
    BC_REQUIRE(machine->is_success());
    BC_REQUIRE_EQUAL(code{ machine->op_else() }, error::op_success);
    BC_REQUIRE(!machine->is_success());
}

BC_AUTO_TEST_CASE(interpreter__op_endif__balanced__op_endif)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_endif() }, error::op_endif);
}

BC_AUTO_TEST_CASE(interpreter__op_verify__empty_stack__op_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_verify() }, error::op_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_verify__false__op_verify2_not_dropped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(false);
    BC_REQUIRE_EQUAL(code{ machine->op_verify() }, error::op_verify2);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
}

BC_AUTO_TEST_CASE(interpreter__op_verify__true__dropped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    BC_REQUIRE_EQUAL(code{ machine->op_verify() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

// Stack operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_to_alt_stack__value__moved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_to_alt_stack() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
    BC_REQUIRE(!machine->is_alternate_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_to_alt_stack__empty_stack__op_to_alt_stack)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_to_alt_stack() }, error::op_to_alt_stack);
}

BC_AUTO_TEST_CASE(interpreter__op_from_alt_stack__value__returned)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_to_alt_stack() }, error::op_success);
    BC_REQUIRE_EQUAL(code{ machine->op_from_alt_stack() }, error::op_success);
    BC_REQUIRE(machine->is_alternate_empty());

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 7);
}

BC_AUTO_TEST_CASE(interpreter__op_from_alt_stack__empty_alternate__op_from_alt_stack)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_from_alt_stack() }, error::op_from_alt_stack);
}

BC_AUTO_TEST_CASE(interpreter__op_drop2__two__both_dropped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_drop2() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_drop2__one__op_drop2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_drop2() }, error::op_drop2);
}

BC_AUTO_TEST_CASE(interpreter__op_dup2__two__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_dup2() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 4u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE_EQUAL(value1, 2);
    BC_REQUIRE_EQUAL(value2, 1);
    BC_REQUIRE_EQUAL(value3, 2);
    BC_REQUIRE_EQUAL(value4, 1);
}

BC_AUTO_TEST_CASE(interpreter__op_dup2__one__op_dup2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_dup2() }, error::op_dup2);
}

BC_AUTO_TEST_CASE(interpreter__op_dup3__three__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_dup3() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 6u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    int32_t value5{};
    int32_t value6{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE(machine->pop_signed32(value5));
    BC_REQUIRE(machine->pop_signed32(value6));
    BC_REQUIRE_EQUAL(value1, 3);
    BC_REQUIRE_EQUAL(value2, 2);
    BC_REQUIRE_EQUAL(value3, 1);
    BC_REQUIRE_EQUAL(value4, 3);
    BC_REQUIRE_EQUAL(value5, 2);
    BC_REQUIRE_EQUAL(value6, 1);
}

BC_AUTO_TEST_CASE(interpreter__op_dup3__two__op_dup3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_dup3() }, error::op_dup3);
}

BC_AUTO_TEST_CASE(interpreter__op_over2__four__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    machine->push_signed64(4);
    BC_REQUIRE_EQUAL(code{ machine->op_over2() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 6u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    int32_t value5{};
    int32_t value6{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE(machine->pop_signed32(value5));
    BC_REQUIRE(machine->pop_signed32(value6));
    BC_REQUIRE_EQUAL(value1, 2);
    BC_REQUIRE_EQUAL(value2, 1);
    BC_REQUIRE_EQUAL(value3, 4);
    BC_REQUIRE_EQUAL(value4, 3);
    BC_REQUIRE_EQUAL(value5, 2);
    BC_REQUIRE_EQUAL(value6, 1);
}

BC_AUTO_TEST_CASE(interpreter__op_over2__three__op_over2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_over2() }, error::op_over2);
}

BC_AUTO_TEST_CASE(interpreter__op_rot2__six__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    machine->push_signed64(4);
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_rot2() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 6u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    int32_t value5{};
    int32_t value6{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE(machine->pop_signed32(value5));
    BC_REQUIRE(machine->pop_signed32(value6));
    BC_REQUIRE_EQUAL(value1, 1);
    BC_REQUIRE_EQUAL(value2, 0);
    BC_REQUIRE_EQUAL(value3, 5);
    BC_REQUIRE_EQUAL(value4, 4);
    BC_REQUIRE_EQUAL(value5, 3);
    BC_REQUIRE_EQUAL(value6, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_rot2__five__op_rot2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    machine->push_signed64(4);
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_rot2() }, error::op_rot2);
}

BC_AUTO_TEST_CASE(interpreter__op_swap2__four__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    machine->push_signed64(4);
    BC_REQUIRE_EQUAL(code{ machine->op_swap2() }, error::op_success);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE_EQUAL(value1, 2);
    BC_REQUIRE_EQUAL(value2, 1);
    BC_REQUIRE_EQUAL(value3, 4);
    BC_REQUIRE_EQUAL(value4, 3);
}

BC_AUTO_TEST_CASE(interpreter__op_swap2__three__op_swap2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_swap2() }, error::op_swap2);
}

BC_AUTO_TEST_CASE(interpreter__op_if_dup__true__duplicated)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_if_dup() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 2u);
}

BC_AUTO_TEST_CASE(interpreter__op_if_dup__false__not_duplicated)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_if_dup() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
}

BC_AUTO_TEST_CASE(interpreter__op_if_dup__empty_stack__op_if_dup)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_if_dup() }, error::op_if_dup);
}

BC_AUTO_TEST_CASE(interpreter__op_depth__empty__zero)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_depth() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 0);
}

BC_AUTO_TEST_CASE(interpreter__op_depth__two__two)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_depth() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_drop__value__dropped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_drop() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_drop__empty_stack__op_drop)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_drop() }, error::op_drop);
}

BC_AUTO_TEST_CASE(interpreter__op_dup__chunk__duplicated)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x2a });
    BC_REQUIRE_EQUAL(code{ machine->op_dup() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 2u);

    const auto top = machine->pop_chunk_();
    const auto next = machine->pop_chunk_();
    BC_REQUIRE_EQUAL(*top, *next);
    BC_REQUIRE_EQUAL(*top, data_chunk{ 0x2a });
}

BC_AUTO_TEST_CASE(interpreter__op_dup__empty_stack__op_dup)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_dup() }, error::op_dup);
}

BC_AUTO_TEST_CASE(interpreter__op_nip__two__second_removed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_nip() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 8);
}

BC_AUTO_TEST_CASE(interpreter__op_nip__one__op_nip)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_nip() }, error::op_nip);
}

BC_AUTO_TEST_CASE(interpreter__op_over__two__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_over() }, error::op_success);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE_EQUAL(value1, 7);
    BC_REQUIRE_EQUAL(value2, 8);
    BC_REQUIRE_EQUAL(value3, 7);
}

BC_AUTO_TEST_CASE(interpreter__op_over__one__op_over)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_over() }, error::op_over);
}

BC_AUTO_TEST_CASE(interpreter__op_pick__index_two__copied)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(10);
    machine->push_signed64(20);
    machine->push_signed64(30);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_pick() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 4u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    int32_t value4{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE(machine->pop_signed32(value4));
    BC_REQUIRE_EQUAL(value1, 10);
    BC_REQUIRE_EQUAL(value2, 30);
    BC_REQUIRE_EQUAL(value3, 20);
    BC_REQUIRE_EQUAL(value4, 10);
}

BC_AUTO_TEST_CASE(interpreter__op_pick__index_out_of_range__op_pick)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(10);
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_pick() }, error::op_pick);
}

BC_AUTO_TEST_CASE(interpreter__op_pick__negative_index__op_pick)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(10);
    machine->push_signed64(-1);
    BC_REQUIRE_EQUAL(code{ machine->op_pick() }, error::op_pick);
}

BC_AUTO_TEST_CASE(interpreter__op_roll__index_two__moved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(10);
    machine->push_signed64(20);
    machine->push_signed64(30);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_roll() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 3u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE_EQUAL(value1, 10);
    BC_REQUIRE_EQUAL(value2, 30);
    BC_REQUIRE_EQUAL(value3, 20);
}

BC_AUTO_TEST_CASE(interpreter__op_roll__index_out_of_range__op_roll)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(10);
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_roll() }, error::op_roll);
}

BC_AUTO_TEST_CASE(interpreter__op_rot__three__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_rot() }, error::op_success);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE_EQUAL(value1, 1);
    BC_REQUIRE_EQUAL(value2, 3);
    BC_REQUIRE_EQUAL(value3, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_rot__two__op_rot)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_rot() }, error::op_rot);
}

BC_AUTO_TEST_CASE(interpreter__op_swap__two__swapped)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_swap() }, error::op_success);

    int32_t value1{};
    int32_t value2{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE_EQUAL(value1, 1);
    BC_REQUIRE_EQUAL(value2, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_swap__one__op_swap)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_swap() }, error::op_swap);
}

BC_AUTO_TEST_CASE(interpreter__op_tuck__two__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_tuck() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 3u);

    int32_t value1{};
    int32_t value2{};
    int32_t value3{};
    BC_REQUIRE(machine->pop_signed32(value1));
    BC_REQUIRE(machine->pop_signed32(value2));
    BC_REQUIRE(machine->pop_signed32(value3));
    BC_REQUIRE_EQUAL(value1, 2);
    BC_REQUIRE_EQUAL(value2, 1);
    BC_REQUIRE_EQUAL(value3, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_tuck__one__op_tuck)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_tuck() }, error::op_tuck);
}

BC_AUTO_TEST_CASE(interpreter__op_size__chunk__length_pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x02, 0x03 });
    BC_REQUIRE_EQUAL(code{ machine->op_size() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 2u);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 3);
    BC_REQUIRE_EQUAL(machine->peek_size(), 3u);
}

BC_AUTO_TEST_CASE(interpreter__op_size__empty_stack__op_size)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_size() }, error::op_size);
}

// Equality operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_equal__same_chunks__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x2a });
    machine->push_chunk(data_chunk{ 0x2a });
    BC_REQUIRE_EQUAL(code{ machine->op_equal() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_equal__cross_type__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x01 });
    BC_REQUIRE_EQUAL(code{ machine->op_equal() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_equal__different_chunks__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_equal() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_equal__one__op_equal)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_equal() }, error::op_equal);
}

BC_AUTO_TEST_CASE(interpreter__op_equal_verify__same__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x2a });
    machine->push_chunk(data_chunk{ 0x2a });
    BC_REQUIRE_EQUAL(code{ machine->op_equal_verify() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_equal_verify__different__op_equal_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_equal_verify() }, error::op_equal_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_equal_verify__one__op_equal_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_equal_verify() }, error::op_equal_verify1);
}

// Arithmetic operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_add1__value__incremented)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(41);
    BC_REQUIRE_EQUAL(code{ machine->op_add1() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 42);
}

BC_AUTO_TEST_CASE(interpreter__op_add1__negative__incremented)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-2);
    BC_REQUIRE_EQUAL(code{ machine->op_add1() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -1);
}

BC_AUTO_TEST_CASE(interpreter__op_add1__max_int32__five_byte_result)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(max_int32);
    BC_REQUIRE_EQUAL(code{ machine->op_add1() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->peek_size(), 5u);
}

BC_AUTO_TEST_CASE(interpreter__op_add1__five_byte_operand__op_add1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(add1<int64_t>(max_int32));
    BC_REQUIRE_EQUAL(code{ machine->op_add1() }, error::op_add1);
}

BC_AUTO_TEST_CASE(interpreter__op_add1__empty_stack__op_add1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_add1() }, error::op_add1);
}

BC_AUTO_TEST_CASE(interpreter__op_sub1__value__decremented)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(43);
    BC_REQUIRE_EQUAL(code{ machine->op_sub1() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 42);
}

BC_AUTO_TEST_CASE(interpreter__op_sub1__negative__decremented)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-1);
    BC_REQUIRE_EQUAL(code{ machine->op_sub1() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -2);
}

BC_AUTO_TEST_CASE(interpreter__op_sub1__empty_stack__op_sub1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_sub1() }, error::op_sub1);
}

BC_AUTO_TEST_CASE(interpreter__op_negate__positive__negative)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(42);
    BC_REQUIRE_EQUAL(code{ machine->op_negate() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -42);
}

BC_AUTO_TEST_CASE(interpreter__op_negate__empty_stack__op_negate)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_negate() }, error::op_negate);
}

BC_AUTO_TEST_CASE(interpreter__op_abs__negative__positive)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-42);
    BC_REQUIRE_EQUAL(code{ machine->op_abs() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 42);
}

BC_AUTO_TEST_CASE(interpreter__op_abs__empty_stack__op_abs)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_abs() }, error::op_abs);
}

BC_AUTO_TEST_CASE(interpreter__op_not__zero__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_not() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_not__nonzero__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_not() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_not__empty_stack__op_not)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_not() }, error::op_not);
}

BC_AUTO_TEST_CASE(interpreter__op_nonzero__nonzero__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-3);
    BC_REQUIRE_EQUAL(code{ machine->op_nonzero() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_nonzero__zero__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_nonzero() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_add__two_and_three__five)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_add() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 5);
}

BC_AUTO_TEST_CASE(interpreter__op_add__negative_result__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-2);
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_add() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -1);
}

BC_AUTO_TEST_CASE(interpreter__op_add__five_byte_operand__op_add)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(add1<int64_t>(max_int32));
    BC_REQUIRE_EQUAL(code{ machine->op_add() }, error::op_add);
}

BC_AUTO_TEST_CASE(interpreter__op_add__one__op_add)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_add() }, error::op_add);
}

BC_AUTO_TEST_CASE(interpreter__op_sub__five_less_three__two)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(5);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_sub() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_sub__three_less_five__negative_two)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(3);
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_sub() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -2);
}

BC_AUTO_TEST_CASE(interpreter__op_sub__one__op_sub)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_sub() }, error::op_sub);
}

BC_AUTO_TEST_CASE(interpreter__op_bool_and__both_nonzero__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(-1);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_and() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_bool_and__one_zero__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_and() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_bool_or__one_zero__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_or() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_bool_or__both_zero__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_or() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal__equal__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal__unequal__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal_verify__equal__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal_verify() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal_verify__unequal__op_num_equal_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal_verify() }, error::op_num_equal_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal_verify__one__op_num_equal_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal_verify() }, error::op_num_equal_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_num_not_equal__unequal__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(8);
    BC_REQUIRE_EQUAL(code{ machine->op_num_not_equal() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_num_not_equal__equal__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(7);
    BC_REQUIRE_EQUAL(code{ machine->op_num_not_equal() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_less_than__lesser__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_less_than__equal__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than__greater__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than__equal__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_less_than_or_equal__equal__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than_or_equal() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_less_than_or_equal__greater__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(3);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than_or_equal() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than_or_equal__equal__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(2);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than_or_equal() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than_or_equal__lesser__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than_or_equal() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_min__two_values__lesser)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_min() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -1);
}

BC_AUTO_TEST_CASE(interpreter__op_max__two_values__greater)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_max() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 2);
}

BC_AUTO_TEST_CASE(interpreter__op_within__inside__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(1);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_within() }, error::op_success);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_within__at_upper__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(3);
    machine->push_signed64(1);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_within() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_within__below_lower__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0);
    machine->push_signed64(1);
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_within() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_within__two__op_within)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_within() }, error::op_within);
}

// Hashing operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_ripemd160__empty_chunk__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BC_REQUIRE_EQUAL(code{ machine->op_ripemd160() }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("9c1185a5c5e9fc54612808977ee8f548b2258d31"));
}

BC_AUTO_TEST_CASE(interpreter__op_ripemd160__empty_stack__op_ripemd160)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_ripemd160() }, error::op_ripemd160);
}

BC_AUTO_TEST_CASE(interpreter__op_sha1__empty_chunk__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BC_REQUIRE_EQUAL(code{ machine->op_sha1() }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("da39a3ee5e6b4b0d3255bfef95601890afd80709"));
}

BC_AUTO_TEST_CASE(interpreter__op_sha1__empty_stack__op_sha1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_sha1() }, error::op_sha1);
}

BC_AUTO_TEST_CASE(interpreter__op_sha256__empty_chunk__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BC_REQUIRE_EQUAL(code{ machine->op_sha256() }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
}

BC_AUTO_TEST_CASE(interpreter__op_sha256__empty_stack__op_sha256)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_sha256() }, error::op_sha256);
}

BC_AUTO_TEST_CASE(interpreter__op_hash160__empty_chunk__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BC_REQUIRE_EQUAL(code{ machine->op_hash160() }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("b472a266d0bd89c13706a4132ccfb16f7c3b9fcb"));
}

BC_AUTO_TEST_CASE(interpreter__op_hash160__empty_stack__op_hash160)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_hash160() }, error::op_hash160);
}

BC_AUTO_TEST_CASE(interpreter__op_hash256__empty_chunk__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BC_REQUIRE_EQUAL(code{ machine->op_hash256() }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("5df6e0e2761359d30a8275058e299fcc0381534545f55cf43e41983f5d4c9456"));
}

BC_AUTO_TEST_CASE(interpreter__op_hash256__empty_stack__op_hash256)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_hash256() }, error::op_hash256);
}

// Signature operations.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_codeseparator__second_op__offset_advanced)
{
    const script ops_script{ operations{ operation{ opcode::nop }, operation{ opcode::codeseparator }, operation{ opcode::nop } } };
    machine_accessor<contiguous_stack> machine{ ops_script, flags::all_rules };
    const auto separator = std::next(machine->begin());
    BC_REQUIRE_EQUAL(code{ machine->op_codeseparator(separator) }, error::op_success);

    const auto& input_script = *machine.transaction().inputs_ptr()->front()->script_ptr();
    const auto expected = std::next(input_script.ops().cbegin(), 2);
    BC_REQUIRE(input_script.offset == expected);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__one__op_check_sig_low_stack)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_check_sig_low_stack);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_verify__empty_endorsement__op_check_sig_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig_verify() }, error::op_check_sig_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__empty_endorsement__false_pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__invalid_der_bip66__op_check_sig_decode_signature)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_check_sig_decode_signature);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__invalid_der_no_rules__false_pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::no_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__zero_of_zero_null_dummy__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__zero_of_zero_null_dummy__true_pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__nonnull_dummy_bip147__op_check_multisig_verify9)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify9);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__nonnull_dummy_no_rules__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::no_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__empty_stack__op_check_multisig_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__twenty_one_keys__op_check_multisig_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_signed64(0);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(21);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__operation_count_exceeded__op_check_multisig_verify3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE(machine->ops_increment(199));
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__invalid_signature_count__op_check_multisig_verify5)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x02, 0x03, 0x04, 0x05 });
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify5);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__more_signatures_than_keys__op_check_multisig_verify6)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_signed64(1);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_verify6);
}

// op_check_multisig maps each verify code through its own disjunction.

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__empty_stack__op_check_multisig_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__twenty_one_keys__op_check_multisig_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_signed64(0);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(21);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__operation_count_exceeded__op_check_multisig_verify3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE(machine->ops_increment(199));
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(3);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__invalid_signature_count__op_check_multisig_verify5)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x02, 0x03, 0x04, 0x05 });
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify5);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__more_signatures_than_keys__op_check_multisig_verify6)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_signed64(1);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify6);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__nonnull_dummy_bip147__op_check_multisig_verify9)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });
    machine->push_signed64(0);
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_verify9);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__invalid_der_no_rules__false_pushed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::no_rules };
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x01, 0x01 });
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__invalid_der_bip66__op_check_multisig_decode_signature)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x01, 0x01 });
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_check_multisig_decode_signature);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__no_rules__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::no_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__final_input__op_check_locktime_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__empty_stack__op_check_locktime_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 42 };
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__negative_stack__op_check_locktime_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 42 };
    machine->push_signed64(-1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__type_mismatch__op_check_locktime_verify3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 42 };
    machine->push_signed64(500000000);
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify3);
}

// A transaction locktime equal to the threshold is a timestamp, not a height.
BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__transaction_locktime_threshold__op_check_locktime_verify3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 500000000 };
    machine->push_signed64(0);
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__exceeded__op_check_locktime_verify4)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 42 };
    machine->push_signed64(43);
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_check_locktime_verify4);
}

BC_AUTO_TEST_CASE(interpreter__op_check_locktime_verify__equal__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0, 42 };
    machine->push_signed64(42);
    BC_REQUIRE_EQUAL(code{ machine->op_check_locktime_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__no_rules__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::no_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__empty_stack__op_check_sequence_verify1)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 2 };
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_check_sequence_verify1);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__disabled_stack_sequence__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 2 };
    machine->push_signed64(0x80000000);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__version_one__op_check_sequence_verify2)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 1 };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_check_sequence_verify2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__disabled_input_sequence__op_check_sequence_verify3)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 0x80000000, 0, 2 };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_check_sequence_verify3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__type_mismatch__op_check_sequence_verify4)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 2 };
    machine->push_signed64(0x00400001);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_check_sequence_verify4);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__exceeded__op_check_sequence_verify5)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 2 };
    machine->push_signed64(6);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_check_sequence_verify5);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sequence_verify__equal__op_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules, 5, 0, 2 };
    machine->push_signed64(5);
    BC_REQUIRE_EQUAL(code{ machine->op_check_sequence_verify() }, error::op_success);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__input_program__op_reserved)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig_add() }, error::op_reserved);
}

// Tapscript execution.
// ----------------------------------------------------------------------------

constexpr uint64_t prevout_value = 42u;
constexpr auto taproot_rules = flags::bip141_rule | flags::bip143_rule |
    flags::bip341_rule | flags::bip342_rule;

// A non-empty signature charges the budget. A key that is neither empty nor
// 32 bytes is an unknown (upgradable) type, for which signature validation is
// considered successful, so no valid signature is required here [bip342].
static const data_chunk dummy_signature{ 0x01 };
static const data_chunk unknown_key(add1(ec_xonly_size), 0x02);

// <sig> <key> OP_CHECKSIGVERIFY per sigop (37 bytes each), then OP_1.
static script to_budget_script(size_t sigops) NOEXCEPT
{
    operations ops{};
    for (size_t sigop{}; sigop < sigops; ++sigop)
    {
        ops.emplace_back(dummy_signature, false);
        ops.emplace_back(unknown_key, false);
        ops.emplace_back(opcode::checksigverify);
    }

    ops.emplace_back(opcode::push_positive_1);
    return script{ std::move(ops) };
}

// The tx spending the tapscript. The signature hash does not cover the witness
// (apart from an annex), so a tx built with an empty witness produces the same
// hashes as one carrying the signatures over which those hashes are taken.
static transaction to_spending_transaction(const chunk_cptrs& stack) NOEXCEPT
{
    // A non-null point, so the tx is not taken for a coinbase.
    const point outpoint{ one_hash, 0u };
    const witness spender{ stack };
    const chain::inputs ins{ input{ outpoint, script{}, spender, 0xffffffff } };
    const chain::outputs outs{ output{ prevout_value, script{} } };
    const transaction tx{ 1u, ins, outs, 0u };

    const auto prevout = to_shared<output>(output{ prevout_value, script{} });
    tx.inputs_ptr()->front()->prevout = prevout;
    return tx;
}

// Run a tapscript exactly as connect_witness does: the input carries the full
// witness, while the program is handed the stack with the control block and
// leaf script already popped. Elements are the witness stack-elements, in
// witness order (the last is the top of the execution stack).
static code run_tapscript(const script& leaf, const chunk_cptrs& elements={}) NOEXCEPT
{
    // Witness of the input: [elements] <script> <control>.
    const data_chunk control(add1(ec_xonly_size), tapscript_version);
    auto stack = elements;
    stack.push_back(to_shared<data_chunk>(leaf.to_data(false)));
    stack.push_back(to_shared<data_chunk>(control));

    const auto tx = to_spending_transaction(stack);
    const auto in = tx.inputs_ptr()->cbegin();

    // Execution stack is the witness less the control block and leaf script.
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto execution = emplace_shared<chunk_cptrs>(elements);
    const auto leaf_ptr = to_shared<script>(leaf);
    constexpr auto version = script_version::taproot;
    const signatures capture{};

    interpreter<contiguous_stack> program
    {
        tx,
        in,
        leaf_ptr,
        taproot_rules,
        version,
        execution,
        tapleaf,
        capture
    };

    return program.run();
}

static code run_budget_script(size_t sigops) NOEXCEPT
{
    return run_tapscript(to_budget_script(sigops));
}

// Two sigops against a 75 byte leaf script. The full witness serializes to 111
// bytes, so the budget is 162 and two sigops (100) fit. Deriving the budget
// from the empty execution stack instead yields 51, which stops at one sigop.
BC_AUTO_TEST_CASE(interpreter__run__tapscript_budget_from_full_witness__success)
{
    BC_REQUIRE_EQUAL(run_budget_script(2), error::script_success);
}

// Six sigops (300) still fit a 223 byte leaf script, whose witness serializes
// to 259 bytes for a budget of 310.
BC_AUTO_TEST_CASE(interpreter__run__tapscript_budget_at_limit__success)
{
    BC_REQUIRE_EQUAL(run_budget_script(6), error::script_success);
}

// Seven sigops (350) exceed the 349 budget of a 260 byte leaf script, whose
// witness serializes to 298 bytes. The budget remains enforced.
BC_AUTO_TEST_CASE(interpreter__run__tapscript_budget_exceeded__op_check_sig_budget)
{
    BC_REQUIRE_EQUAL(run_budget_script(7), error::op_check_sig_budget);
}

// <sig> <0> <key> OP_CHECKSIGADD <1> OP_NUMEQUAL.
// Success pushes number+1, so the script is true only when the sigop passed.
static script to_checksigadd_script(const data_chunk& key, const data_chunk& endorsement) NOEXCEPT
{
    operations ops{};
    ops.emplace_back(endorsement, false);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(key, false);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(opcode::push_positive_1);
    ops.emplace_back(opcode::numequal);
    return script{ std::move(ops) };
}

// 64 bytes plus an undefined sighash byte (zero must be implicit) [bip341].
static data_chunk to_undefined_sighash_endorsement() NOEXCEPT
{
    data_chunk endorsement(add1(ec_signature_size), 0x11);
    endorsement.back() = 0x04;
    return endorsement;
}

// An endorsement of neither 64 nor 65 bytes is not a schnorr signature, but
// with an unknown key type it is never parsed, so the sigop passes.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_unknown_key_unparsable_endorsement__success)
{
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(unknown_key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::script_success);
}

// Likewise an undefined sighash byte does not encumber an unknown key type.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_unknown_key_undefined_sighash__success)
{
    const auto endorsement = to_undefined_sighash_endorsement();
    const auto leaf = to_checksigadd_script(unknown_key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::script_success);
}

// An empty key is invalid for any endorsement [bip342].
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_empty_key__op_check_sig_add2)
{
    const data_chunk key{};
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add2);
}

// A 32 byte key remains subject to endorsement parsing.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_unparsable_endorsement__op_check_sig_add4)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add4);
}

// A 32 byte key remains subject to the defined sighash type rule.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_undefined_sighash__op_check_sig_add4)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const auto endorsement = to_undefined_sighash_endorsement();
    const auto leaf = to_checksigadd_script(key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add4);
}

// A 32 byte key remains subject to signature verification.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_invalid_signature__op_check_sig_add6)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk endorsement(ec_signature_size, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add6);
}

// OP_0 <key> OP_CHECKSIGADD OP_DROP OP_CODESEPARATOR OP_0 <key> OP_CHECKSIGADD
// Both sigops take their signature from the witness. The second signs over the
// codeseparator position of 4, the first over none (0xffffffff). The offset is
// the op following the codeseparator, from which that position is derived.
constexpr size_t after_codeseparator = 5;

static script to_codeseparator_script(const ec_xonly& key) NOEXCEPT
{
    const data_chunk point{ key.cbegin(), key.cend() };
    return script
    {
        operations
        {
            { opcode::push_size_0 },
            { point, false },
            { opcode::checksigadd },
            { opcode::drop },
            { opcode::codeseparator },
            { opcode::push_size_0 },
            { point, false },
            { opcode::checksigadd }
        }
    };
}

BC_AUTO_TEST_CASE(interpreter__run__tapscript_codeseparator_signature_hash__success)
{
    // Any valid secret, with its bip340 x-only point.
    const auto secret = base16_hash("b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da56a784d9045190cfef");
    ec_compressed compressed{};
    BC_REQUIRE(secret_to_public(compressed, secret));

    const auto x_only = std::next(compressed.data());
    const auto key = unsafe_array_cast<uint8_t, ec_xonly_size>(x_only);
    const auto leaf = to_codeseparator_script(key);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    constexpr auto version = script_version::taproot;
    constexpr auto sighash = coverage::hash_default;

    // The signature hash is taken over a tx without the signatures, which it
    // does not cover, and is identical to that of the tx that carries them.
    const auto tx = to_spending_transaction({});
    const auto in = tx.inputs_ptr()->cbegin();

    // The first sigop executes before any codeseparator.
    hash_digest first{};
    leaf.clear_offset();
    BC_REQUIRE(tx.signature_hash(first, in, leaf, prevout_value, tapleaf, version, sighash, taproot_rules));

    // The second executes after the codeseparator, which the interpreter
    // records by advancing the offset to the op following it.
    hash_digest second{};
    leaf.offset = std::next(leaf.ops().cbegin(), after_codeseparator);
    BC_REQUIRE(tx.signature_hash(second, in, leaf, prevout_value, tapleaf, version, sighash, taproot_rules));
    leaf.clear_offset();

    // The codeseparator position is the only difference between them.
    BC_REQUIRE_NE(first, second);

    // Sign each with the implied SIGHASH_DEFAULT (64 byte, no sighash byte).
    ec_signature first_signature{};
    ec_signature second_signature{};
    BC_REQUIRE(schnorr::sign(first_signature, secret, first, one_hash));
    BC_REQUIRE(schnorr::sign(second_signature, secret, second, one_hash));

    // Witness elements, the last of which is the top of the execution stack.
    const data_chunk second_chunk{ second_signature.cbegin(), second_signature.cend() };
    const data_chunk first_chunk{ first_signature.cbegin(), first_signature.cend() };
    const chunk_cptrs elements{ to_shared<data_chunk>(second_chunk), to_shared<data_chunk>(first_chunk) };

    // Both signatures verify only if the second sigop takes its own hash. A
    // cache retained across the codeseparator hands it the first instead.
    BC_REQUIRE_EQUAL(run_tapscript(leaf, elements), error::script_success);
}

BC_AUTO_TEST_CASE(interpreter__run__tapscript_checksig_after_codeseparator__success)
{
    const auto secret = base16_hash("b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da56a784d9045190cfef");
    ec_compressed compressed{};
    BC_REQUIRE(secret_to_public(compressed, secret));

    const auto x_only = std::next(compressed.data());
    const auto key = unsafe_array_cast<uint8_t, ec_xonly_size>(x_only);
    const data_chunk point{ key.begin(), key.end() };
    const script leaf{ operations{ { opcode::codeseparator }, { point, false }, { opcode::checksig } } };
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto tx = to_spending_transaction({});
    const auto in = tx.inputs_ptr()->begin();

    // The checksig executes after the codeseparator at position zero.
    hash_digest sighash{};
    leaf.offset = std::next(leaf.ops().begin());
    BC_REQUIRE(tx.signature_hash(sighash, in, leaf, prevout_value, tapleaf, script_version::taproot, coverage::hash_default, taproot_rules));

    leaf.clear_offset();
    ec_signature signature{};
    BC_REQUIRE(schnorr::sign(signature, secret, sighash, one_hash));

    const data_chunk signature_chunk{ signature.cbegin(), signature.cend() };
    BC_REQUIRE_EQUAL(run_tapscript(leaf, { to_shared<data_chunk>(signature_chunk) }), error::script_success);
}

// Tapscript program state (via accessor).
// ----------------------------------------------------------------------------

static script to_op_success_script() NOEXCEPT
{
    return script{ operations{ operation{ opcode::reserved_80 } } };
}

static script to_minimal_leaf_script() NOEXCEPT
{
    return script{ operations{ operation{ opcode::push_positive_1 } } };
}

static chunk_cptrs to_tapscript_witness(const script& leaf) NOEXCEPT
{
    const data_chunk control(add1(ec_xonly_size), tapscript_version);
    chunk_cptrs stack{};
    stack.push_back(to_shared<data_chunk>(leaf.to_data(false)));
    stack.push_back(to_shared<data_chunk>(control));
    return stack;
}

BC_AUTO_TEST_CASE(interpreter__initialize__tapscript_op_success__prevalid_script)
{
    const auto leaf = to_op_success_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.initialize() == error::prevalid_script);
}

BC_AUTO_TEST_CASE(interpreter__op_if__tapscript_nonminimal__op_if2)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ accessor.op_if() }, error::op_if2);
}

BC_AUTO_TEST_CASE(interpreter__op_notif__tapscript_nonminimal__op_notif2)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ accessor.op_notif() }, error::op_notif2);
}

BC_AUTO_TEST_CASE(interpreter__op_if__tapscript_minimal_true__op_success)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk{ 0x01 });
    BC_REQUIRE_EQUAL(code{ accessor.op_if() }, error::op_success);
    BC_REQUIRE(accessor.is_success());
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__tapscript__op_reserved)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE_EQUAL(code{ accessor.op_check_multisig() }, error::op_reserved);
    BC_REQUIRE_EQUAL(code{ accessor.op_check_multisig_verify() }, error::op_reserved);
}

// Operation dispatch (run_op).
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__run_op__push_size_0__pushes_number_zero)
{
    const script leaf{ operations{ operation{ opcode::push_size_0 } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__run_op__push_positive_1__pushes_number_one)
{
    const script leaf{ operations{ operation{ opcode::push_positive_1 } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_success);

    int32_t value{};
    BC_REQUIRE(machine->pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 1);
}

BC_AUTO_TEST_CASE(interpreter__run_op__push_size__pushes_payload)
{
    const script leaf{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_success);
    BC_REQUIRE_EQUAL(*machine->pop_chunk_(), base16_chunk("0102"));
}

BC_AUTO_TEST_CASE(interpreter__run_op__nop__op_success)
{
    const script leaf{ operations{ operation{ opcode::nop } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__run_op__dup_on_empty_stack__op_dup)
{
    const script leaf{ operations{ operation{ opcode::dup } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_dup);
}

BC_AUTO_TEST_CASE(interpreter__run_op__invalid_opcode__op_invalid)
{
    const script leaf{ operations{ operation{ opcode::op_cat } } };
    machine_accessor<contiguous_stack> machine{ leaf, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->run_op(machine->begin()) }, error::op_invalid);
}

// Signature operation control flow (mocked verification).
// ----------------------------------------------------------------------------

using mocked = mock_program<contiguous_stack>;

BC_AUTO_TEST_CASE(interpreter__op_check_sig__mocked_valid_signature__true_pushed)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__mocked_invalid_signature__false_pushed)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->ecdsa_result = false;
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_verify__mocked_valid_signature__op_success)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig_verify() }, error::op_success);
    BC_REQUIRE(machine->is_stack_empty());
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_verify__mocked_invalid_signature__op_check_sig_verify4)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->ecdsa_result = false;
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig_verify() }, error::op_check_sig_verify4);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_verify__mocked_hash_failure__op_check_sig_verify3)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->hash_result = false;
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x02 });
    BC_REQUIRE_EQUAL(code{ machine->op_check_sig_verify() }, error::op_check_sig_verify3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__mocked_one_of_one__true_pushed)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_success);
    BC_REQUIRE_EQUAL(machine->stack_size(), 1u);
    BC_REQUIRE(machine->peek_bool_());
}

// A failed verification advances to the next key, not the next endorsement.
BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__mocked_one_of_two_first_key_unmatched__op_success)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->ecdsa_failures = 1;
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x03 });
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_success);
}

// CONSENSUS: endorsements must be ordered as their keys.
BC_AUTO_TEST_CASE(interpreter__op_check_multisig_verify__mocked_out_of_order__op_check_multisig_mismatch)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->ecdsa_failures = 1;
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_chunk(data_chunk{ 0x30, 0x02 });
    machine->push_signed64(2);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_chunk(data_chunk{ 0x03 });
    machine->push_signed64(2);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig_verify() }, error::op_check_multisig_mismatch);
}

BC_AUTO_TEST_CASE(interpreter__op_check_multisig__mocked_mismatch__false_pushed)
{
    machine_accessor<contiguous_stack, mocked> machine{ {}, flags::all_rules };
    machine->ecdsa_result = false;
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x30, 0x01 });
    machine->push_signed64(1);
    machine->push_chunk(data_chunk{ 0x02 });
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_check_multisig() }, error::op_success);
    BC_REQUIRE(!machine->peek_bool_());
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__mocked_valid_signature__incremented)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack, mocked> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk(ec_signature_size, 0x11));
    accessor.push_signed64(0);
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_add() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(accessor.pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 1);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__mocked_hash_failure__op_check_sig_add5)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack, mocked> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.hash_result = false;
    accessor.push_chunk(data_chunk(ec_signature_size, 0x11));
    accessor.push_signed64(0);
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_add() }, error::op_check_sig_add5);
}

// This witness serializes to 37 bytes, affording exactly one sigop [bip342].
BC_AUTO_TEST_CASE(interpreter__sigops_increment__budget_boundary__expected)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE_EQUAL((*tx.inputs_ptr()->front()).witness().serialized_size(true), 37u);
    BC_REQUIRE(accessor.sigops_increment());
    BC_REQUIRE(!accessor.sigops_increment());
}

// Numeric operand underflow codes.
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(interpreter__op_nonzero__empty_stack__op_nonzero)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BC_REQUIRE_EQUAL(code{ machine->op_nonzero() }, error::op_nonzero);
}

BC_AUTO_TEST_CASE(interpreter__op_bool_and__one_operand__op_bool_and)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_and() }, error::op_bool_and);
}

BC_AUTO_TEST_CASE(interpreter__op_bool_or__one_operand__op_bool_or)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_bool_or() }, error::op_bool_or);
}

BC_AUTO_TEST_CASE(interpreter__op_num_equal__one_operand__op_num_equal)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_num_equal() }, error::op_num_equal);
}

BC_AUTO_TEST_CASE(interpreter__op_num_not_equal__one_operand__op_num_not_equal)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_num_not_equal() }, error::op_num_not_equal);
}

BC_AUTO_TEST_CASE(interpreter__op_less_than__one_operand__op_less_than)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than() }, error::op_less_than);
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than__one_operand__op_greater_than)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than() }, error::op_greater_than);
}

BC_AUTO_TEST_CASE(interpreter__op_less_than_or_equal__one_operand__op_less_than_or_equal)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_less_than_or_equal() }, error::op_less_than_or_equal);
}

BC_AUTO_TEST_CASE(interpreter__op_greater_than_or_equal__one_operand__op_greater_than_or_equal)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_greater_than_or_equal() }, error::op_greater_than_or_equal);
}

BC_AUTO_TEST_CASE(interpreter__op_min__one_operand__op_min)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_min() }, error::op_min);
}

BC_AUTO_TEST_CASE(interpreter__op_max__one_operand__op_max)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BC_REQUIRE_EQUAL(code{ machine->op_max() }, error::op_max);
}

// Tapscript checksig failure codes [bip342].
// ----------------------------------------------------------------------------

static script to_checksig_script(const data_chunk& key, const data_chunk& endorsement, opcode checksig) NOEXCEPT
{
    return script{ operations{ operation{ endorsement, false }, operation{ key, false }, operation{ checksig } } };
}

BC_AUTO_TEST_CASE(interpreter__run__checksigadd_low_stack__op_check_sig_add1)
{
    const script leaf{ operations{ operation{ opcode::checksigadd } } };
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add1);
}

// The number operand is limited to four bytes.
BC_AUTO_TEST_CASE(interpreter__run__checksigadd_five_byte_number__op_check_sig_add3)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk number{ 0x00, 0x00, 0x00, 0x00, 0x01 };
    const script leaf{ operations{ operation{ data_chunk{}, false }, operation{ number, false }, operation{ key, false }, operation{ opcode::checksigadd } } };
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add3);
}

// An empty key fails the script for any endorsement [bip342].
BC_AUTO_TEST_CASE(interpreter__run__checksig_empty_key__op_check_sig_empty_key)
{
    const data_chunk endorsement(ec_signature_size, 0x11);
    const auto leaf = to_checksig_script({}, endorsement, opcode::checksig);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_empty_key);
}

// An empty endorsement is false, which fails the verify form.
BC_AUTO_TEST_CASE(interpreter__run__checksigverify_empty_endorsement__op_check_sig_verify1)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const auto leaf = to_checksig_script(key, {}, opcode::checksigverify);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_verify1);
}

// A 32 byte key is subject to the defined sighash type rule.
BC_AUTO_TEST_CASE(interpreter__run__checksig_xonly_key_undefined_sighash__op_check_sig_schnorr1)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const auto leaf = to_checksig_script(key, to_undefined_sighash_endorsement(), opcode::checksig);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_schnorr1);
}

// A 32 byte key is subject to signature verification, which fails the script.
BC_AUTO_TEST_CASE(interpreter__run__checksig_xonly_key_invalid_signature__op_check_sig_schnorr3)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk endorsement(ec_signature_size, 0x11);
    const auto leaf = to_checksig_script(key, endorsement, opcode::checksig);
    BC_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_schnorr3);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig_verify__mocked_hash_failure__op_check_sig_schnorr2)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack, mocked> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.hash_result = false;
    accessor.push_chunk(data_chunk(ec_signature_size, 0x11));
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_verify() }, error::op_check_sig_schnorr2);
}

BC_AUTO_TEST_CASE(interpreter__op_check_sig__mocked_hash_failure__op_check_sig_schnorr2)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack, mocked> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.hash_result = false;
    accessor.push_chunk(data_chunk(ec_signature_size, 0x11));
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig() }, error::op_check_sig_schnorr2);
}

// tapscript initial stack and operation limits [bip342]

BC_AUTO_TEST_CASE(interpreter__initialize__tapscript_overflow__stack_size)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto count = add1(chain::max_unified_stack_size);
    const auto element = to_shared<data_chunk>();
    const auto execution = std::make_shared<chunk_cptrs>(count, element);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.initialize() == error::invalid_stack_size);
}

// The primary and alternate stacks share one limit [bip342].
BC_AUTO_TEST_CASE(interpreter__is_stack_overflow__at_limit_then_alternate__expected)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto element = to_shared<data_chunk>();
    const auto execution = std::make_shared<chunk_cptrs>(chain::max_unified_stack_size, element);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.initialize() != error::invalid_stack_size);
    BC_REQUIRE_EQUAL(accessor.stack_size(), chain::max_unified_stack_size);
    BC_REQUIRE(!accessor.is_stack_overflow());

    accessor.push_alternate(stack_variant{ true });
    BC_REQUIRE(accessor.is_stack_overflow());
}

BC_AUTO_TEST_CASE(interpreter__initialize__tapscript_oversized__witness_stack)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const data_chunk big(add1(chain::max_push_data_size), 0x00);
    const auto element = to_shared<data_chunk>(big);
    const auto execution = std::make_shared<chunk_cptrs>(one, element);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.initialize() == error::invalid_witness_stack);
}

BC_AUTO_TEST_CASE(interpreter__ops_increment__tapscript_operation__true)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.ops_increment(operation{ opcode::checksig }));
}

BC_AUTO_TEST_CASE(interpreter__ops_increment__tapscript_public_keys__true)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.ops_increment(add1(chain::max_counted_ops)));
}

// An empty signature leaves the accumulator unchanged and continues [bip342].
BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__empty_endorsement__unchanged)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk{});
    accessor.push_signed64(7);
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_add() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(accessor.pop_signed32(value));
    BC_REQUIRE_EQUAL(value, 7);
}

// An unknown key type skips verification, so the accumulator is incremented.
BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__negative_accumulator__incremented)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    accessor.push_chunk(data_chunk{ 0x30 });
    accessor.push_signed64(-2);
    accessor.push_chunk(data_chunk(ec_compressed_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_add() }, error::op_success);

    int32_t value{};
    BC_REQUIRE(accessor.pop_signed32(value));
    BC_REQUIRE_EQUAL(value, -1);
}

// The sigop budget is 50 per signature, from add1(50) plus witness size [bip342].
BC_AUTO_TEST_CASE(interpreter__op_check_sig_add__budget_exhausted__op_check_sig_add7)
{
    const auto leaf = to_minimal_leaf_script();
    const auto tx = to_spending_transaction(to_tapscript_witness(leaf));
    const auto in = tx.inputs_ptr()->cbegin();
    const auto execution = std::make_shared<chunk_cptrs>();
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    interpreter_accessor<contiguous_stack, mocked> accessor{ tx, in, leaf_ptr, taproot_rules, script_version::taproot, execution, tapleaf, capture };
    BC_REQUIRE(accessor.sigops_increment());
    BC_REQUIRE(!accessor.sigops_increment());

    accessor.push_chunk(data_chunk(ec_signature_size, 0x11));
    accessor.push_signed64(0);
    accessor.push_chunk(data_chunk(ec_xonly_size, 0x02));
    BC_REQUIRE_EQUAL(code{ accessor.op_check_sig_add() }, error::op_check_sig_add7);
}

BC_AUTO_TEST_SUITE_END()
