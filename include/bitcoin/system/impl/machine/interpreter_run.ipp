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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_RUN_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_RUN_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>

namespace libbitcoin {
namespace system {
namespace machine {
    
// Run the program.
// ----------------------------------------------------------------------------

TEMPLATE
code CLASS::
run() NOEXCEPT
{
    // Enforce initial limits, determine early success or failure.
    if (const auto ec = state::initialize())
        return (ec == error::prevalid_script) ? error::script_success : ec;

    for (auto it = state::begin(); it != state::end(); ++it)
    {
        const auto& op = *it;

        if (op.is_oversized())
            return error::invalid_push_data_size;

        if (!state::ops_increment(op))
            return error::invalid_operation_count;

        if (state::if_(op))
        {
            if (const auto ec = run_op(it))
                return ec;

            if (state::is_stack_overflow())
                return error::invalid_stack_size;
        }
    }

    if (!state::is_balanced())
        return error::invalid_stack_scope;

    return error::script_success;
}

// Operation disatch.
// ----------------------------------------------------------------------------
// It is expected that the compiler will produce a very efficient jump table.

// protected
TEMPLATE
error::op_error_t CLASS::
run_op(const op_iterator& op) NOEXCEPT
{
    const auto code = op->code();

    switch (code)
    {
        case opcode::push_size_0:
            return op_push_number(0);
        case opcode::push_size_1:
        case opcode::push_size_2:
        case opcode::push_size_3:
        case opcode::push_size_4:
        case opcode::push_size_5:
        case opcode::push_size_6:
        case opcode::push_size_7:
        case opcode::push_size_8:
        case opcode::push_size_9:
        case opcode::push_size_10:
        case opcode::push_size_11:
        case opcode::push_size_12:
        case opcode::push_size_13:
        case opcode::push_size_14:
        case opcode::push_size_15:
        case opcode::push_size_16:
        case opcode::push_size_17:
        case opcode::push_size_18:
        case opcode::push_size_19:
        case opcode::push_size_20:
        case opcode::push_size_21:
        case opcode::push_size_22:
        case opcode::push_size_23:
        case opcode::push_size_24:
        case opcode::push_size_25:
        case opcode::push_size_26:
        case opcode::push_size_27:
        case opcode::push_size_28:
        case opcode::push_size_29:
        case opcode::push_size_30:
        case opcode::push_size_31:
        case opcode::push_size_32:
        case opcode::push_size_33:
        case opcode::push_size_34:
        case opcode::push_size_35:
        case opcode::push_size_36:
        case opcode::push_size_37:
        case opcode::push_size_38:
        case opcode::push_size_39:
        case opcode::push_size_40:
        case opcode::push_size_41:
        case opcode::push_size_42:
        case opcode::push_size_43:
        case opcode::push_size_44:
        case opcode::push_size_45:
        case opcode::push_size_46:
        case opcode::push_size_47:
        case opcode::push_size_48:
        case opcode::push_size_49:
        case opcode::push_size_50:
        case opcode::push_size_51:
        case opcode::push_size_52:
        case opcode::push_size_53:
        case opcode::push_size_54:
        case opcode::push_size_55:
        case opcode::push_size_56:
        case opcode::push_size_57:
        case opcode::push_size_58:
        case opcode::push_size_59:
        case opcode::push_size_60:
        case opcode::push_size_61:
        case opcode::push_size_62:
        case opcode::push_size_63:
        case opcode::push_size_64:
        case opcode::push_size_65:
        case opcode::push_size_66:
        case opcode::push_size_67:
        case opcode::push_size_68:
        case opcode::push_size_69:
        case opcode::push_size_70:
        case opcode::push_size_71:
        case opcode::push_size_72:
        case opcode::push_size_73:
        case opcode::push_size_74:
        case opcode::push_size_75:
            return op_push_size(*op);
        case opcode::push_one_size:
            return op_push_one_size(*op);
        case opcode::push_two_size:
            return op_push_two_size(*op);
        case opcode::push_four_size:
            return op_push_four_size(*op);
        case opcode::push_negative_1:
            return op_push_number(-1);
        case opcode::reserved_80:
            return op_unevaluated(code);
        case opcode::push_positive_1:
            return op_push_number(1);
        case opcode::push_positive_2:
            return op_push_number(2);
        case opcode::push_positive_3:
            return op_push_number(3);
        case opcode::push_positive_4:
            return op_push_number(4);
        case opcode::push_positive_5:
            return op_push_number(5);
        case opcode::push_positive_6:
            return op_push_number(6);
        case opcode::push_positive_7:
            return op_push_number(7);
        case opcode::push_positive_8:
            return op_push_number(8);
        case opcode::push_positive_9:
            return op_push_number(9);
        case opcode::push_positive_10:
            return op_push_number(10);
        case opcode::push_positive_11:
            return op_push_number(11);
        case opcode::push_positive_12:
            return op_push_number(12);
        case opcode::push_positive_13:
            return op_push_number(13);
        case opcode::push_positive_14:
            return op_push_number(14);
        case opcode::push_positive_15:
            return op_push_number(15);
        case opcode::push_positive_16:
            return op_push_number(16);
        case opcode::nop:
            return op_nop();
        case opcode::op_ver:
            return op_ver();
        case opcode::if_:
            return op_if();
        case opcode::notif:
            return op_notif();
        case opcode::op_verif:
            return op_verif();
        case opcode::op_vernotif:
            return op_vernotif();
        case opcode::else_:
            return op_else();
        case opcode::endif:
            return op_endif();
        case opcode::verify:
            return op_verify();
        case opcode::op_return:
            return op_return();
        case opcode::toaltstack:
            return op_to_alt_stack();
        case opcode::fromaltstack:
            return op_from_alt_stack();
        case opcode::drop2:
            return op_drop2();
        case opcode::dup2:
            return op_dup2();
        case opcode::dup3:
            return op_dup3();
        case opcode::over2:
            return op_over2();
        case opcode::rot2:
            return op_rot2();
        case opcode::swap2:
            return op_swap2();
        case opcode::ifdup:
            return op_if_dup();
        case opcode::depth:
            return op_depth();
        case opcode::drop:
            return op_drop();
        case opcode::dup:
            return op_dup();
        case opcode::nip:
            return op_nip();
        case opcode::over:
            return op_over();
        case opcode::pick:
            return op_pick();
        case opcode::roll:
            return op_roll();
        case opcode::rot:
            return op_rot();
        case opcode::swap:
            return op_swap();
        case opcode::tuck:
            return op_tuck();
        case opcode::op_cat:
            return op_cat();
        case opcode::op_substr:
            return op_substr();
        case opcode::op_left:
            return op_left();
        case opcode::op_right:
            return op_right();
        case opcode::size:
            return op_size();
        case opcode::op_invert:
            return op_invert();
        case opcode::op_and:
            return op_and();
        case opcode::op_or:
            return op_or();
        case opcode::op_xor:
            return op_xor();
        case opcode::equal:
            return op_equal();
        case opcode::equalverify:
            return op_equal_verify();
        case opcode::reserved_137:
            return op_unevaluated(code);
        case opcode::reserved_138:
            return op_unevaluated(code);
        case opcode::add1:
            return op_add1();
        case opcode::sub1:
            return op_sub1();
        case opcode::op_mul2:
            return op_mul2();
        case opcode::op_div2:
            return op_div2();
        case opcode::negate:
            return op_negate();
        case opcode::abs:
            return op_abs();
        case opcode::not_:
            return op_not();
        case opcode::nonzero:
            return op_nonzero();
        case opcode::add:
            return op_add();
        case opcode::sub:
            return op_sub();
        case opcode::op_mul:
            return op_mul();
        case opcode::op_div:
            return op_div();
        case opcode::op_mod:
            return op_mod();
        case opcode::op_lshift:
            return op_lshift();
        case opcode::op_rshift:
            return op_rshift();
        case opcode::booland:
            return op_bool_and();
        case opcode::boolor:
            return op_bool_or();
        case opcode::numequal:
            return op_num_equal();
        case opcode::numequalverify:
            return op_num_equal_verify();
        case opcode::numnotequal:
            return op_num_not_equal();
        case opcode::lessthan:
            return op_less_than();
        case opcode::greaterthan:
            return op_greater_than();
        case opcode::lessthanorequal:
            return op_less_than_or_equal();
        case opcode::greaterthanorequal:
            return op_greater_than_or_equal();
        case opcode::min:
            return op_min();
        case opcode::max:
            return op_max();
        case opcode::within:
            return op_within();
        case opcode::ripemd160:
            return op_ripemd160();
        case opcode::sha1:
            return op_sha1();
        case opcode::sha256:
            return op_sha256();
        case opcode::hash160:
            return op_hash160();
        case opcode::hash256:
            return op_hash256();
        case opcode::codeseparator:
            return op_codeseparator(op);
        case opcode::checksig:
            return op_check_sig();
        case opcode::checksigverify:
            return op_check_sig_verify();
        case opcode::checkmultisig:
            return op_check_multisig();
        case opcode::checkmultisigverify:
            return op_check_multisig_verify();
        case opcode::nop1:
            return op_nop(code);
        case opcode::checklocktimeverify:
            return op_check_locktime_verify();
        case opcode::checksequenceverify:
            return op_check_sequence_verify();
        case opcode::nop4:
        case opcode::nop5:
        case opcode::nop6:
        case opcode::nop7:
        case opcode::nop8:
        case opcode::nop9:
        case opcode::nop10:
            return op_nop(code);
        case opcode::checksigadd:
            return op_check_sig_add();
        default:
            return op_unevaluated(code);
    }
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
