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

BC_AUTO_TEST_SUITE(interpreter_run_tests)

using namespace system::chain;
using namespace system::machine;

// run
// ----------------------------------------------------------------------------
// Dispatch is faked, isolating iteration, counting and conditional scope.

using runner = mock_runner<contiguous_stack>;

BC_AUTO_TEST_CASE(interpreter__run__empty_script__script_success)
{
    const auto tx = accessor_transaction(script{});
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::script_success);
    BC_REQUIRE(machine.dispatched.empty());
}

BC_AUTO_TEST_CASE(interpreter__run__operations__dispatched_in_order)
{
    const operations ops
    {
        operation{ opcode::dup },
        operation{ opcode::drop },
        operation{ opcode::nip }
    };

    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::script_success);
    BC_REQUIRE_EQUAL(machine.dispatched.size(), 3u);
    BC_REQUIRE(machine.dispatched.at(0) == opcode::dup);
    BC_REQUIRE(machine.dispatched.at(1) == opcode::drop);
    BC_REQUIRE(machine.dispatched.at(2) == opcode::nip);
}

BC_AUTO_TEST_CASE(interpreter__run__dispatch_error__returned_and_terminal)
{
    const operations ops{ operation{ opcode::dup }, operation{ opcode::drop } };
    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    machine.op_result = error::op_dup;
    BC_REQUIRE_EQUAL(machine.run(), error::op_dup);
    BC_REQUIRE_EQUAL(machine.dispatched.size(), 1u);
}

BC_AUTO_TEST_CASE(interpreter__run__oversized_push__invalid_push_data_size)
{
    const data_chunk data(add1(max_push_data_size), 0x00_u8);
    const operations ops{ operation{ data, false } };
    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::invalid_push_data_size);
    BC_REQUIRE(machine.dispatched.empty());
}

BC_AUTO_TEST_CASE(interpreter__run__above_op_count__invalid_operation_count)
{
    const operations ops(add1(max_counted_ops), operation{ opcode::nop });
    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::invalid_operation_count);
    BC_REQUIRE_EQUAL(machine.dispatched.size(), max_counted_ops);
}

BC_AUTO_TEST_CASE(interpreter__run__negative_condition__unconditional_ops_skipped)
{
    const operations ops
    {
        operation{ opcode::if_ },
        operation{ opcode::dup },
        operation{ opcode::endif }
    };

    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::script_success);
    BC_REQUIRE_EQUAL(machine.dispatched.size(), 2u);
    BC_REQUIRE(machine.dispatched.at(0) == opcode::if_);
    BC_REQUIRE(machine.dispatched.at(1) == opcode::endif);
}

BC_AUTO_TEST_CASE(interpreter__run__unbalanced_condition__invalid_stack_scope)
{
    const operations ops{ operation{ opcode::if_ } };
    const auto tx = accessor_transaction(script{ ops });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    runner machine{ tx, it, flags::all_rules, capture };
    BC_REQUIRE_EQUAL(machine.run(), error::invalid_stack_scope);
    BC_REQUIRE_EQUAL(machine.dispatched.size(), 1u);
}

// run_op
// ----------------------------------------------------------------------------
// Handlers are faked, isolating the opcode to handler mapping.

using dispatcher = mock_handlers<contiguous_stack>;

static std::string_view dispatch(const operation& op, int8_t& number,
    opcode& code) NOEXCEPT
{
    const auto tx = accessor_transaction(script{ operations{ op } });
    const auto it = tx.inputs_ptr()->cbegin();
    const signatures capture{};
    dispatcher machine{ tx, it, flags::all_rules, capture };
    /* op_error_t */ machine.run_op(machine.begin());
    number = machine.number;
    code = machine.code;
    return machine.handler;
}

static std::string_view dispatch(const operation& op) NOEXCEPT
{
    int8_t number{};
    opcode code{};
    return dispatch(op, number, code);
}

static int8_t dispatch_number(const operation& op) NOEXCEPT
{
    int8_t number{};
    opcode code{};
    /* handler */ dispatch(op, number, code);
    return number;
}

BC_AUTO_TEST_CASE(interpreter__run_op__push_data_codes__expected_handlers)
{
    const operation size_1{ data_chunk{ 0x01 }, false };
    const operation size_75{ data_chunk(75, 0x00_u8), false };
    const operation one_size{ data_chunk(76, 0x00_u8), false };
    const operation two_size{ data_chunk(256, 0x00_u8), false };
    const operation four_size{ base16_chunk("4e0400000001020304") };
    BC_REQUIRE_EQUAL(dispatch(size_1), "op_push_size");
    BC_REQUIRE_EQUAL(dispatch(size_75), "op_push_size");
    BC_REQUIRE_EQUAL(dispatch(one_size), "op_push_one_size");
    BC_REQUIRE_EQUAL(dispatch(two_size), "op_push_two_size");
    BC_REQUIRE_EQUAL(dispatch(four_size), "op_push_four_size");
}

