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
    DELETE5(interpreter);

    using state = program<Stack>;
    using op_iterator = typename state::op_iterator;
    using input_iterator = typename input_cptrs::const_iterator;

    /// Use program constructors.
    using program<Stack>::program;

    /// Run a program.
    inline code run() NOEXCEPT;

    /// Connect tx.input[#].script to tx.input[#].prevout.script.
    static code connect(const context& state, const transaction& tx,
        uint32_t index) NOEXCEPT;

    /// Connect tx.input[*].script to tx.input[*].prevout.script.
    static code connect(const context& state, const transaction& tx,
        const input_iterator& it) NOEXCEPT;

protected:
    /// Operation disatch.
    inline error::op_error_t run_op(const op_iterator& op) NOEXCEPT;

    /// Operation handlers.
    inline error::op_error_t op_unevaluated(chain::opcode) const NOEXCEPT;
    inline error::op_error_t op_nop(chain::opcode) const NOEXCEPT;
    inline error::op_error_t op_push_number(int8_t value) NOEXCEPT;
    inline error::op_error_t op_push_size(const chain::operation& op) NOEXCEPT;
    inline error::op_error_t op_push_one_size(const chain::operation& op) NOEXCEPT;
    inline error::op_error_t op_push_two_size(const chain::operation& op) NOEXCEPT;
    inline error::op_error_t op_push_four_size(const chain::operation& op) NOEXCEPT;
    inline error::op_error_t op_nop() const NOEXCEPT;
    inline error::op_error_t op_ver() const NOEXCEPT;
    inline error::op_error_t op_if() NOEXCEPT;
    inline error::op_error_t op_notif() NOEXCEPT;
    inline error::op_error_t op_verif() const NOEXCEPT;
    inline error::op_error_t op_vernotif() const NOEXCEPT;
    inline error::op_error_t op_else() NOEXCEPT;
    inline error::op_error_t op_endif() NOEXCEPT;
    inline error::op_error_t op_verify() NOEXCEPT;
    inline error::op_error_t op_return() const NOEXCEPT;
    inline error::op_error_t op_to_alt_stack() NOEXCEPT;
    inline error::op_error_t op_from_alt_stack() NOEXCEPT;
    inline error::op_error_t op_drop2() NOEXCEPT;
    inline error::op_error_t op_dup2() NOEXCEPT;
    inline error::op_error_t op_dup3() NOEXCEPT;
    inline error::op_error_t op_over2() NOEXCEPT;
    inline error::op_error_t op_rot2() NOEXCEPT;
    inline error::op_error_t op_swap2() NOEXCEPT;
    inline error::op_error_t op_if_dup() NOEXCEPT;
    inline error::op_error_t op_depth() NOEXCEPT;
    inline error::op_error_t op_drop() NOEXCEPT;
    inline error::op_error_t op_dup() NOEXCEPT;
    inline error::op_error_t op_nip() NOEXCEPT;
    inline error::op_error_t op_over() NOEXCEPT;
    inline error::op_error_t op_pick() NOEXCEPT;
    inline error::op_error_t op_roll() NOEXCEPT;
    inline error::op_error_t op_rot() NOEXCEPT;
    inline error::op_error_t op_swap() NOEXCEPT;
    inline error::op_error_t op_tuck() NOEXCEPT;
    inline error::op_error_t op_cat() const NOEXCEPT;
    inline error::op_error_t op_substr() const NOEXCEPT;
    inline error::op_error_t op_left() const NOEXCEPT;
    inline error::op_error_t op_right() const NOEXCEPT;
    inline error::op_error_t op_size() NOEXCEPT;
    inline error::op_error_t op_invert() const NOEXCEPT;
    inline error::op_error_t op_and() const NOEXCEPT;
    inline error::op_error_t op_or() const NOEXCEPT;
    inline error::op_error_t op_xor() const NOEXCEPT;
    inline error::op_error_t op_equal() NOEXCEPT;
    inline error::op_error_t op_equal_verify() NOEXCEPT;
    inline error::op_error_t op_add1() NOEXCEPT;
    inline error::op_error_t op_sub1() NOEXCEPT;
    inline error::op_error_t op_mul2() const NOEXCEPT;
    inline error::op_error_t op_div2() const NOEXCEPT;
    inline error::op_error_t op_negate() NOEXCEPT;
    inline error::op_error_t op_abs() NOEXCEPT;
    inline error::op_error_t op_not() NOEXCEPT;
    inline error::op_error_t op_nonzero() NOEXCEPT;
    inline error::op_error_t op_add() NOEXCEPT;
    inline error::op_error_t op_sub() NOEXCEPT;
    inline error::op_error_t op_mul() const NOEXCEPT;
    inline error::op_error_t op_div() const NOEXCEPT;
    inline error::op_error_t op_mod() const NOEXCEPT;
    inline error::op_error_t op_lshift() const NOEXCEPT;
    inline error::op_error_t op_rshift() const NOEXCEPT;
    inline error::op_error_t op_bool_and() NOEXCEPT;
    inline error::op_error_t op_bool_or() NOEXCEPT;
    inline error::op_error_t op_num_equal() NOEXCEPT;
    inline error::op_error_t op_num_equal_verify() NOEXCEPT;
    inline error::op_error_t op_num_not_equal() NOEXCEPT;
    inline error::op_error_t op_less_than() NOEXCEPT;
    inline error::op_error_t op_greater_than() NOEXCEPT;
    inline error::op_error_t op_less_than_or_equal() NOEXCEPT;
    inline error::op_error_t op_greater_than_or_equal() NOEXCEPT;
    inline error::op_error_t op_min() NOEXCEPT;
    inline error::op_error_t op_max() NOEXCEPT;
    inline error::op_error_t op_within() NOEXCEPT;
    inline error::op_error_t op_ripemd160() NOEXCEPT;
    inline error::op_error_t op_sha1() NOEXCEPT;
    inline error::op_error_t op_sha256() NOEXCEPT;
    inline error::op_error_t op_hash160() NOEXCEPT;
    inline error::op_error_t op_hash256() NOEXCEPT;
    inline error::op_error_t op_codeseparator(const op_iterator& op) NOEXCEPT;
    inline error::op_error_t op_check_sig_verify() NOEXCEPT;
    inline error::op_error_t op_check_sig() NOEXCEPT;
    inline error::op_error_t op_check_multisig_verify() NOEXCEPT;
    inline error::op_error_t op_check_multisig() NOEXCEPT;
    inline error::op_error_t op_check_locktime_verify() const NOEXCEPT;
    inline error::op_error_t op_check_sequence_verify() const NOEXCEPT;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/interpreter.ipp>

#endif
