/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__MACHINE_INTERPRETER_HPP
#define LIBBITCOIN__MACHINE_INTERPRETER_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/machine/interpreter.hpp>
#include <bitcoin/machine_opcode.hpp>
#include <bitcoin/machine_operation.hpp>
#include <bitcoin/machine_program.hpp>
#include <bitcoin/utility_data_chunk.hpp>

namespace libbitcoin {
//namespace machine {
namespace api {

class BC_API machine_interpreter : public machine::interpreter
{
public:
//    typedef error::error_code_t result;

    // Operations (shared).
    //-------------------------------------------------------------------------

    static result op_nop(machine_opcode);
    static result op_disabled(machine_opcode);
    static result op_reserved(machine_opcode);
    static result op_push_number(machine_program& program, uint8_t value);
    static result op_push_size(machine_program& program, const machine_operation& op);
    static result op_push_data(machine_program& program, const utility_data_chunk& data,
        uint32_t size_limit);

    // Operations (not shared).
    //-------------------------------------------------------------------------

    static result op_if(machine_program& program);
    static result op_notif(machine_program& program);
    static result op_else(machine_program& program);
    static result op_endif(machine_program& program);
    static result op_verify(machine_program& program);
    static result op_return(machine_program& program);
    static result op_to_alt_stack(machine_program& program);
    static result op_from_alt_stack(machine_program& program);
    static result op_drop2(machine_program& program);
    static result op_dup2(machine_program& program);
    static result op_dup3(machine_program& program);
    static result op_over2(machine_program& program);
    static result op_rot2(machine_program& program);
    static result op_swap2(machine_program& program);
    static result op_if_dup(machine_program& program);
    static result op_depth(machine_program& program);
    static result op_drop(machine_program& program);
    static result op_dup(machine_program& program);
    static result op_nip(machine_program& program);
    static result op_over(machine_program& program);
    static result op_pick(machine_program& program);
    static result op_roll(machine_program& program);
    static result op_rot(machine_program& program);
    static result op_swap(machine_program& program);
    static result op_tuck(machine_program& program);
    static result op_size(machine_program& program);
    static result op_equal(machine_program& program);
    static result op_equal_verify(machine_program& program);
    static result op_add1(machine_program& program);
    static result op_sub1(machine_program& program);
    static result op_negate(machine_program& program);
    static result op_abs(machine_program& program);
    static result op_not(machine_program& program);
    static result op_nonzero(machine_program& program);
    static result op_add(machine_program& program);
    static result op_sub(machine_program& program);
    static result op_bool_and(machine_program& program);
    static result op_bool_or(machine_program& program);
    static result op_num_equal(machine_program& program);
    static result op_num_equal_verify(machine_program& program);
    static result op_num_not_equal(machine_program& program);
    static result op_less_than(machine_program& program);
    static result op_greater_than(machine_program& program);
    static result op_less_than_or_equal(machine_program& program);
    static result op_greater_than_or_equal(machine_program& program);
    static result op_min(machine_program& program);
    static result op_max(machine_program& program);
    static result op_within(machine_program& program);
    static result op_ripemd160(machine_program& program);
    static result op_sha1(machine_program& program);
    static result op_sha256(machine_program& program);
    static result op_hash160(machine_program& program);
    static result op_hash256(machine_program& program);
    static result op_codeseparator(machine_program& program, const machine_operation& op);
    static result op_check_sig_verify(machine_program& program);
    static result op_check_sig(machine_program& program);
    static result op_check_multisig_verify(machine_program& program);
    static result op_check_multisig(machine_program& program);
    static result op_check_locktime_verify(machine_program& program);
    static result op_check_sequence_verify(machine_program& program);

    /// Run program script.
    static error_code run(machine_program& program);

    /// Run individual operations (independent of the script).
    /// For best performance use script runner for a sequence of operations.
    static error_code run(const machine_operation& op, machine_program& program);

//private:
//    static result run_op(const machine_operation& op, machine_program& program);
};

} // namespace api
//} // namespace machine
} // namespace libbitcoin

//#include <bitcoin/bitcoin/impl/machine/interpreter.ipp>

#endif