BC_AUTO_TEST_CASE(interpreter__run_op__push_number_codes__expected_values)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::push_size_0 }), "op_push_number");
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_size_0 }), 0);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_negative_1 }), -1);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_1 }), 1);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_2 }), 2);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_3 }), 3);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_4 }), 4);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_5 }), 5);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_6 }), 6);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_7 }), 7);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_8 }), 8);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_9 }), 9);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_10 }), 10);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_11 }), 11);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_12 }), 12);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_13 }), 13);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_14 }), 14);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_15 }), 15);
    BC_REQUIRE_EQUAL(dispatch_number(operation{ opcode::push_positive_16 }), 16);
}

BC_AUTO_TEST_CASE(interpreter__run_op__conditional_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::if_ }), "op_if");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::notif }), "op_notif");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_verif }), "op_verif");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_vernotif }), "op_vernotif");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::else_ }), "op_else");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::endif }), "op_endif");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::verify }), "op_verify");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_return }), "op_return");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_ver }), "op_ver");
}

BC_AUTO_TEST_CASE(interpreter__run_op__stack_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::toaltstack }), "op_to_alt_stack");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::fromaltstack }), "op_from_alt_stack");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::drop2 }), "op_drop2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::dup2 }), "op_dup2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::dup3 }), "op_dup3");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::over2 }), "op_over2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::rot2 }), "op_rot2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::swap2 }), "op_swap2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::ifdup }), "op_if_dup");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::depth }), "op_depth");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::drop }), "op_drop");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::dup }), "op_dup");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nip }), "op_nip");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::over }), "op_over");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::pick }), "op_pick");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::roll }), "op_roll");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::rot }), "op_rot");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::swap }), "op_swap");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::tuck }), "op_tuck");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::size }), "op_size");
}

BC_AUTO_TEST_CASE(interpreter__run_op__splice_and_bitwise_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_cat }), "op_cat");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_substr }), "op_substr");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_left }), "op_left");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_right }), "op_right");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_invert }), "op_invert");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_and }), "op_and");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_or }), "op_or");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_xor }), "op_xor");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::equal }), "op_equal");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::equalverify }), "op_equal_verify");
}

BC_AUTO_TEST_CASE(interpreter__run_op__arithmetic_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::add1 }), "op_add1");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::sub1 }), "op_sub1");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_mul2 }), "op_mul2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_div2 }), "op_div2");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::negate }), "op_negate");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::abs }), "op_abs");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::not_ }), "op_not");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nonzero }), "op_nonzero");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::add }), "op_add");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::sub }), "op_sub");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_mul }), "op_mul");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_div }), "op_div");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_mod }), "op_mod");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_lshift }), "op_lshift");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::op_rshift }), "op_rshift");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::booland }), "op_bool_and");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::boolor }), "op_bool_or");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::numequal }), "op_num_equal");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::numequalverify }), "op_num_equal_verify");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::numnotequal }), "op_num_not_equal");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::lessthan }), "op_less_than");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::greaterthan }), "op_greater_than");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::lessthanorequal }), "op_less_than_or_equal");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::greaterthanorequal }), "op_greater_than_or_equal");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::min }), "op_min");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::max }), "op_max");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::within }), "op_within");
}

BC_AUTO_TEST_CASE(interpreter__run_op__crypto_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::ripemd160 }), "op_ripemd160");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::sha1 }), "op_sha1");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::sha256 }), "op_sha256");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::hash160 }), "op_hash160");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::hash256 }), "op_hash256");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::codeseparator }), "op_codeseparator");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checksig }), "op_check_sig");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checksigverify }), "op_check_sig_verify");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checkmultisig }), "op_check_multisig");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checkmultisigverify }), "op_check_multisig_verify");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checksigadd }), "op_check_sig_add");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checklocktimeverify }), "op_check_locktime_verify");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::checksequenceverify }), "op_check_sequence_verify");
}

BC_AUTO_TEST_CASE(interpreter__run_op__nop_codes__expected_handlers)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop }), "op_nop");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop1 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop4 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop5 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop6 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop7 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop8 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop9 }), "op_nop_code");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::nop10 }), "op_nop_code");
}

BC_AUTO_TEST_CASE(interpreter__run_op__reserved_codes__op_unevaluated)
{
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::reserved_80 }), "op_unevaluated");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::reserved_137 }), "op_unevaluated");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::reserved_138 }), "op_unevaluated");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::reserved_187 }), "op_unevaluated");
    BC_REQUIRE_EQUAL(dispatch(operation{ opcode::reserved_255 }), "op_unevaluated");
}

BC_AUTO_TEST_SUITE_END()

