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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_INTERPRETER_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/machine/program.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

/// Class to isolate operation iteration, dispatch, and handlers from state.
template <typename Stack>
class interpreter
  : public program<Stack>
{
public:
    DELETE_COPY_MOVE_DESTRUCT(interpreter);

    using state = program<Stack>;
    using op_iterator = typename state::op_iterator;
    using input_iterator = chain::input_cptrs::const_iterator;

    /// Use program constructors.
    using program<Stack>::program;

    /// Run a program.
    code run() NOEXCEPT;

    /// Connect tx.input[#].script to tx.input[#].prevout.script.
    static code connect(const chain::context& state,
        const chain::transaction& tx, uint32_t index) NOEXCEPT;

    /// Connect tx.input[*].script to tx.input[*].prevout.script.
    static code connect(const chain::context& state,
        const chain::transaction& tx, const input_iterator& it) NOEXCEPT;

protected:
    using flags = chain::flags;
    using opcode = chain::opcode;
    using operation = chain::operation;
    using op_error_t = error::op_error_t;

    /// Embedded script handler.
    static code connect_embedded(const chain::context& state,
        const chain::transaction& tx, const input_iterator& it,
        interpreter& in_program) NOEXCEPT;

    /// Witnessed script handler.
    static code connect_witness(const chain::context& state,
        const chain::transaction& tx, const input_iterator& it,
        const chain::script& prevout) NOEXCEPT;

    /// Operation disatch.
    op_error_t run_op(const op_iterator& op) NOEXCEPT;

    /// Operation handlers.
    inline op_error_t op_unevaluated(opcode) const NOEXCEPT;
    inline op_error_t op_nop(opcode) const NOEXCEPT;
    inline op_error_t op_push_number(int8_t value) NOEXCEPT;
    inline op_error_t op_push_size(const operation& op) NOEXCEPT;
    inline op_error_t op_push_one_size(const operation& op) NOEXCEPT;
    inline op_error_t op_push_two_size(const operation& op) NOEXCEPT;
    inline op_error_t op_push_four_size(const operation& op) NOEXCEPT;
    inline op_error_t op_nop() const NOEXCEPT;
    inline op_error_t op_ver() const NOEXCEPT;
    inline op_error_t op_if() NOEXCEPT;
    inline op_error_t op_notif() NOEXCEPT;
    inline op_error_t op_verif() const NOEXCEPT;
    inline op_error_t op_vernotif() const NOEXCEPT;
    inline op_error_t op_else() NOEXCEPT;
    inline op_error_t op_endif() NOEXCEPT;
    inline op_error_t op_verify() NOEXCEPT;
    inline op_error_t op_return() const NOEXCEPT;
    inline op_error_t op_to_alt_stack() NOEXCEPT;
    inline op_error_t op_from_alt_stack() NOEXCEPT;
    inline op_error_t op_drop2() NOEXCEPT;
    inline op_error_t op_dup2() NOEXCEPT;
    inline op_error_t op_dup3() NOEXCEPT;
    inline op_error_t op_over2() NOEXCEPT;
    inline op_error_t op_rot2() NOEXCEPT;
    inline op_error_t op_swap2() NOEXCEPT;
    inline op_error_t op_if_dup() NOEXCEPT;
    inline op_error_t op_depth() NOEXCEPT;
    inline op_error_t op_drop() NOEXCEPT;
    inline op_error_t op_dup() NOEXCEPT;
    inline op_error_t op_nip() NOEXCEPT;
    inline op_error_t op_over() NOEXCEPT;
    inline op_error_t op_pick() NOEXCEPT;
    inline op_error_t op_roll() NOEXCEPT;
    inline op_error_t op_rot() NOEXCEPT;
    inline op_error_t op_swap() NOEXCEPT;
    inline op_error_t op_tuck() NOEXCEPT;
    inline op_error_t op_cat() const NOEXCEPT;
    inline op_error_t op_substr() const NOEXCEPT;
    inline op_error_t op_left() const NOEXCEPT;
    inline op_error_t op_right() const NOEXCEPT;
    inline op_error_t op_size() NOEXCEPT;
    inline op_error_t op_invert() const NOEXCEPT;
    inline op_error_t op_and() const NOEXCEPT;
    inline op_error_t op_or() const NOEXCEPT;
    inline op_error_t op_xor() const NOEXCEPT;
    inline op_error_t op_equal() NOEXCEPT;
    inline op_error_t op_equal_verify() NOEXCEPT;
    inline op_error_t op_add1() NOEXCEPT;
    inline op_error_t op_sub1() NOEXCEPT;
    inline op_error_t op_mul2() const NOEXCEPT;
    inline op_error_t op_div2() const NOEXCEPT;
    inline op_error_t op_negate() NOEXCEPT;
    inline op_error_t op_abs() NOEXCEPT;
    inline op_error_t op_not() NOEXCEPT;
    inline op_error_t op_nonzero() NOEXCEPT;
    inline op_error_t op_add() NOEXCEPT;
    inline op_error_t op_sub() NOEXCEPT;
    inline op_error_t op_mul() const NOEXCEPT;
    inline op_error_t op_div() const NOEXCEPT;
    inline op_error_t op_mod() const NOEXCEPT;
    inline op_error_t op_lshift() const NOEXCEPT;
    inline op_error_t op_rshift() const NOEXCEPT;
    inline op_error_t op_bool_and() NOEXCEPT;
    inline op_error_t op_bool_or() NOEXCEPT;
    inline op_error_t op_num_equal() NOEXCEPT;
    inline op_error_t op_num_equal_verify() NOEXCEPT;
    inline op_error_t op_num_not_equal() NOEXCEPT;
    inline op_error_t op_less_than() NOEXCEPT;
    inline op_error_t op_greater_than() NOEXCEPT;
    inline op_error_t op_less_than_or_equal() NOEXCEPT;
    inline op_error_t op_greater_than_or_equal() NOEXCEPT;
    inline op_error_t op_min() NOEXCEPT;
    inline op_error_t op_max() NOEXCEPT;
    inline op_error_t op_within() NOEXCEPT;
    inline op_error_t op_ripemd160() NOEXCEPT;
    inline op_error_t op_sha1() NOEXCEPT;
    inline op_error_t op_sha256() NOEXCEPT;
    inline op_error_t op_hash160() NOEXCEPT;
    inline op_error_t op_hash256() NOEXCEPT;
    inline op_error_t op_codeseparator(const op_iterator& op) NOEXCEPT;
    inline op_error_t op_check_sig() NOEXCEPT;
    inline op_error_t op_check_sig_verify() NOEXCEPT;
    inline op_error_t op_check_multisig_verify() NOEXCEPT;
    inline op_error_t op_check_multisig() NOEXCEPT;
    inline op_error_t op_check_locktime_verify() const NOEXCEPT;
    inline op_error_t op_check_sequence_verify() const NOEXCEPT;
    inline op_error_t op_check_sig_add() NOEXCEPT;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Stack>
#define CLASS interpreter<Stack>

#include <bitcoin/system/impl/machine/interpreter.ipp>

#undef CLASS
#undef TEMPLATE

#endif
