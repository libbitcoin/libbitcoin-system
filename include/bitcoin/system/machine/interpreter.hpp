/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP

#include <cstdint>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/machine/program.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

class BC_API interpreter
{
public:
    typedef error::op_error_t result;
    typedef program::op_iterator op_iterator;

    // Operations (shared).
    // ------------------------------------------------------------------------

    static result op_unevaluated(chain::opcode) noexcept;
    static result op_nop(const program& program, chain::opcode) noexcept;
    static result op_push_number(program& program, uint8_t value) noexcept;
    static result op_push_size(program& program,
        const chain::operation& op) noexcept;
    static result op_push_data(program& program, const chunk_cptr& data,
        uint32_t size_limit) noexcept;

    // Operations (not shared).
    // ------------------------------------------------------------------------

    static result op_nop(chain::opcode) noexcept;
    static result op_ver(const program& program) noexcept;
    static result op_if(program& program) noexcept;
    static result op_notif(program& program) noexcept;
    static result op_verif(const program& program) noexcept;
    static result op_vernotif(const program& program) noexcept;
    static result op_else(program& program) noexcept;
    static result op_endif(program& program) noexcept;
    static result op_verify(program& program) noexcept;
    static result op_return(const program& program) noexcept;
    static result op_to_alt_stack(program& program) noexcept;
    static result op_from_alt_stack(program& program) noexcept;
    static result op_drop2(program& program) noexcept;
    static result op_dup2(program& program) noexcept;
    static result op_dup3(program& program) noexcept;
    static result op_over2(program& program) noexcept;
    static result op_rot2(program& program) noexcept;
    static result op_swap2(program& program) noexcept;
    static result op_if_dup(program& program) noexcept;
    static result op_depth(program& program) noexcept;
    static result op_drop(program& program) noexcept;
    static result op_dup(program& program) noexcept;
    static result op_nip(program& program) noexcept;
    static result op_over(program& program) noexcept;
    static result op_pick(program& program) noexcept;
    static result op_roll(program& program) noexcept;
    static result op_rot(program& program) noexcept;
    static result op_swap(program& program) noexcept;
    static result op_tuck(program& program) noexcept;
    static result op_cat(const program& program) noexcept;
    static result op_substr(const program& program) noexcept;
    static result op_left(const program& program) noexcept;
    static result op_right(const program& program) noexcept;
    static result op_size(program& program) noexcept;
    static result op_invert(const program& program) noexcept;
    static result op_and(const program& program) noexcept;
    static result op_or(const program& program) noexcept;
    static result op_xor(const program& program) noexcept;
    static result op_equal(program& program) noexcept;
    static result op_equal_verify(program& program) noexcept;
    static result op_add1(program& program) noexcept;
    static result op_sub1(program& program) noexcept;
    static result op_mul2(const program& program) noexcept;
    static result op_div2(const program& program) noexcept;
    static result op_negate(program& program) noexcept;
    static result op_abs(program& program) noexcept;
    static result op_not(program& program) noexcept;
    static result op_nonzero(program& program) noexcept;
    static result op_add(program& program) noexcept;
    static result op_sub(program& program) noexcept;
    static result op_mul(const program& program) noexcept;
    static result op_div(const program& program) noexcept;
    static result op_mod(const program& program) noexcept;
    static result op_lshift(const program& program) noexcept;
    static result op_rshift(const program& program) noexcept;
    static result op_bool_and(program& program) noexcept;
    static result op_bool_or(program& program) noexcept;
    static result op_num_equal(program& program) noexcept;
    static result op_num_equal_verify(program& program) noexcept;
    static result op_num_not_equal(program& program) noexcept;
    static result op_less_than(program& program) noexcept;
    static result op_greater_than(program& program) noexcept;
    static result op_less_than_or_equal(program& program) noexcept;
    static result op_greater_than_or_equal(program& program) noexcept;
    static result op_min(program& program) noexcept;
    static result op_max(program& program) noexcept;
    static result op_within(program& program) noexcept;
    static result op_ripemd160(program& program) noexcept;
    static result op_sha1(program& program) noexcept;
    static result op_sha256(program& program) noexcept;
    static result op_hash160(program& program) noexcept;
    static result op_hash256(program& program) noexcept;
    static result op_codeseparator(program& program,
        const op_iterator& op) noexcept;
    static result op_check_sig_verify(program& program) noexcept;
    static result op_check_sig(program& program) noexcept;
    static result op_check_multisig_verify(program& program) noexcept;
    static result op_check_multisig(program& program) noexcept;
    static result op_check_locktime_verify(const program& program) noexcept;
    static result op_check_sequence_verify(const program& program) noexcept;

    /// Run program script.
    static code run(program& program) noexcept;

private:
    static result run_op(const op_iterator& op, program& program) noexcept;

    // Not constructable (really just a namespace).
    interpreter() = delete;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
